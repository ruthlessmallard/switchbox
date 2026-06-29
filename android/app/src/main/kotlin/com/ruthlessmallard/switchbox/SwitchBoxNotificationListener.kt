package com.ruthlessmallard.switchbox

import android.service.notification.NotificationListenerService
import android.service.notification.StatusBarNotification
import android.util.Log

/**
 * NotificationListenerService required for MediaSessionManager.getActiveSessions().
 * We don't actually care about notifications — we just need the binding
 * so Android lets us enumerate and control other apps' media sessions.
 */
class SwitchBoxNotificationListener : NotificationListenerService() {

    companion object {
        private const val TAG = "SwitchBoxNotification"
    }

    override fun onListenerConnected() {
        super.onListenerConnected()
        Log.d(TAG, "Notification listener connected — media session control available")
    }

    override fun onListenerDisconnected() {
        super.onListenerDisconnected()
        Log.d(TAG, "Notification listener disconnected")
    }

    override fun onNotificationPosted(sbn: StatusBarNotification?) {
        // We don't process notifications — this is just a binding requirement
    }

    override fun onNotificationRemoved(sbn: StatusBarNotification?) {
        // We don't process notifications — this is just a binding requirement
    }
}
