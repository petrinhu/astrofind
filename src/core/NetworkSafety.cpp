// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#include "NetworkSafety.h"

#include <QString>
#include <QUrl>

namespace core {

bool isSafeServiceUrl(const QUrl& url)
{
    const QString scheme = url.scheme().toLower();
    if (scheme == QLatin1String("https")) return true;
    if (scheme != QLatin1String("http")) return false;
    const QString host = url.host().toLower();
    return host == QLatin1String("localhost")
        || host == QLatin1String("127.0.0.1")
        || host == QLatin1String("::1");
}

bool isSafeServiceUrl(const QString& url)
{
    return isSafeServiceUrl(QUrl(url.trimmed()));
}

} // namespace core
