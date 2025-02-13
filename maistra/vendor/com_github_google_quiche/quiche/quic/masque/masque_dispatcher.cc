// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "quiche/quic/masque/masque_dispatcher.h"
#include "quiche/quic/masque/masque_server_session.h"

namespace quic {

MasqueDispatcher::MasqueDispatcher(
    MasqueMode masque_mode,
    const QuicConfig* config,
    const QuicCryptoServerConfig* crypto_config,
    QuicVersionManager* version_manager,
    QuicEpollServer* epoll_server,
    std::unique_ptr<QuicConnectionHelperInterface> helper,
    std::unique_ptr<QuicCryptoServerStreamBase::Helper> session_helper,
    std::unique_ptr<QuicAlarmFactory> alarm_factory,
    MasqueServerBackend* masque_server_backend,
    uint8_t expected_server_connection_id_length)
    : QuicSimpleDispatcher(config,
                           crypto_config,
                           version_manager,
                           std::move(helper),
                           std::move(session_helper),
                           std::move(alarm_factory),
                           masque_server_backend,
                           expected_server_connection_id_length),
      masque_mode_(masque_mode),
      epoll_server_(epoll_server),
      masque_server_backend_(masque_server_backend) {}

std::unique_ptr<QuicSession> MasqueDispatcher::CreateQuicSession(
    QuicConnectionId connection_id, const QuicSocketAddress& self_address,
    const QuicSocketAddress& peer_address, absl::string_view /*alpn*/,
    const ParsedQuicVersion& version,
    const ParsedClientHello& /*parsed_chlo*/) {
  // The MasqueServerSession takes ownership of |connection| below.
  QuicConnection* connection =
      new QuicConnection(connection_id, self_address, peer_address, helper(),
                         alarm_factory(), writer(),
                         /*owns_writer=*/false, Perspective::IS_SERVER,
                         ParsedQuicVersionVector{version});

  auto session = std::make_unique<MasqueServerSession>(
      masque_mode_, config(), GetSupportedVersions(), connection, this, this,
      epoll_server_, session_helper(), crypto_config(),
      compressed_certs_cache(), masque_server_backend_);
  session->Initialize();
  return session;
}

bool MasqueDispatcher::OnFailedToDispatchPacket(
    const ReceivedPacketInfo& packet_info) {
  auto connection_id_registration = client_connection_id_registrations_.find(
      packet_info.destination_connection_id);
  if (connection_id_registration == client_connection_id_registrations_.end()) {
    QUIC_DLOG(INFO) << "MasqueDispatcher failed to dispatch " << packet_info;
    return false;
  }
  MasqueServerSession* masque_server_session =
      connection_id_registration->second;
  masque_server_session->HandlePacketFromServer(packet_info);
  return true;
}

void MasqueDispatcher::RegisterClientConnectionId(
    QuicConnectionId client_connection_id,
    MasqueServerSession* masque_server_session) {
  QUIC_DLOG(INFO) << "Registering encapsulated " << client_connection_id
                  << " to MASQUE session "
                  << masque_server_session->connection_id();

  // Make sure we don't try to overwrite an existing registration with a
  // different session.
  QUIC_BUG_IF(quic_bug_12013_1,
              client_connection_id_registrations_.find(client_connection_id) !=
                      client_connection_id_registrations_.end() &&
                  client_connection_id_registrations_[client_connection_id] !=
                      masque_server_session)
      << "Overwriting existing registration for " << client_connection_id;
  client_connection_id_registrations_[client_connection_id] =
      masque_server_session;
}

void MasqueDispatcher::UnregisterClientConnectionId(
    QuicConnectionId client_connection_id) {
  QUIC_DLOG(INFO) << "Unregistering " << client_connection_id;
  client_connection_id_registrations_.erase(client_connection_id);
}

}  // namespace quic
