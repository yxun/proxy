# Copyright 2021 The Bazel Authors. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Partial implementation for Swift frameworks with third party interfaces."""

load(
    "@build_bazel_rules_apple//apple/internal:processor.bzl",
    "processor",
)
load(
    "@build_bazel_rules_apple//apple/internal:swift_info_support.bzl",
    "swift_info_support",
)
load(
    "@bazel_skylib//lib:partial.bzl",
    "partial",
)
load(
    "@bazel_skylib//lib:paths.bzl",
    "paths",
)

def _swift_framework_partial_impl(
        *,
        actions,
        bundle_name,
        label_name,
        output_discriminator,
        swift_infos):
    """Implementation for the Swift framework processing partial."""

    if len(swift_infos) == 0:
        fail("""
Internal error: Expected to find a SwiftInfo before entering this partial. Please file an \
issue with a reproducible error case.
""")

    bundle_files = []
    expected_module_name = bundle_name
    found_module_name = None
    found_generated_header = None
    modules_parent = paths.join("Modules", "{}.swiftmodule".format(expected_module_name))

    for arch, swiftinfo in swift_infos.items():
        swift_module = swift_info_support.swift_include_info(
            found_module_name = found_module_name,
            transitive_modules = swiftinfo.transitive_modules,
        )

        # If headers are generated, they should be generated equally for all archs, so just take the
        # first one found.
        if (not found_generated_header) and swift_module.clang:
            if swift_module.clang.compilation_context.direct_headers:
                found_generated_header = swift_module.clang.compilation_context.direct_headers[0]

        found_module_name = swift_module.name

        bundle_interface = swift_info_support.declare_swiftinterface(
            actions = actions,
            arch = arch,
            label_name = label_name,
            output_discriminator = output_discriminator,
            swiftinterface = swift_module.swift.swiftinterface,
        )
        bundle_files.append((processor.location.bundle, modules_parent, depset([bundle_interface])))

        bundle_doc = swift_info_support.declare_swiftdoc(
            actions = actions,
            arch = arch,
            label_name = label_name,
            output_discriminator = output_discriminator,
            swiftdoc = swift_module.swift.swiftdoc,
        )
        bundle_files.append((processor.location.bundle, modules_parent, depset([bundle_doc])))

    swift_info_support.verify_found_module_name(
        bundle_name = expected_module_name,
        found_module_name = found_module_name,
    )

    if found_generated_header:
        bundle_header = swift_info_support.declare_generated_header(
            actions = actions,
            generated_header = found_generated_header,
            label_name = label_name,
            output_discriminator = output_discriminator,
            module_name = expected_module_name,
        )
        bundle_files.append((processor.location.bundle, "Headers", depset([bundle_header])))

        modulemap = swift_info_support.declare_modulemap(
            actions = actions,
            label_name = label_name,
            output_discriminator = output_discriminator,
            module_name = expected_module_name,
        )
        bundle_files.append((processor.location.bundle, "Modules", depset([modulemap])))

    return struct(bundle_files = bundle_files)

def swift_framework_partial(
        *,
        actions,
        bundle_name,
        label_name,
        output_discriminator = None,
        swift_infos):
    """Constructor for the Swift framework processing partial.

    This partial collects and bundles the necessary files to construct a Swift based static
    framework.

    Args:
        actions: The actions provider from `ctx.actions`.
        bundle_name: The name of the output bundle.
        label_name: Name of the target being built.
        output_discriminator: A string to differentiate between different target intermediate files
            or `None`.
        swift_infos: A dictionary with architectures as keys and the SwiftInfo provider containing
            the required artifacts for that architecture as values.

    Returns:
        A partial that returns the bundle location of the supporting Swift artifacts needed in a
        Swift based sdk framework.
    """
    return partial.make(
        _swift_framework_partial_impl,
        actions = actions,
        bundle_name = bundle_name,
        label_name = label_name,
        output_discriminator = output_discriminator,
        swift_infos = swift_infos,
    )
