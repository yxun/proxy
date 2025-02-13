# rules_pkg - reference

<div class="toc">
  <h2>Rules</h2>
  <ul>
    <li><a href="#pkg_tar">pkg_tar</a></li>
    <li><a href="#pkg_zip">pkg_zip</a></li>
    <li><a href="#pkg_deb">pkg_deb</a></li>
    <li><a href="#pkg_rpm">pkg_rpm</a></li>
  </ul>

</div>

<a name="common"></a>
## Common Attributes

These attributes are used in several rules within this module.

**ATTRIBUTES**

| Name              | Description                                                                                                                                                                     | Type                                                               | Mandatory       | Default                                   |
| :-------------    | :-------------                                                                                                                                                                  | :-------------:                                                    | :-------------: | :-------------                            |
| out               | Name of the output file. This file will always be created and used to access the package content. If `package_file_name` is also specified, `out` will be a symlink.            | String                                                             | required        |                                           |
| package_file_name | The name of the file which will contain the package. The name may contain variables in the form `{var}`. The values for substitution are specified through `package_variables`. | String                                                             | optional        | package type specific                     |
| package_variables | A target that provides `PackageVariablesInfo` to substitute into `package_file_name`.                                                                                           | <a href="https://bazel.build/docs/build-ref.html#labels">Label</a> | optional        | None                                      |
| attributes        | Attributes to set on entities created within packages.  Not to be confused with bazel rule attributes.  See 'Mapping "Attributes"' below                                        | Undefined.                                                         | optional        | Varies.  Consult individual rule documentation for details. |

