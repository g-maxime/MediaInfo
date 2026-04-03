/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "portal_utils.h"

#include <QRegularExpression>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QVariantMap>
#include <QFileInfo>
#include <QDir>

namespace {

static bool getHostPathForDocId(const QString& docId, QString& hostPath)
{
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.isConnected()) {
        return false;
    }

    QDBusInterface documentsPortal(
        "org.freedesktop.portal.Documents",
        "/org/freedesktop/portal/documents",
        "org.freedesktop.portal.Documents",
        sessionBus);

    if (!documentsPortal.isValid()) {
        return false;
    }

    const QStringList docIds = QStringList() << docId;
    const QDBusMessage reply = documentsPortal.call("GetHostPaths", docIds);
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().isEmpty()) {
        return false;
    }

    const QVariantMap paths = reply.arguments().at(0).toMap();
    const QVariant hostPathValue = paths.value(docId);
    if (!hostPathValue.isValid()) {
        return false;
    }

    hostPath = QString::fromUtf8(hostPathValue.toByteArray());
    return !hostPath.isEmpty();
}

}

bool PortalUtils::isPortalPath(const QString& path)
{
    // Check if path matches a portal pattern: /run/user/*/doc*/*/ or /run/doc/<random>/<filename>
    QRegularExpression portalPattern("^/run/(doc|user/[0-9]+/doc)([a-zA-Z0-9/_-]*)/[a-zA-Z0-9]+/");

    return portalPattern.match(path).hasMatch();
}

QString PortalUtils::resolvePortalPath(const QString& portalPath)
{
    if (!isPortalPath(portalPath)) {
        return portalPath;
    }

    const int docSegmentPos = portalPath.indexOf("/doc/");
    const QString relative = portalPath.mid(docSegmentPos + 5);
    const int slashPos = relative.indexOf('/');
    if (slashPos <= 0) {
        return portalPath;
    }

    const QString docId = relative.left(slashPos);
    const QString docVisiblePath = relative.mid(slashPos + 1);

    QString hostBasePath;
    if (!getHostPathForDocId(docId, hostBasePath)) {
        return portalPath;
    }

    if (docVisiblePath.isEmpty()) {
        return hostBasePath;
    }

    // For files exported as /doc/<id>/<basename>, GetHostPaths already returns
    // the full file path. For exported directories, preserve the subpath.
    const QStringList visibleParts = docVisiblePath.split('/', Qt::SkipEmptyParts);
    if (visibleParts.size() <= 1) {
        return hostBasePath;
    }

    QString relativeInsideExport;
    const QString hostBaseName = QFileInfo(hostBasePath).fileName();
    if (!hostBaseName.isEmpty() && visibleParts.first() == hostBaseName) {
        relativeInsideExport = visibleParts.mid(1).join('/');
    } else {
        relativeInsideExport = visibleParts.join('/');
    }

    if (relativeInsideExport.isEmpty()) {
        return hostBasePath;
    }

    QFileInfo hostInfo(hostBasePath);
    if (hostInfo.isDir()) {
        return QDir(hostBasePath).filePath(relativeInsideExport);
    }
    return hostInfo.dir().filePath(relativeInsideExport);
}
