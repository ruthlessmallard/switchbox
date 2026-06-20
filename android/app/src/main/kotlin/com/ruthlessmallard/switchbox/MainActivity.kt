package com.ruthlessmallard.switchbox

import android.os.Bundle
import android.view.KeyEvent
import io.flutter.embedding.android.FlutterActivity
import io.flutter.embedding.engine.FlutterEngine

class MainActivity: FlutterActivity() {
    private var sPenPlugin: SPenPlugin? = null
    
    override fun configureFlutterEngine(flutterEngine: FlutterEngine) {
        super.configureFlutterEngine(flutterEngine)
        MediaButtonPlugin.registerWith(flutterEngine, this)
        sPenPlugin = SPenPlugin(this)
        SPenPlugin.registerWith(flutterEngine, this)
    }
    
    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        // Detect S Pen button (Samsung uses KEYCODE_S or KEYCODE_BUTTON_1)
        when (keyCode) {
            KeyEvent.KEYCODE_S, 
            KeyEvent.KEYCODE_BUTTON_1,
            KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE -> {
                if (event?.device?.name?.contains("S Pen", ignoreCase = true) == true ||
                    event?.device?.name?.contains("Stylus", ignoreCase = true) == true) {
                    sPenPlugin?.onSPenButtonPressed()
                    return true
                }
            }
        }
        return super.onKeyDown(keyCode, event)
    }
}