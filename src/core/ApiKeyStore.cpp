// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#include "ApiKeyStore.h"

#include <QSettings>
#include <QFile>
#include <QFileDevice>

#ifdef ASTROFIND_HAS_KEYCHAIN
#  include <qt6keychain/keychain.h>
#  include <QEventLoop>
#endif

namespace core {

namespace {
#ifndef ASTROFIND_HAS_KEYCHAIN
// AUD-SEC-5: the plain-text QSettings fallback (no qt6keychain) writes the
// API key to a config file that inherits the platform-default permissions
// (typically 0644 — world-readable). The user IS warned about this in
// SettingsDialog, but the file itself should still be locked down to the
// owner. sync() first so the write is flushed to disk before we chmod it
// (setPermissions on a not-yet-created file is a silent no-op).
void restrictSettingsFilePermissions(QSettings& s)
{
    s.sync();
    QFile::setPermissions(s.fileName(),
        QFileDevice::ReadOwner | QFileDevice::WriteOwner);
}
#endif
} // anonymous namespace

// ─── isSecure ─────────────────────────────────────────────────────────────────

bool ApiKeyStore::isSecure() noexcept
{
#ifdef ASTROFIND_HAS_KEYCHAIN
    return true;
#else
    return false;
#endif
}

// ─── read ─────────────────────────────────────────────────────────────────────

QString ApiKeyStore::read()
{
#ifdef ASTROFIND_HAS_KEYCHAIN
    // ── Try the keychain ────────────────────────────────────────────────
    const QLatin1String svcName(kService);
    QKeychain::ReadPasswordJob job(svcName);
    job.setAutoDelete(false);
    job.setKey(QLatin1String(kAccount));

    QEventLoop loop;
    QObject::connect(&job, &QKeychain::Job::finished, &loop, &QEventLoop::quit);
    job.start();
    loop.exec();

    if (job.error() == QKeychain::NoError && !job.textData().isEmpty())
        return job.textData();

    // ── One-time migration from QSettings ───────────────────────────────
    // If the user had the key stored in plain-text QSettings before this
    // feature was introduced, silently move it to the keychain.
    QSettings s;
    QString legacy = s.value(QLatin1String(kLegacySettingsKey)).toString().trimmed();
    if (!legacy.isEmpty()) {
        write(legacy);                             // store in keychain
        s.remove(QLatin1String(kLegacySettingsKey)); // erase from QSettings
        return legacy;
    }

    return {};
#else
    // No keychain — read from QSettings plain-text
    QSettings s;
    return s.value(QLatin1String(kLegacySettingsKey)).toString().trimmed();
#endif
}

// ─── write ────────────────────────────────────────────────────────────────────

void ApiKeyStore::write(const QString& key)
{
#ifdef ASTROFIND_HAS_KEYCHAIN
    if (key.isEmpty()) {
        // Delete the entry
        auto* del = new QKeychain::DeletePasswordJob(QLatin1String(kService));
        del->setAutoDelete(true);
        del->setKey(QLatin1String(kAccount));
        del->start();
    } else {
        auto* job = new QKeychain::WritePasswordJob(QLatin1String(kService));
        job->setAutoDelete(true);
        job->setKey(QLatin1String(kAccount));
        job->setTextData(key);
        job->start();
    }
    // Also ensure the legacy QSettings key is gone
    QSettings s;
    s.remove(QLatin1String(kLegacySettingsKey));
#else
    // No keychain — fall back to QSettings plain-text
    QSettings s;
    if (key.isEmpty())
        s.remove(QLatin1String(kLegacySettingsKey));
    else
        s.setValue(QLatin1String(kLegacySettingsKey), key);
    restrictSettingsFilePermissions(s);
#endif
}

} // namespace core
