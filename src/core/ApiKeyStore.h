#pragma once

#include <QString>

namespace core {

/// Secure storage for the astrometry.net API key.
///
/// When compiled with Qt6Keychain (ASTROFIND_HAS_KEYCHAIN defined), keys are
/// stored in the OS credential manager (KWallet / libsecret on Linux,
/// Keychain on macOS, Windows Credential Store on Windows).
///
/// Without Qt6Keychain the class falls back to QSettings plain-text storage
/// and `isSecure()` returns false — the UI surfaces a warning to the user.
///
/// One-time migration: if a key is found under the legacy QSettings key
/// "astrometry/apiKey" it is automatically moved to the keychain on the
/// first `read()` call and removed from QSettings.
class ApiKeyStore {
public:
    /// True when compiled with Qt6Keychain AND a keychain daemon is reachable.
    static bool isSecure() noexcept;

    /// Read the stored key.  Returns an empty string if none is set.
    /// Safe to call from the main thread (blocks briefly via QEventLoop when
    /// the keychain backend is active).
    static QString read();

    /// Persist @p key.  Pass an empty string to clear the stored key.
    static void write(const QString& key);

private:
    static constexpr const char* kService = "AstroFind";
    static constexpr const char* kAccount = "astrometry_api_key";
    static constexpr const char* kLegacySettingsKey = "astrometry/apiKey";
};

} // namespace core
