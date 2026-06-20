import 'dart:developer' as developer;
import 'package:android_intent_plus/android_intent.dart';
import 'package:android_intent_plus/flag.dart';

class MediaController {
  // Package names for target apps
  static const String youtubeMusicPackage = 'com.google.android.apps.youtube.music';
  static const String audiblePackage = 'com.audible.application';
  static const String googleMapsPackage = 'com.google.android.apps.maps';

  /// Launch YouTube Music and attempt to play
  Future<void> launchYouTubeMusic() async {
    developer.log('Launching YouTube Music', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.MAIN',
      package: youtubeMusicPackage,
      componentName: '$youtubeMusicPackage/.activities.MainActivity',
      flags: [Flag.FLAG_ACTIVITY_NEW_TASK, Flag.FLAG_ACTIVITY_CLEAR_TOP],
    );
    
    try {
      await intent.launch();
      // Small delay then send media play command
      await Future.delayed(const Duration(milliseconds: 500));
      await _sendMediaPlay();
    } catch (e) {
      developer.log('Error launching YouTube Music: $e', name: 'SwitchBox');
      // Fallback: just open the app
      final fallbackIntent = AndroidIntent(
        action: 'android.intent.action.VIEW',
        data: 'vnd.youtube.music://',
      );
      await fallbackIntent.launch();
    }
  }

  /// Launch Audible and attempt to play
  Future<void> launchAudible() async {
    developer.log('Launching Audible', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.MAIN',
      package: audiblePackage,
      flags: [Flag.FLAG_ACTIVITY_NEW_TASK, Flag.FLAG_ACTIVITY_CLEAR_TOP],
    );
    
    try {
      await intent.launch();
      // Audible should auto-resume if configured
    } catch (e) {
      developer.log('Error launching Audible: $e', name: 'SwitchBox');
    }
  }

  /// Send next track media command
  Future<void> nextTrack() async {
    developer.log('Next track', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.MEDIA_BUTTON',
      arguments: <String, dynamic>{
        'android.intent.extra.KEY_EVENT': <String, dynamic>{
          'action': 0, // ACTION_DOWN
          'code': 87, // KEYCODE_MEDIA_NEXT
        },
      },
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error sending next track: $e', name: 'SwitchBox');
    }
  }

  /// Skip forward 30 seconds (Audible)
  Future<void> skipForward30() async {
    developer.log('Skip +30s', name: 'SwitchBox');
    
    // Audible-specific intent for skip forward
    final intent = AndroidIntent(
      action: 'com.audible.application.ACTION_SKIP_FORWARD',
      package: audiblePackage,
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error skipping forward: $e', name: 'SwitchBox');
      // Fallback: try media fast forward
      await _sendMediaFastForward();
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

  /// Accept incoming call on speakerphone
  Future<void> acceptCall() async {
    developer.log('Accepting call', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.ANSWER',
    );
    
    try {
      await intent.launch();
      // Enable speakerphone after accepting
      await Future.delayed(const Duration(milliseconds: 500));
      await _enableSpeakerphone();
    } catch (e) {
      developer.log('Error accepting call: $e', name: 'SwitchBox');
    }
  }

  /// Navigate to home location
  Future<void> navigateHome() async {
    developer.log('Navigating home', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.VIEW',
      data: 'google.navigation:q=home',
      package: googleMapsPackage,
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error navigating home: $e', name: 'SwitchBox');
    }
  }

  /// Find nearby gas stations
  Future<void> findGasStation() async {
    developer.log('Finding gas station', name: 'SwitchBox');
    
    final intent = AndroidIntent(
      action: 'android.intent.action.VIEW',
      data: 'geo:0,0?q=gas+station',
      package: googleMapsPackage,
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error finding gas station: $e', name: 'SwitchBox');
    }
  }

  // Private helper methods
  
  Future<void> _sendMediaPlay() async {
    final intent = AndroidIntent(
      action: 'android.intent.action.MEDIA_BUTTON',
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error sending play: $e', name: 'SwitchBox');
    }
  }

  Future<void> _sendMediaFastForward() async {
    final intent = AndroidIntent(
      action: 'android.intent.action.MEDIA_BUTTON',
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error sending fast forward: $e', name: 'SwitchBox');
    }
  }

  Future<void> _enableSpeakerphone() async {
    final intent = AndroidIntent(
      action: 'android.intent.action.VOICE_COMMAND',
    );
    
    try {
      await intent.launch();
    } catch (e) {
      developer.log('Error enabling speakerphone: $e', name: 'SwitchBox');
    }
  }
}