// Copyright (c) 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_QUIC_TOOLS_QUIC_TRANSPORT_SIMPLE_SERVER_SESSION_H_
#define QUICHE_QUIC_TOOLS_QUIC_TRANSPORT_SIMPLE_SERVER_SESSION_H_

#include <memory>
#include <vector>

#include "url/origin.h"
#include "quiche/quic/core/quic_types.h"
#include "quiche/quic/core/quic_versions.h"
#include "quiche/quic/platform/api/quic_containers.h"
#include "quiche/quic/platform/api/quic_flags.h"
#include "quiche/quic/quic_transport/quic_transport_server_session.h"
#include "quiche/quic/quic_transport/quic_transport_stream.h"

namespace quic {

// QuicTransport simple server is a non-production server that can be used for
// testing QuicTransport.  It has two modes that can be changed using the
// command line flags, "echo" and "discard".
class QuicTransportSimpleServerSession
    : public QuicTransportServerSession,
      QuicTransportServerSession::ServerVisitor {
 public:
  enum Mode {
    // In DISCARD mode, any data on incoming streams is discarded and no
    // outgoing streams are initiated.
    DISCARD,
    // In ECHO mode, any data sent on a bidirectional stream is echoed back.
    // Any data sent on a unidirectional stream is buffered, and echoed back on
    // a server-initiated unidirectional stream that is sent as soon as a FIN is
    // received on the incoming stream.
    ECHO,
    // In OUTGOING_BIDIRECTIONAL mode, a server-originated bidirectional stream
    // is created on receipt of a unidirectional stream. The contents of the
    // unidirectional stream are disregarded. The bidirectional stream initially
    // sends "hello", then any received data is echoed back.
    // TODO(ricea): Maybe this should be replaced by a more general mechanism
    // where commands on the unidirectional stream trigger different behaviour?
    OUTGOING_BIDIRECTIONAL,
  };

  QuicTransportSimpleServerSession(
      QuicConnection* connection,
      bool owns_connection,
      Visitor* owner,
      const QuicConfig& config,
      const ParsedQuicVersionVector& supported_versions,
      const QuicCryptoServerConfig* crypto_config,
      QuicCompressedCertsCache* compressed_certs_cache,
      std::vector<url::Origin> accepted_origins);
  ~QuicTransportSimpleServerSession();

  void OnIncomingDataStream(QuicTransportStream* stream) override;
  void OnCanCreateNewOutgoingStream(bool unidirectional) override;
  bool CheckOrigin(url::Origin origin) override;
  bool ProcessPath(const GURL& url) override;
  void OnMessageReceived(absl::string_view message) override;

  void EchoStreamBack(const std::string& data) {
    streams_to_echo_back_.push_back(data);
    MaybeEchoStreamsBack();
  }

 private:
  void MaybeEchoStreamsBack();
  void MaybeCreateOutgoingBidirectionalStream();

  const bool owns_connection_;
  size_t pending_outgoing_bidirectional_streams_ = 0u;
  Mode mode_;
  std::vector<url::Origin> accepted_origins_;
  quiche::QuicheCircularDeque<std::string> streams_to_echo_back_;
};

}  // namespace quic

#endif  // QUICHE_QUIC_TOOLS_QUIC_TRANSPORT_SIMPLE_SERVER_SESSION_H_
