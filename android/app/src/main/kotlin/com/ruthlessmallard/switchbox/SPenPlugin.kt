package com.ruthlessmallard.switchbox

import android.content.Context
import android.hardware.input.InputManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.view.InputDevice
import android.view.KeyEvent
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.plugin.common.EventChannel
import java.util.Timer
import java.util.TimerTask

class SPenPlugin(private val context: Context) : EventChannel.StreamHandler {
    companion object {
        const val CHANNEL = "com.ruthlessmallard.switchbox/spen"
        
        fun registerWith(engine: FlutterEngine, context: Context) {
            val channel = EventChannel(engine.dartExecutor.binaryMessenger, CHANNEL)
            channel.setStreamHandler(SPenPlugin(context))
        }
    }
    
    private var eventSink: EventChannel.EventSink? = null
    private var timer: Timer? = null
    private var lastButtonState = false
    
    override fun onListen(arguments: Any?, events: EventChannel.EventSink?) {
        eventSink = events
        
        // Poll for S Pen button state (Samsung-specific)
        timer = Timer().apply {
            scheduleAtFixedRate(object : TimerTask() {
                override fun run() {
                    checkSPenButton()
                }
            }, 0, 100) // Check every 100ms
        }
    }
    
    override fun onCancel(arguments: Any?) {
        timer?.cancel()
        timer = null
        eventSink = null
    }
    
    private fun checkSPenButton() {
        try {
            // Check if S Pen is present and button is pressed
            // This is Samsung-specific and uses undocumented APIs
            // Fallback: detect via InputDevice
            val inputManager = context.getSystemService(Context.INPUT_SERVICE) as InputManager
            val deviceIds = inputManager.inputDeviceIds
            
            var sPenFound = false
            var buttonPressed = false
            
            for (deviceId in deviceIds) {
                val device = inputManager.getInputDevice(deviceId)
                if (device != null && (device.name?.contains("S Pen", ignoreCase = true) == true ||
                    device.name?.contains("Stylus", ignoreCase = true) == true)) {
                    sPenFound = true
                    
                    // Try to detect button via KeyCharacterMap or motion range
                    // This is a best-effort approach
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                        // Check if device has button capability
                        val sources = device.sources
                        if (sources and InputDevice.SOURCE_STYLUS != 0) {
                            // S Pen detected - button state requires polling
                            // which isn't directly accessible
                        }
                    }
                }
            }
            
            // Alternative: Listen for KeyEvent.KEYCODE_S (Samsung S Pen button)
            // This requires Activity-level dispatchKeyEvent, not available here
            
        } catch (e: Exception) {
            // Silent fail - S Pen detection is best-effort
        }
    }
    
    // Called from MainActivity when S Pen button key event detected
    fun onSPenButtonPressed() {
        Handler(Looper.getMainLooper()).post {
            eventSink?.success("button_press")
        }
    }
}