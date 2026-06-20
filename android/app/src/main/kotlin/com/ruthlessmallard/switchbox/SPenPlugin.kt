package com.ruthlessmallard.switchbox

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.input.InputManager
import android.view.InputDevice
import android.view.KeyEvent
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.plugin.common.EventChannel

class SPenPlugin(private val context: Context) : EventChannel.StreamHandler {
    companion object {
        const val CHANNEL = "com.ruthlessmallard.switchbox/spen"
        
        fun registerWith(engine: FlutterEngine, context: Context) {
            val channel = EventChannel(engine.dartExecutor.binaryMessenger, CHANNEL)
            channel.setStreamHandler(SPenPlugin(context))
        }
    }
    
    private var eventSink: EventChannel.EventSink? = null
    private var receiver: BroadcastReceiver? = null
    
    override fun onListen(arguments: Any?, events: EventChannel.EventSink?) {
        eventSink = events
        
        // Register for media button events (which S Pen button can trigger)
        receiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context?, intent: Intent?) {
                if (intent?.action == Intent.ACTION_MEDIA_BUTTON) {
                    val event = intent.getParcelableExtra<KeyEvent>(Intent.EXTRA_KEY_EVENT)
                    if (event?.keyCode == KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE) {
                        eventSink?.success("button_press")
                    }
                }
            }
        }
        
        val filter = IntentFilter(Intent.ACTION_MEDIA_BUTTON)
        context.registerReceiver(receiver, filter)
    }
    
    override fun onCancel(arguments: Any?) {
        receiver?.let { context.unregisterReceiver(it) }
        receiver = null
        eventSink = null
    }
}