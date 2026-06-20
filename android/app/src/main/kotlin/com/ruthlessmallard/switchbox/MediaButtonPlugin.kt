package com.ruthlessmallard.switchbox

import android.content.Context
import android.media.AudioManager
import android.view.KeyEvent
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel

class MediaButtonPlugin(private val context: Context) : MethodChannel.MethodCallHandler {
    companion object {
        const val CHANNEL = "com.ruthlessmallard.switchbox/mediabutton"
        
        fun registerWith(engine: FlutterEngine, context: Context) {
            val channel = MethodChannel(engine.dartExecutor.binaryMessenger, CHANNEL)
            channel.setMethodCallHandler(MediaButtonPlugin(context))
        }
    }

    override fun onMethodCall(call: MethodCall, result: MethodChannel.Result) {
        when (call.method) {
            "playPause" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE)
                result.success(null)
            }
            "play" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_PLAY)
                result.success(null)
            }
            "pause" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_PAUSE)
                result.success(null)
            }
            "next" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_NEXT)
                result.success(null)
            }
            "previous" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_PREVIOUS)
                result.success(null)
            }
            "fastForward" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_FAST_FORWARD)
                result.success(null)
            }
            "rewind" -> {
                sendMediaButton(KeyEvent.KEYCODE_MEDIA_REWIND)
                result.success(null)
            }
            else -> result.notImplemented()
        }
    }

    private fun sendMediaButton(keyCode: Int) {
        val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
        
        // Send DOWN event
        audioManager.dispatchMediaKeyEvent(
            KeyEvent(KeyEvent.ACTION_DOWN, keyCode)
        )
        
        // Small delay
        Thread.sleep(50)
        
        // Send UP event
        audioManager.dispatchMediaKeyEvent(
            KeyEvent(KeyEvent.ACTION_UP, keyCode)
        )
    }
}