/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

package net.mediaarea.mediainfo

import android.app.Application
import androidx.appcompat.app.AppCompatDelegate
import androidx.multidex.MultiDexApplication


import com.yariksoffice.lingver.Lingver
import com.yariksoffice.lingver.store.InMemoryLocaleStore


class MediaInfoApplication : MultiDexApplication() {
    override fun onCreate() {
        super.onCreate()

        Lingver.init(this, InMemoryLocaleStore())
        AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO)
    }
}