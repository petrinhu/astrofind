// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#pragma once

class QUrl;
class QString;

namespace core {

/// AUD-SEC-3 / AUD-SEC-10: transfer timeout for every outbound request. Qt
/// restarts it on any upload/download progress, so it bounds a stalled server
/// without cutting off a slow-but-alive transfer (e.g. the ~200 MB MPCORB.DAT).
inline constexpr int kHttpTransferTimeoutMs = 30000;

/// AUD-SEC-4 / AUD-SEC-13: true when a user-configurable service endpoint
/// (astrometry.net base URL, VizieR TAP mirror, MPC submission URL) may carry
/// secrets or reports safely — https:// always, or http:// restricted to
/// loopback (localhost / 127.0.0.1 / ::1: a local instance, relay or proxy,
/// where no network eavesdropper can intercept the traffic). Any other scheme
/// (including none) is rejected.
///
/// Single source of truth shared by the client setters (AstrometryClient::
/// setBaseUrl, CatalogClient::setVizierUrl, MpcSubmit::setEndpoint) and the
/// save-time check in SettingsDialog, so the dialog never stores a URL the
/// setter would refuse.
bool isSafeServiceUrl(const QUrl& url);
bool isSafeServiceUrl(const QString& url);

} // namespace core
