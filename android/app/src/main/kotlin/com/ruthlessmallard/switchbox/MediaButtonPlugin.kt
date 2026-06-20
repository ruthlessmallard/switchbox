package com.ruthlessmallard.switchbox

import android.content.Context
import android.content.Intent
import android.media.AudioManager
import android.media.session.MediaSession
import android.media.session.PlaybackState
import android.os.Build
import android.view.KeyEvent
import io.flutter.embedding.engine.FlutterEngine
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import android.os.Handler
import android.os.Looper

class MediaButtonPlugin(private val context: Context) : MethodChannel.MethodCallHandler {
    companion object {
        const val CHANNEL = "com.ruthlessmallard.switchbox/mediabutton"
        private var mediaSession: MediaSession? = null
        
        fun registerWith(engine: FlutterEngine, context: Context) {
            val channel = MethodChannel(engine.dartExecutor.binaryMessenger, CHANNEL)
            channel.setMethodCallHandler(MediaButtonPlugin(context))
            
            // Initialize media session for better media control
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                mediaSession = MediaSession(context, "SwitchBox").apply {
                    setPlaybackState(PlaybackState.Builder()
                        .setState(PlaybackState.STATE_PLAYING, 0, 1.0f)
                        .setActions(PlaybackState.ACTION_PLAY or 
                                   PlaybackState.ACTION_PAUSE or
                                   PlaybackState.ACTION_SKIP_TO_NEXT or
                                   PlaybackState.ACTION_SKIP_TO_PREVIOUS or
                                   PlaybackState.ACTION_FAST_FORWARD or
                                   PlaybackState.ACTION_REWIND)
                        .build())
                    isActive = true
                }
            }
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
        Handler(Looper.getMainLooper()).post {
            try {
                val audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
                
                // Method 1: Try dispatchMediaKeyEvent (API 19+)
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                    audioManager.dispatchMediaKeyEvent(
                        KeyEvent(KeyEvent.ACTION_DOWN, keyCode)
                    )
                    audioManager.dispatchMediaKeyEvent(
                        KeyEvent(KeyEvent.ACTION_UP, keyCode)
                    )
                } else {
                    // Method 2: Fallback to broadcast (deprecated but works)
                    val intent = Intent(Intent.ACTION_MEDIA_BUTTON)
                    intent.putExtra(Intent.EXTRA_KEY_EVENT, KeyEvent(KeyEvent.ACTION_DOWN, keyCode))
                    context.sendOrderedBroadcast(intent, null)
                    
                    intent.putExtra(Intent.EXTRA_KEY_EVENT, KeyEvent(KeyEvent.ACTION_UP, keyCode))
                    context.sendOrderedBroadcast(intent, null)
                }
            } catch (e: Exception) {
                android.util.Log.e("SwitchBox", "Error sending media key: ${e.message}")
            }
        }
    }
}