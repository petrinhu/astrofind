#include "ApiKeyStore.h"

#include <QSettings>

#ifdef ASTROFIND_HAS_KEYCHAIN
#  include <qt6keychain/keychain.h>
#  include <QEventLoop>
#endif

namespace core {

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
    const QString legacy = s.value(QLatin1String(kLegacySettingsKey)).toString().trimmed();
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
#endif
}

} // namespace core