See
[examples/naming_package_files](https://github.com/bazelbuild/rules_pkg/tree/main/examples/naming_package_files)
for examples of how `out`, `package_file_name`, and `package_variables`
interact.

<a name="mapping-attrs"></a>
### Mapping "Attributes"

The "attributes" attribute specifies properties of package contents as used in
rules such as `pkg_files`, and `pkg_mkdirs`.  These allow fine-grained control
of the contents of your package.  For example:

```python
attributes = pkg_attributes(
    mode = "0644",
    user = "root",
    group = "wheel",
    my_custom_attribute = "some custom value",
)
```

`mode`, `user`, and `group` correspond to common UNIX-style filesystem
permissions.  Attributes should always be specified using the `pkg_attributes`
helper macro.

Each mapping rule has some default mapping attributes.  At this time, the only
default is "mode", which will be set if it is not otherwise overridden by the user.

If `user` and `group` are not specified, then defaults for them will be chosen
by the underlying package builder.  Any specific behavior from package builders
should not be relied upon.

Any other attributes should be specified as additional arguments to
`pkg_attributes`.

There are currently no other well-known attributes.

---

<a name="pkg_tar"></a>
## pkg_tar

```python
pkg_tar(name, extension, strip_prefix, package_dir, srcs, compressor,
        compressor_args, mode, modes, deps, symlinks, package_file_name,
        package_variables)
```

Creates a tar file from a list of inputs.

<table class="table table-condensed table-bordered table-params">
  <colgroup>
    <col class="col-param" />
    <col class="param-description" />
  </colgroup>
  <thead>
    <tr>
      <th colspan="2">Attributes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>name</code></td>
      <td>
        <code>Name, required</code>
        <p>A unique name for this rule.</p>
      </td>
    </tr>
    <tr>
      <td><code>extension</code></td>
      <td>
        <code>String, default to 'tar'</code>
        <p>
            The extension for the resulting tarball. The output
            file will be '<i>name</i>.<i>extension</i>'. This extension
            also decide on the compression: if set to <code>tar.gz</code>
            or <code>tgz</code> then gzip compression will be used and
            if set to <code>tar.bz2</code> or <code>tar.bzip2</code> then
            bzip2 compression will be used.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>strip_prefix</code></td>
      <td>
        <code>String, optional</code>
        <p>Root path of the files.</p>
        <p>
          The directory structure from the files is preserved inside the
          tarball but a prefix path determined by <code>strip_prefix</code>
          is removed from the directory structure. This path can
          be absolute from the workspace root if starting with a <code>/</code> or
          relative to the rule's directory. A relative path may start with "./"
          (or be ".") but cannot use ".." to go up level(s). By default, the
          <code>strip_prefix</code> attribute is unused and all files are supposed to have no
          prefix. A <code>strip_prefix</code> of "" (the empty string) means the
          same as the default.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>package_dir</code></td>
      <td>
        <code>String, optional</code>
        <p>Target directory.</p>
        <p>
          The directory in which to expand the specified files, defaulting to '/'.
          Only makes sense accompanying files.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>srcs</code></td>
      <td>
        <code>List of files, optional</code>
        <p>File to add to the layer.</p>
        <p>
          A list of files that should be included in the archive.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>compressor</code></td>
      <td>
        <code>Label, optional</code>
        <p>
          Executable to be built and used as part of a custom compression filter.
          For example, to compress with <code>pigz -p 4</code>, use <code>"@pigz"</code> here
          (assuming a workspace rule named "pigz" exists).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>compressor_args</code></td>
      <td>
        <code>String, optional</code>
        <p>
          Arguments to be passed to <code>compressor</code>.
          For example, to compress with <code>pigz -p 4</code>, use <code>"-p 4"</code> here.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>mode</code></td>
      <td>
        <code>String, default to 0555</code>
        <p>
          Set the mode of files added by the <code>srcs</code> attribute.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>mtime</code></td>
      <td>
        <code>int, seconds since Jan 1, 1970, default to -1 (ignored)</code>
        <p>
          Set the modification time of files added by the <code>srcs</code> attribute.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>portable_mtime</code></td>
      <td>
        <code>bool, default True</code>
        <p>
          Set the modification time of files added by the <code>srcs</code> attribute
          to a 2000-01-01.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>modes</code></td>
      <td>
        <code>Dictionary, default to '{}'</code>
        <p>
          A string dictionary to change default mode of specific files from
          <code>srcs</code>. Each key should be a path to a file before
          appending the prefix <code>package_dir</code> and the corresponding
          value the octal permission of to apply to the file.
        </p>
        <p>
          <code>
          modes = {
           "tools/py/2to3.sh": "0755",
           ...
          },
          </code>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>owner</code></td>
      <td>
        <code>String, default to '0.0'</code>
        <p>
          <code>UID.GID</code> to set the default numeric owner for all files
          provided in <code>srcs</code>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>owners</code></td>
      <td>
        <code>Dictionary, default to '{}'</code>
        <p>
          A string dictionary to change default owner of specific files from
          <code>srcs</code>. Each key should be a path to a file before
          appending the prefix <code>package_dir</code> and the corresponding
          value the <code>UID.GID</code> numeric string for the owner of the
          file. When determining owner ids, this attribute is looked first then
          <code>owner</code>.
        </p>
        <p>
          <code>
          owners = {
           "tools/py/2to3.sh": "42.24",
           ...
          },
          </code>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>ownername</code></td>
      <td>
        <code>String, optional</code>
        <p>
          <code>username.groupname</code> to set the default owner for all files
          provided in <code>srcs</code> (by default there is no owner names).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>ownernames</code></td>
      <td>
        <code>Dictionary, default to '{}'</code>
        <p>
          A string dictionary to change default owner of specific files from
          <code>srcs</code>. Each key should be a path to a file before
          appending the prefix <code>package_dir</code> and the corresponding
          value the <code>username.groupname</code> string for the owner of the
          file. When determining ownernames, this attribute is looked first then
          <code>ownername</code>.
        </p>
        <p>
          <code>
          owners = {
           "tools/py/2to3.sh": "leeroy.jenkins",
           ...
          },
          </code>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>deps</code></td>
      <td>
        <code>List of labels, optional</code>
        <p>Tar files to extract and include in this tar package.</p>
        <p>
          A list of tarball labels to merge into the output tarball.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>stamp</code></td>
      <td>
        <code>Integer; optional; default is -1</code>
        <p>Enable file time stamping.  Possible values:<ul>
          <li>stamp = 1: Use the time of the build as the modification time of each file in the archive.</li>
          <li>stamp = 0: Use an "epoch" time for the modification time of each file. This gives good build result caching.</li>
          <li>stamp = -1: Control the chosen modification time using the --[no]stamp flag.</li>
          </ul>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>symlinks</code></td>
      <td>
        <code>Dictionary, optional</code>
        <p>Symlinks to create in the output tarball.</p>
        <p>
          <code>
          symlinks = {
           "/path/to/link": "/path/to/target",
           ...
          },
          </code>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>remap_paths</code></td>
      <td>
        <code>Dictionary, optional</code>
        <p>Source path prefixes to remap in the tarfile.</p>
        <p>
          <code>
          remap_paths = {
           "original/path/prefix": "replaced/path",
           ...
          },
          </code>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>package_file_name</code></td>
      <td>See <a href="#common">Common Attributes</a></td>
    </tr>
    <tr>
      <td><code>package_variables</code></td>
      <td>See <a href="#common">Common Attributes</a></td>
    </tr>
  </tbody>
</table>

<a name="pkg_zip"></a>
## pkg_zip

```python
pkg_zip(name, extension, package_dir, srcs, timestamp, package_file_name,
package_variables)
```

Creates a zip file from a list of inputs.

<table class="table table-condensed table-bordered table-params">
  <colgroup>
    <col class="col-param" />
    <col class="param-description" />
  </colgroup>
  <thead>
    <tr>
      <th colspan="2">Attributes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>name</code></td>
      <td>
        <code>Name, required</code>
        <p>A unique name for this rule.</p>
      </td>
    </tr>
    <tr>
      <td><code>extension</code></td>
      <td>
        <code>String, default to 'zip'</code>
        <p>
            <b>Deprecated. Use <code>out</code> or <code>package_file_name</code> to specify the output file name.</b>
            The extension for the resulting zipfile. The output
            file will be '<i>name</i>.<i>extension</i>'.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>package_dir</code></td>
      <td>
        <code>String, default to '/'</code>
        <p>Target directory inside zip.</p>
        <p>
          The prefix of all paths in the zip.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>srcs</code></td>
      <td>
        <code>List of files, optional</code>
        <p>File to add to the layer.</p>
        <p>
          A list of files that should be included in the archive.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>stamp</code></td>
      <td>
        <code>Integer; optional; default is -1</code>
        <p>Enable file time stamping.  Possible values:<ul>
          <li>stamp = 1: Use the time of the build as the modification time of each file in the archive.</li>
          <li>stamp = 0: Use an "epoch" time for the modification time of each file. This gives good build result caching.</li>
          <li>stamp = -1: Control the chosen modification time using the --[no]stamp flag.</li>
          </ul>
        </p>
      </td>
    </tr>
    <tr>
      <td><code>timestamp</code></td>
      <td>
        <code>Integer, default to 315532800</code>
        <p>
          The time to use for every file in the zip, expressed as seconds since
          Unix Epoch, RFC 3339.
        </p>
        <p>
          Due to limitations in the format of zip files, values before
          Jan 1, 1980 will be rounded up and the precision in the zip file is
          limited to a granularity of 2 seconds.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>package_file_name</code></td>
      <td>See <a href="#common">Common Attributes</a></td>
    </tr>
    <tr>
      <td><code>package_variables</code></td>
      <td>See <a href="#common">Common Attributes</a></td>
    </tr>
  </tbody>
</table>

<a name="pkg_deb"></a>
### pkg_deb

```python
pkg_deb(name, data, package, architecture, maintainer, preinst, postinst, prerm, postrm,
        version, version_file, description, description_file, built_using, built_using_file,
        priority, section, homepage, depends, suggests, enhances, breaks, conflicts,
        predepends, recommends, replaces, package_file_name, package_variables)
```

Create a debian package. See <a
href="http://www.debian.org/doc/debian-policy/ch-controlfields.html">http://www.debian.org/doc/debian-policy/ch-controlfields.html</a>
for more details on this.

<table class="table table-condensed table-bordered table-params">
  <colgroup>
    <col class="col-param" />
    <col class="param-description" />
  </colgroup>
  <thead>
    <tr>
      <th colspan="2">Attributes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>name</code></td>
      <td>
        <code>Name, required</code>
        <p>A unique name for this rule.</p>
      </td>
    </tr>
    <tr>
      <td><code>data</code></td>
      <td>
        <code>File, required</code>
        <p>
          A tar file that contains the data for the debian package (basically
          the list of files that will be installed by this package).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>package</code></td>
      <td>
        <code>String, required</code>
        <p>The name of the package.</p>
      </td>
    </tr>
    <tr>
      <td><code>architecture</code></td>
      <td>
        <code>String, default to 'all'</code>
        <p>The architecture that this package target.</p>
        <p>
          See <a href="http://www.debian.org/ports/">http://www.debian.org/ports/</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>maintainer</code></td>
      <td>
        <code>String, required</code>
        <p>The maintainer of the package.</p>
      </td>
    </tr>
    <tr>
      <td><code>preinst</code>, <code>postinst</code>, <code>prerm</code> and <code>postrm</code></td>
      <td>
        <code>Files, optional</code>
        <p>
          Respectively, the pre-install, post-install, pre-remove and
          post-remove scripts for the package.
        </p>
        <p>
          See <a href="http://www.debian.org/doc/debian-policy/ch-maintainerscripts.html">http://www.debian.org/doc/debian-policy/ch-maintainerscripts.html</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>config</code></td>
      <td>
        <code>File, optional</code>
        <p>
          config file used for debconf integration.
        </p>
        <p>
          See <a href="https://www.debian.org/doc/debian-policy/ch-binary.html#prompting-in-maintainer-scripts">https://www.debian.org/doc/debian-policy/ch-binary.html#prompting-in-maintainer-scripts</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>templates</code></td>
      <td>
        <code>File, optional</code>
        <p>
          templates file used for debconf integration.
        </p>
        <p>
          See <a href="https://www.debian.org/doc/debian-policy/ch-binary.html#prompting-in-maintainer-scripts">https://www.debian.org/doc/debian-policy/ch-binary.html#prompting-in-maintainer-scripts</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>triggers</code></td>
      <td>
        <code>File, optional</code>
        <p>
          triggers file for configuring installation events exchanged by packages.
        </p>
        <p>
          See <a href="https://wiki.debian.org/DpkgTriggers">https://wiki.debian.org/DpkgTriggers</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>conffiles</code>, <code>conffiles_file</code></td>
      <td>
        <code>String list or File, optional</code>
        <p>
          The list of conffiles or a file containing one conffile per
          line. Each item is an absolute path on the target system
          where the deb is installed.
        </p>
        <p>
          See <a href="https://www.debian.org/doc/debian-policy/ch-files.html#s-config-files">https://www.debian.org/doc/debian-policy/ch-files.html#s-config-files</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>version</code>, <code>version_file</code></td>
      <td>
        <code>String or File, required</code>
        <p>
          The package version provided either inline (with <code>version</code>)
          or from a file (with <code>version_file</code>).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>description</code>, <code>description_file</code></td>
      <td>
        <code>String or File, required</code>
        <p>
          The package description provided either inline (with <code>description</code>)
          or from a file (with <code>description_file</code>).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>built_using</code>, <code>built_using_file</code></td>
      <td>
        <code>String or File</code>
        <p>
          The tool that were used to build this package provided either inline
          (with <code>built_using</code>) or from a file (with <code>built_using_file</code>).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>priority</code></td>
      <td>
        <code>String, default to 'optional'</code>
        <p>The priority of the package.</p>
        <p>
          See <a href="http://www.debian.org/doc/debian-policy/ch-archive.html#s-priorities">http://www.debian.org/doc/debian-policy/ch-archive.html#s-priorities</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>section</code></td>
      <td>
        <code>String, default to 'contrib/devel'</code>
        <p>The section of the package.</p>
        <p>
          See <a href="http://www.debian.org/doc/debian-policy/ch-archive.html#s-subsections">http://www.debian.org/doc/debian-policy/ch-archive.html#s-subsections</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>homepage</code></td>
      <td>
        <code>String, optional</code>
        <p>The homepage of the project.</p>
      </td>
    </tr>
    <tr>
      <td>
        <code>breaks</code>, <code>depends</code>, <code>suggests</code>,
        <code>enhances</code>, <code>conflicts</code>, <code>predepends</code>,
        <code>recommends</code>, <code>replaces</code> and <code>provides</code>.
      </td>
      <td>
        <code>String list, optional</code>
        <p>The list of dependencies in the project.</p>
        <p>
          See <a href="http://www.debian.org/doc/debian-policy/ch-relationships.html#s-binarydeps">http://www.debian.org/doc/debian-policy/ch-relationships.html#s-binarydeps</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>package_file_name</code></td>
      <td>See <a href="#common">Common Attributes</a>
      Default: "%{package}-%{version}-%{architecture}.deb"
      </td>
    </tr>
    <tr>
      <td><code>package_variables</code></td>
      <td>See <a href="#common">Common Attributes</a></td>
    </tr>
  </tbody>
</table>

<a name="pkg_rpm"></a>
### pkg_rpm

```python
pkg_rpm(name, spec_file, architecture, version, version_file, changelog, data)
```

Create an RPM package. See <a
href="http://rpm.org/documentation.html">http://rpm.org/documentation.html</a>
for more details on this.

<table class="table table-condensed table-bordered table-params">
  <colgroup>
    <col class="col-param" />
    <col class="param-description" />
  </colgroup>
  <thead>
    <tr>
      <th colspan="2">Attributes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>name</code></td>
      <td>
        <code>Name, required</code>
        <p>A unique name for this rule. Used to name the output package.</p>
      </td>
    </tr>
    <tr>
      <td><code>spec_file</code></td>
      <td>
        <code>File, required</code>
        <p>The RPM specification file used to generate the package.</p>
        <p>
          See <a href="http://ftp.rpm.org/max-rpm/s1-rpm-build-creating-spec-file.html">http://ftp.rpm.org/max-rpm/s1-rpm-build-creating-spec-file.html</a>.
        </p>
      </td>
    </tr>
    <tr>
      <td><code>architecture</code></td>
      <td>
        <code>String, default to 'all'</code>
        <p>The architecture that this package target.</p>
      </td>
    </tr>
    <tr>
      <td><code>version</code>, <code>version_file</code></td>
      <td>
        <code>String or File, required</code>
        <p>
          The package version provided either inline (with <code>version</code>)
          or from a file (with <code>version_file</code>).
        </p>
      </td>
    </tr>
    <tr>
      <td><code>data</code></td>
      <td>
        <code>Files, required</code>
        <p>
          Files to include in the generated package.
        </p>
      </td>
    </tr>
  </tbody>
</table>

<!-- Generated with Stardoc: http://skydoc.bazel.build -->

<a name="#filter_directory"></a>

## filter_directory

<pre>
filter_directory(<a href="#filter_directory-name">name</a>, <a href="#filter_directory-excludes">excludes</a>, <a href="#filter_directory-outdir_name">outdir_name</a>, <a href="#filter_directory-prefix">prefix</a>, <a href="#filter_directory-renames">renames</a>, <a href="#filter_directory-src">src</a>, <a href="#filter_directory-strip_prefix">strip_prefix</a>)
</pre>

Transform directories (TreeArtifacts) using pkg_filegroup-like semantics.

    Effective order of operations:

    1) Files are `exclude`d
    2) `renames` _or_ `strip_prefix` is applied.
    3) `prefix` is applied

    In particular, if a `rename` applies to an individual file, `strip_prefix`
    will not be applied to that particular file.

    Each non-`rename``d path will look like this:

    ```
    $OUTPUT_DIR/$PREFIX/$FILE_WITHOUT_STRIP_PREFIX
    ```

    Each `rename`d path will look like this:

    ```
    $OUTPUT_DIR/$PREFIX/$FILE_RENAMED
    ```

    If an operation cannot be applied (`strip_prefix`) to any component in the
    directory, or if one is unused (`exclude`, `rename`), the underlying command
    will fail.  See the individual attributes for details.


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| excludes |  Files to exclude from the output directory.<br><br>            Each element must refer to an individual file in <code>src</code>.<br><br>            All exclusions must be used.   | List of strings | optional | [] |
| outdir_name |  Name of output directory (otherwise defaults to the rule's name)   | String | optional | "" |
| prefix |  Prefix to add to all paths in the output directory.<br><br>            This does not include the output directory name, which will be added             regardless.   | String | optional | "" |
| renames |  Files to rename in the output directory.<br><br>            Keys are destinations, values are sources prior to any path             modifications (e.g. via <code>prefix</code> or <code>strip_prefix</code>).  Files that are             <code>exclude</code>d must not be renamed.<br><br>            This currently only operates on individual files.  <code>strip_prefix</code>             does not apply to them.<br><br>            All renames must be used.   | <a href="https://bazel.build/docs/skylark/lib/dict.html">Dictionary: String -> String</a> | optional | {} |
| src |  Directory (TreeArtifact) to process.   | <a href="https://bazel.build/docs/build-ref.html#labels">Label</a> | required |  |
| strip_prefix |  Prefix to remove from all paths in the output directory.<br><br>            Must apply to all paths in the directory, even those rename'd.   | String | optional | "" |


<a name="#pkg_filegroup"></a>

## pkg_filegroup

<pre>
pkg_filegroup(<a href="#pkg_filegroup-name">name</a>, <a href="#pkg_filegroup-prefix">prefix</a>, <a href="#pkg_filegroup-srcs">srcs</a>)
</pre>

Package contents grouping rule.

    This rule represents a collection of packaging specifications (e.g. those
    created by `pkg_files`, `pkg_mklink`, etc.) that have something in common,
    such as a prefix or a human-readable category.


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| prefix |  A prefix to prepend to provided paths, applied like so:<br><br>            - For files and directories, this is simply prepended to the destination             - For symbolic links, this is prepended to the "destination" part.   | String | optional | "" |
| srcs |  A list of packaging specifications to be grouped together.   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | required |  |


<a name="#pkg_files"></a>

## pkg_files

<pre>
pkg_files(<a href="#pkg_files-name">name</a>, <a href="#pkg_files-attributes">attributes</a>, <a href="#pkg_files-excludes">excludes</a>, <a href="#pkg_files-prefix">prefix</a>, <a href="#pkg_files-renames">renames</a>, <a href="#pkg_files-srcs">srcs</a>, <a href="#pkg_files-strip_prefix">strip_prefix</a>)
</pre>

General-purpose package target-to-destination mapping rule.

    This rule provides a specification for the locations and attributes of
    targets when they are packaged. No outputs are created other than Providers
    that are intended to be consumed by other packaging rules, such as
    `pkg_rpm`.

    Labels associated with these rules are not passed directly to packaging
    rules, instead, they should be passed to an associated `pkg_filegroup` rule,
    which in turn should be passed to packaging rules.

    Consumers of `pkg_files`s will, where possible, create the necessary
    directory structure for your files so you do not have to unless you have
    special requirements.  Consult `pkg_mkdirs` for more details.


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| attributes |  Attributes to set on packaged files.<br><br>            Always use <code>pkg_attributes()</code> to set this rule attribute.<br><br>            If not otherwise overridden, the file's mode will be set to UNIX             "0644", or the target platform's equivalent.<br><br>            Consult the "Mapping Attributes" documentation in the rules_pkg             reference for more details.   | String | optional | "{}" |
| excludes |  List of files or labels to exclude from the inputs to this rule.<br><br>            Mostly useful for removing files from generated outputs or             preexisting <code>filegroup</code>s.   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | optional | [] |
| prefix |  Installation prefix.<br><br>            This may be an arbitrary string, but it should be understandable by             the packaging system you are using to have the desired outcome.  For             example, RPM macros like <code>%{_libdir}</code> may work correctly in paths             for RPM packages, not, say, Debian packages.<br><br>            If any part of the directory structure of the computed destination             of a file provided to <code>pkg_filegroup</code> or any similar rule does not             already exist within a package, the package builder will create it             for you with a reasonable set of default permissions (typically             <code>0755 root.root</code>).<br><br>            It is possible to establish directory structures with arbitrary             permissions using <code>pkg_mkdirs</code>.   | String | optional | "" |
| renames |  Destination override map.<br><br>            This attribute allows the user to override destinations of files in             <code>pkg_file</code>s relative to the <code>prefix</code> attribute.  Keys to the             dict are source files/labels, values are destinations relative to             the <code>prefix</code>, ignoring whatever value was provided for             <code>strip_prefix</code>.<br><br>            If the key refers to a TreeArtifact (directory output), you may             specify the constant <code>REMOVE_BASE_DIRECTORY</code> as the value, which             will result in all containing files and directories being installed             relative to the otherwise specified install prefix (via the <code>prefix</code>             and <code>strip_prefix</code> attributes), not the directory name.<br><br>            The following keys are rejected:<br><br>            - Any label that expands to more than one file (mappings must be               one-to-one).<br><br>            - Any label or file that was either not provided or explicitly               <code>exclude</code>d.<br><br>            The following values result in undefined behavior:<br><br>            - "" (the empty string)<br><br>            - "."<br><br>            - Anything containing ".."   | <a href="https://bazel.build/docs/skylark/lib/dict.html">Dictionary: Label -> String</a> | optional | {} |
| srcs |  Files/Labels to include in the outputs of these rules   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | required |  |
| strip_prefix |  What prefix of a file's path to discard prior to installation.<br><br>            This specifies what prefix of an incoming file's path should not be             included in the output package at after being appended to the             install prefix (the <code>prefix</code> attribute).  Note that this is only             applied to full directory names, see <code>strip_prefix</code> for more             details.<br><br>            Use the <code>strip_prefix</code> struct to define this attribute.  If this             attribute is not specified, all directories will be stripped from             all files prior to being included in packages             (<code>strip_prefix.files_only()</code>).<br><br>            If prefix stripping fails on any file provided in <code>srcs</code>, the build             will fail.<br><br>            Note that this only functions on paths that are known at analysis             time.  Specifically, this will not consider directories within             TreeArtifacts (directory outputs), or the directories themselves.             See also #269.   | String | optional | "." |


<a name="#pkg_mkdirs"></a>

## pkg_mkdirs

<pre>
pkg_mkdirs(<a href="#pkg_mkdirs-name">name</a>, <a href="#pkg_mkdirs-attributes">attributes</a>, <a href="#pkg_mkdirs-dirs">dirs</a>)
</pre>

Defines creation and ownership of directories in packages

    Use this if:

    1) You need to create an empty directory in your package.

    2) Your package needs to explicitly own a directory, even if it already owns
       files in those directories.

    3) You need nonstandard permissions (typically, not "0755") on a directory
       in your package.

    For some package management systems (e.g. RPM), directory ownership (2) may
    imply additional semantics.  Consult your package manager's and target
    distribution's documentation for more details.


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| attributes |  Attributes to set on packaged directories.<br><br>            Always use <code>pkg_attributes()</code> to set this rule attribute.<br><br>            If not otherwise overridden, the directory's mode will be set to             UNIX "0755", or the target platform's equivalent.<br><br>            Consult the "Mapping Attributes" documentation in the rules_pkg             reference for more details.   | String | optional | "{}" |
| dirs |  Directory names to make within the package<br><br>            If any part of the requested directory structure does not already             exist within a package, the package builder will create it for you             with a reasonable set of default permissions (typically <code>0755             root.root</code>).   | List of strings | required |  |


<a name="#pkg_mklink"></a>

## pkg_mklink

<pre>
pkg_mklink(<a href="#pkg_mklink-name">name</a>, <a href="#pkg_mklink-attributes">attributes</a>, <a href="#pkg_mklink-dest">dest</a>, <a href="#pkg_mklink-src">src</a>)
</pre>

Define a symlink  within packages

    This rule results in the creation of a single link within a package.

    Symbolic links specified by this rule may point at files/directories outside of the
    package, or otherwise left dangling.


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| attributes |  Attributes to set on packaged symbolic links.<br><br>            Always use <code>pkg_attributes()</code> to set this rule attribute.<br><br>            Symlink permissions may have different meanings depending on your             host operating system; consult its documentation for more details.<br><br>            If not otherwise overridden, the link's mode will be set to UNIX             "0777", or the target platform's equivalent.<br><br>            Consult the "Mapping Attributes" documentation in the rules_pkg             reference for more details.   | String | optional | "{}" |
| dest |  Link "target", a path within the package.<br><br>            This is the actual created symbolic link.<br><br>            If the directory structure provided by this attribute is not             otherwise created when exist within the package when it is built, it             will be created implicitly, much like with <code>pkg_files</code>.<br><br>            This path may be prefixed or rooted by grouping or packaging rules.   | String | required |  |
| src |  Link "source", a path on the filesystem.<br><br>            This is what the link "points" to, and may point to an arbitrary             filesystem path, even relative paths.   | String | required |  |


<a name="#pkg_attributes"></a>

## pkg_attributes

<pre>
pkg_attributes(<a href="#pkg_attributes-mode">mode</a>, <a href="#pkg_attributes-user">user</a>, <a href="#pkg_attributes-group">group</a>, <a href="#pkg_attributes-kwargs">kwargs</a>)
</pre>

Format attributes for use in package mapping rules.

If "mode" is not provided, it will default to the mapping rule's default
mode.  These vary per mapping rule; consult the respective documentation for
more details.

Not providing any of "user", or "group" will result in the package builder
choosing one for you.  The chosen value should not be relied upon.

Well-known attributes outside of the above are documented in the rules_pkg
reference.

This is the only supported means of passing in attributes to package mapping
rules (e.g. `pkg_files`).


**PARAMETERS**


| Name  | Description | Default Value |
| :-------------: | :-------------: | :-------------: |
| mode |  string: UNIXy octal permissions, as a string.   |  <code>None</code> |
| user |  string: Filesystem owning user.   |  <code>None</code> |
| group |  string: Filesystem owning group.   |  <code>None</code> |
| kwargs |  any other desired attributes.   |  none |


<a name="#strip_prefix.files_only"></a>

## strip_prefix.files_only

<pre>
strip_prefix.files_only()
</pre>



**PARAMETERS**



<a name="#strip_prefix.from_pkg"></a>

## strip_prefix.from_pkg

<pre>
strip_prefix.from_pkg(<a href="#strip_prefix.from_pkg-path">path</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :-------------: | :-------------: | :-------------: |
| path |  <p align="center"> - </p>   |  <code>""</code> |


<a name="#strip_prefix.from_root"></a>

## strip_prefix.from_root

<pre>
strip_prefix.from_root(<a href="#strip_prefix.from_root-path">path</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :-------------: | :-------------: | :-------------: |
| path |  <p align="center"> - </p>   |  <code>""</code> |


