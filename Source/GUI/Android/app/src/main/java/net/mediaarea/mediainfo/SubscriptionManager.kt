/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

package net.mediaarea.mediainfo

import java.util.concurrent.atomic.AtomicInteger

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import kotlinx.coroutines.delay
import kotlin.math.pow
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleObserver
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.OnLifecycleEvent
import android.util.Log
import android.app.Activity
import android.app.Application
import com.android.billingclient.api.*

class SubscriptionManager private constructor(private val application: Application) : PurchasesUpdatedListener, BillingClientStateListener, LifecycleObserver {
    val ready = MutableLiveData<Boolean>()
    val subscribed = MutableLiveData<Boolean>()
    val details = MutableLiveData<SkuDetails>()

    private lateinit var billingClient: BillingClient

    @OnLifecycleEvent(Lifecycle.Event.ON_CREATE)
    fun create() {
        billingClient = BillingClient.newBuilder(application.applicationContext)
                .enablePendingPurchases()
                .setListener(this)
                .build()

        updateState(false)
        updateSubscribedState(false)

        if (billingClient.isReady) {
            updateState(isSubscriptionSupported())
        }
        else {
            billingClient.startConnection(this)
        }
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_DESTROY)
    fun destroy() {
        if (billingClient.isReady) {
            billingClient.endConnection()
        }
    }

    private fun updateState(newState: Boolean) {
        if (ready.value!=newState) {
            ready.postValue(newState)
        }
    }

    private fun updateSubscribedState(newState: Boolean) {
        if (subscribed.value!=newState) {
            subscribed.postValue(newState)
        }
    }

    private fun isSubscriptionSupported(): Boolean {
        val response = billingClient.isFeatureSupported(BillingClient.FeatureType.SUBSCRIPTIONS)
        return response.responseCode==BillingClient.BillingResponseCode.OK
    }

    fun launchBillingFlow(activity: Activity, params: BillingFlowParams): Int {
        if (!billingClient.isReady) {
            Log.e(LOG_TAG, "BillingClient is not ready to start billing flow")
        }
        val response = billingClient.launchBillingFlow(activity, params)
        Log.i(LOG_TAG, "Launch Billing Flow Response Code: ${response.responseCode}")
        return response.responseCode
    }

    override fun onPurchasesUpdated(p0: BillingResult?, p1: MutableList<Purchase>?) {
        if (p0?.responseCode==BillingClient.BillingResponseCode.OK) {
            handlePurchases(p1)
        } else if (p0?.responseCode==BillingClient.BillingResponseCode.DEVELOPER_ERROR) {
            Log.e(LOG_TAG, "Your app's configuration is incorrect. Review in the Google Play Console. Possible causes of this error include: APK is not signed with release key; SKU productId mismatch.")
        }
    }

    override fun onBillingSetupFinished(p0: BillingResult?) {
        if (p0?.responseCode==BillingClient.BillingResponseCode.OK) {
            RetryPolicies.resetConnectionRetryPolicyCounter()

            fun updatePurchasesTask() {
                val result = billingClient.queryPurchases(BillingClient.SkuType.SUBS)
                handlePurchases(result.purchasesList)
            }
            RetryPolicies.taskExecutionRetryPolicy(billingClient, this) { updatePurchasesTask() }

            if (isSubscriptionSupported()) {
                val params = SkuDetailsParams
                        .newBuilder()
                        .setSkusList(listOf(application.getString(R.string.subscription_sku)))
                        .setType(BillingClient.SkuType.SUBS)
                        .build()

                billingClient.querySkuDetailsAsync(params) { results: BillingResult, list: List<SkuDetails> ->
                    if (results.responseCode == BillingClient.BillingResponseCode.OK) {
                        list.forEach {
                            if (it.sku == application.getString(R.string.subscription_sku)) {
                                details.postValue(it)
                                updateState(true)
                            }
                        }
                    }
                }
            }

            // Trigger cache update
            billingClient.queryPurchaseHistoryAsync(BillingClient.SkuType.SUBS) { _, _ ->
                val result = billingClient.queryPurchases(BillingClient.SkuType.SUBS)
                handlePurchases(result.purchasesList)
            }
        } else  {
            updateState(false)
            Log.d(LOG_TAG, "onBillingSetupFinished with failure response code: ${p0?.responseCode}")
        }
    }

    // Disconnected from the Play BillingClient
    override fun onBillingServiceDisconnected() {
        updateState(false)
        RetryPolicies.connectionRetryPolicy { billingClient.startConnection(this) }
    }

    private fun handlePurchases(purchasesList: List<Purchase>?) {
        if (purchasesList==null)
            return

        purchasesList.forEach {
            if (it.sku == application.getString(R.string.subscription_sku)) {
                updateSubscribedState(true)
            }
        }
    }

    // Retries handler
    private object RetryPolicies {
        private const val maxRetry = 5
        private const val taskDelay = 2000L
        private const val baseDelayMillis = 500
        private var retryCounter = AtomicInteger(1)

        fun resetConnectionRetryPolicyCounter() {
            retryCounter.set(1)
        }

        fun connectionRetryPolicy(block: () -> Unit) {
            val scope = CoroutineScope(Job() + Dispatchers.Main)
            scope.launch {
                val counter = retryCounter.getAndIncrement()
                if (counter < maxRetry) {
                    val waitTime: Long = (2f.pow(counter) * baseDelayMillis).toLong()
                    delay(waitTime)
                    block()
                }
            }
        }

        fun taskExecutionRetryPolicy(billingClient: BillingClient, listener: SubscriptionManager, task: () -> Unit) {
            val scope = CoroutineScope(Job() + Dispatchers.Main)
            scope.launch {
                if (!billingClient.isReady) {
                    billingClient.startConnection(listener)
                    delay(taskDelay)
                }
                task()
            }
        }
    }

    companion object {
        private const val LOG_TAG = "MediaInfo/Billing"

        @Volatile
        private var INSTANCE: SubscriptionManager? = null
        fun getInstance(app: Application): SubscriptionManager = INSTANCE?: synchronized(this) {
            INSTANCE ?: SubscriptionManager(app).also { INSTANCE = it }
        }
    }
}