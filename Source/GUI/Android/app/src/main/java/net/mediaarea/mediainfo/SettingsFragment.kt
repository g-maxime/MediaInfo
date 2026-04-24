/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

package net.mediaarea.mediainfo

import android.content.Intent
import android.content.res.Resources
import android.os.Bundle
import android.os.Build
import android.provider.Settings
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatDelegate
import androidx.core.net.toUri
import androidx.preference.Preference
import androidx.preference.PreferenceFragmentCompat
import androidx.preference.SwitchPreferenceCompat
import androidx.preference.DropDownPreference
import com.yariksoffice.lingver.Lingver
import java.util.*
import android.content.pm.PackageManager

class SettingsFragment : PreferenceFragmentCompat() {
    private var localeDropdown: DropDownPreference? = null
    private var uimodeDropdown: DropDownPreference? = null
    private var systemLanguageSwitch: SwitchPreferenceCompat? = null
    private var permissionsPreference: Preference? = null

    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        setPreferencesFromResource(R.xml.fragment_settings, rootKey)

        //val subscribeButton: Preference? = findPreference(getString(R.string.preferences_subscribe_key))
        localeDropdown = findPreference(getString(R.string.preferences_locale_key))
        uimodeDropdown = findPreference(getString(R.string.preferences_uimode_key))
        systemLanguageSwitch = findPreference(getString(R.string.preferences_report_translate_key))
        permissionsPreference = findPreference(getString(R.string.preferences_permissions_key))

        /*subscribeButton?.setOnPreferenceClickListener  {
            val intent = Intent(activity, SubscribeActivity::class.java)
            startActivity(intent)
            true
        }*/

        if (activity != null) {
            val activityListener = activity as SettingsActivityListener
            val subscriptionManager = activityListener.getSubscriptionManager()

            /*subscriptionManager.ready.observe(this, Observer {
                if (it==true) {
                    subscribeButton?.isVisible = true
                }
            })*/

            subscriptionManager.subscribed.value?.let {
                updateSubscriptionState(it)
            }

            subscriptionManager.subscribed.observe(this) {
                updateSubscriptionState(it)
            }
        }

        localeDropdown?.setOnPreferenceChangeListener { _, newValue ->
            if (newValue is String) {
                val locale: Locale =
                    if (newValue == "system") {
                        if (Build.VERSION.SDK_INT >= 24) {
                            Resources.getSystem().configuration.locales.get(0)
                        } else {
                            @Suppress("DEPRECATION")
                            Resources.getSystem().configuration.locale
                        }
                    } else {
                        val language = newValue.split("-r")
                        if (language.size > 1) {
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.BAKLAVA) {
                                Locale.of(language[0], language[1])
                            } else {
                                @Suppress("DEPRECATION")
                                Locale(language[0], language[1])
                            }
                        } else {
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.BAKLAVA) {
                                Locale.of(language[0])
                            } else {
                                @Suppress("DEPRECATION")
                                Locale(language[0])
                            }
                        }
                    }

                Locale.setDefault(locale)

                if (Lingver.getInstance().getLocale() != locale) {
                    val settingsActivity = activity
                    if (settingsActivity != null) {
                        Lingver.getInstance().setLocale(settingsActivity, locale)
                        settingsActivity.recreate()
                    }
                }
            }
            true
        }

        uimodeDropdown?.setOnPreferenceChangeListener { _, newValue ->
            if (newValue is String) {
                when (newValue) {
                    "off" -> {
                        if (AppCompatDelegate.getDefaultNightMode() != AppCompatDelegate.MODE_NIGHT_NO) {
                            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_NO)
                        }
                    }

                    "on" -> {
                        if (AppCompatDelegate.getDefaultNightMode() != AppCompatDelegate.MODE_NIGHT_YES) {
                            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_YES)
                        }
                    }

                    "system" -> {
                        if (AppCompatDelegate.getDefaultNightMode() != AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM) {
                            AppCompatDelegate.setDefaultNightMode(AppCompatDelegate.MODE_NIGHT_FOLLOW_SYSTEM)
                        }
                    }
                }
            }
            true
        }

        permissionsPreference?.setOnPreferenceClickListener {
            showGeolocationHelpDialog()
            true
        }
        updateGeolocationPreferenceVisibility()
    }

    override fun onResume() {
        super.onResume()
        updateGeolocationPreferenceVisibility()
    }

    fun updateSubscriptionState(value: Boolean) {
        if (value) {
            //subscribeButton?.isVisible = false
            uimodeDropdown?.isEnabled = true
            localeDropdown?.isEnabled = true
            systemLanguageSwitch?.isEnabled = true
        }
    }

    private fun openPermissionsSettings() {
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
        intent.addCategory(Intent.CATEGORY_DEFAULT)
        intent.data = "package:${activity?.packageName}".toUri()
        startActivity(intent)
    }

    private fun showGeolocationHelpDialog() {
        val context = context ?: return
        AlertDialog.Builder(context)
            .setTitle(R.string.permissions_geolocation_title)
            .setMessage(R.string.permissions_geolocation_help)
            .setPositiveButton(android.R.string.ok) { _, _ ->
                openPermissionsSettings()
            }
            .setNegativeButton(android.R.string.cancel, null)
            .show()
    }

    private fun updateGeolocationPreferenceVisibility() {
        val hostActivity = activity
        permissionsPreference?.isVisible =
            Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q &&
                    hostActivity != null &&
                    hostActivity.checkSelfPermission(android.Manifest.permission.ACCESS_MEDIA_LOCATION) != PackageManager.PERMISSION_GRANTED
    }
}
