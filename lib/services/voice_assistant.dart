import 'dart:developer' as developer;
import 'package:android_intent_plus/android_intent.dart';

/// Voice Assistant integration for hands-free navigation
/// 
/// Uses Samsung Bixby or Google Gemini (via voice) to complete
/// navigation actions that require user interaction
class VoiceAssistant {
  /// Trigger Bixby to navigate to a destination
  /// 
  /// Requires Samsung device with Bixby
  static Future<void> bixbyNavigate(String destination) async {
    developer.log('Triggering Bixby navigation to: $destination', name: 'SwitchBox');
    
    try {
      // Open Bixby with voice command
      final intent = AndroidIntent(
        action: 'android.intent.action.VOICE_COMMAND',
        arguments: <String, dynamic>{
          'query': 'Navigate to $destination',
        },
      );
      await intent.launch();
    } catch (e) {
      developer.log('Bixby error: $e', name: 'SwitchBox');
      // Fallback to Google Assistant
      await googleAssistantNavigate(destination);
    }
  }
  
  /// Trigger Google Assistant for navigation
  static Future<void> googleAssistantNavigate(String destination) async {
    developer.log('Triggering Google Assistant navigation', name: 'SwitchBox');
    
    try {
      final intent = AndroidIntent(
        action: 'android.intent.action.VOICE_COMMAND',
      );
      await intent.launch();
    } catch (e) {
      developer.log('Google Assistant error: $e', name: 'SwitchBox');
    }
  }
  
  /// Open Gemini app with a query
  static Future<void> geminiQuery(String query) async {
    developer.log('Opening Gemini with query', name: 'SwitchBox');
    
    try {
      final intent = AndroidIntent(
        action: 'android.intent.action.SEND',
        package: 'com.google.android.apps.bard',
        type: 'text/plain',
        arguments: <String, dynamic>{
          'android.intent.extra.TEXT': query,
        },
      );
      await intent.launch();
    } catch (e) {
      developer.log('Gemini error: $e, trying web', name: 'SwitchBox');
      // Fallback: open Gemini web
      final webIntent = AndroidIntent(
        action: 'android.intent.action.VIEW',
        data: 'https://gemini.google.com',
      );
      await webIntent.launch();
    }
  }
  
  /// Navigate home using voice assistant
  static Future<void> navigateHome() async {
    // Try Bixby first (Samsung), fallback to Google Assistant
    await bixbyNavigate('home');
  }
  
  /// Find nearest diesel station
  static Future<void> findDieselStation() async {
    await bixbyNavigate('nearest diesel station');
  }
}