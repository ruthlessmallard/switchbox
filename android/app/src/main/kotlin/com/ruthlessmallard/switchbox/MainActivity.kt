package com.ruthlessmallard.switchbox

import android.os.Bundle
import io.flutter.embedding.android.FlutterActivity
import io.flutter.embedding.engine.FlutterEngine

class MainActivity: FlutterActivity() {
    override fun configureFlutterEngine(flutterEngine: FlutterEngine) {
        super.configureFlutterEngine(flutterEngine)
        MediaButtonPlugin.registerWith(flutterEngine, this)
        SPenPlugin.registerWith(flutterEngine, this)
    }
}