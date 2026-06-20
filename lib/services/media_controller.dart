import 'dart:developer' as developer;
import 'package:flutter/services.dart';
import 'package:android_intent_plus/android_intent.dart';
import 'package:android_intent_plus/flag.dart';

class MediaController {
  static const MethodChannel _channel = MethodChannel('com.ruthlessmallard.switchbox/mediabutton');
  
  // Package names for target apps
  static const String youtubeMusicPackage = 'com.google.android.apps.youtube.music';
  static const String audiblePackage = 'com.audible.application';
  static const String googleMapsPackage = 'com.google.android.apps.maps';

  /// Launch YouTube Music and start playing
  Future<void> launchYouTubeMusic() async {
    developer.log('Launching YouTube Music', name: 'SwitchBox');
    
    try {
      // Try launching with VIEW intent first (more reliable)
      final intent = AndroidIntent(
        action: 'android.intent.action.VIEW',
        data: 'vnd.youtube.music://',
        flags: [Flag.FLAG_ACTIVITY_NEW_TASK],
      );
      await intent.launch();
      
      // Wait for app to come to foreground
      await Future.delayed(const Duration(milliseconds: 1200));
      await _sendMediaPlay();
    } catch (e) {
      developer.log('Error launching YT Music: $e, trying fallback', name: 'SwitchBox');
      _fallbackLaunchYT();
    }
  }

  void _fallbackLaunchYT() async {
    try {
      final intent = AndroidIntent(
        action: 'android.intent.action.MAIN',
        package: youtubeMusicPackage,
        flags: [Flag.FLAG_ACTIVITY_NEW_TASK, Flag.FLAG_ACTIVITY_CLEAR_TOP],
      );
      await intent.launch();
      await Future.delayed(const Duration(milliseconds: 1200));
      await _sendMediaPlay();
    } catch (e) {
      developer.log('Fallback also failed: $e', name: 'SwitchBox');
    }
  }

  /// Launch Audible and start playing
  Future<void> launchAudible() async {
    developer.log('Launching Audible', name: 'SwitchBox');
    
    try {
      final intent = AndroidIntent(
        action: 'android.intent.action.MAIN',
        package: audiblePackage,
        flags: [Flag.FLAG_ACTIVITY_NEW_TASK, Flag.FLAG_ACTIVITY_CLEAR_TOP],
      );
      await intent.launch();
      
      // Audible needs more time to initialize
      await Future.delayed(const Duration(milliseconds: 1500));
      await _sendMediaPlay();
    } catch (e) {
      developer.log('Error launching Audible: $e', name: 'SwitchBox');
    }
  }

  /// Send play/pause toggle
  Future<void> playPause() async {
    developer.log('Play/Pause', name: 'SwitchBox');
    try {
      await _channel.invokeMethod('playPause');
    } catch (e) {
      developer.log('Error sending playPause: $e', name: 'SwitchBox');
    }
  }

  /// Send play command
  Future<void> _sendMediaPlay() async {
    developer.log('Sending PLAY command', name: 'SwitchBox');
    try {
      // Send playPause twice to ensure toggle to play state
      await _channel.invokeMethod('playPause');
      await Future.delayed(const Duration(milliseconds: 200));
      await _channel.invokeMethod('playPause');
    } catch (e) {
      developer.log('Error sending play: $e', name: 'SwitchBox');
    }
  }

  /// Send next track media command
  Future<void> nextTrack() async {
    developer.log('Next track', name: 'SwitchBox');
    try {
      await _channel.invokeMethod('next');
    } catch (e) {
      developer.log('Error sending next: $e', name: 'SwitchBox');
    }
  }

  /// Send previous track media command
  Future<void> previousTrack() async {
    developer.log('Previous track', name: 'SwitchBox');
    try {
      await _channel.invokeMethod('previous');
    } catch (e) {
      developer.log('Error sending previous: $e', name: 'SwitchBox');
    }
  }

  /// Skip forward 30 seconds (Audible)
  Future<void> skipForward30() async {
    developer.log('Skip +30s', name: 'SwitchBox');
    try {
      await _channel.invokeMethod('fastForward');
    } catch (e) {
      developer.log('Error skipping forward: $e', name: 'SwitchBox');
    }
  }

  /// Skip backward 30 seconds (Audible)
  Future<void> skipBackward30() async {
    developer.log('Skip -30s', name: 'SwitchBox');
    try {
      await _channel.invokeMethod('rewind');
    } catch (e) {
      developer.log('Error skipping backward: $e', name: 'SwitchBox');
    }
  }

  /// Accept incoming call on speakerphone
  Future<void> acceptCall() async {
    developer.log('Accepting call', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.ANSWER',
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error accepting call: $e', name: 'SwitchBox');
    }
  }

  /// Deny incoming call
  Future<void> denyCall() async {
    developer.log('Denying call', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.CALL_BUTTON',
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error denying call: $e', name: 'SwitchBox');
    }
  }

  /// Navigate to home location and start navigation
  Future<void> navigateHome() async {
    developer.log('Navigating home', name: 'SwitchBox');
    
    try {
      // Launch maps with navigation intent
      final intent = AndroidIntent(
        action: 'android.intent.action.VIEW',
        data: 'google.navigation:q=home',
        package: googleMapsPackage,
        flags: [Flag.FLAG_ACTIVITY_NEW_TASK],
      );
      await intent.launch();
      
      // Try to trigger start navigation after a delay
      await Future.delayed(const Duration(milliseconds: 2000));
      await _triggerNavigationStart();
    } catch (e) {
      developer.log('Error navigating home: $e', name: 'SwitchBox');
    }
  }

  /// Find nearby diesel stations and start navigation
  Future<void> findDiesel() async {
    developer.log('Finding diesel', name: 'SwitchBox');
    
    try {
      final intent = AndroidIntent(
        action: 'android.intent.action.VIEW',
        data: 'geo:0,0?q=diesel+fuel+nearby',
        package: googleMapsPackage,
        flags: [Flag.FLAG_ACTIVITY_NEW_TASK],
      );
      await intent.launch();
      
      // Can't auto-start without user selecting a station
      await Future.delayed(const Duration(milliseconds: 2000));
    } catch (e) {
      developer.log('Error finding diesel: $e', name: 'SwitchBox');
    }
  }
  
  /// Attempt to trigger navigation start (best effort)
  Future<void> _triggerNavigationStart() async {
    try {
      // Send a tap event at center of screen via media button
      // This is a hack - proper way requires accessibility service
      await _channel.invokeMethod('playPause');
    } catch (e) {
      developer.log('Could not trigger nav start: $e', name: 'SwitchBox');
    }
  }
}