/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef PORTAL_UTILS_H
#define PORTAL_UTILS_H

#include <QString>

class PortalUtils
{
public:
    static bool isPortalPath(const QString& path);
    static QString resolvePortalPath(const QString& portalPath);
};

#endif // PORTAL_UTILS_H
