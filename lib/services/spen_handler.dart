import 'dart:async';
import 'dart:developer' as developer;
import 'package:flutter/services.dart';

/// S Pen button handler
/// 
/// NOTE: Samsung S Pen button detection has limitations:
/// - Button only works when screen is on and pen is in proximity
/// - True screen-off detection requires Samsung SDK and special permissions
/// - For production hardware, use BLE HID from ESP32 instead
class SPenHandler {
  static const EventChannel _channel = EventChannel('com.ruthlessmallard.switchbox/spen');
  
  StreamSubscription? _subscription;
  Function? _onButtonPress;
  
  /// Start listening for S Pen button events
  void startListening(Function onButtonPress) {
    _onButtonPress = onButtonPress;
    
    developer.log('Starting S Pen listener', name: 'SwitchBox');
    
    _subscription = _channel.receiveBroadcastStream().listen(
      (event) {
        developer.log('S Pen event: $event', name: 'SwitchBox');
        if (event == 'button_press' && _onButtonPress != null) {
          _onButtonPress!();
        }
      },
      onError: (error) {
        developer.log('S Pen error: $error', name: 'SwitchBox');
      },
    );
  }
  
  /// Stop listening
  void stopListening() {
    _subscription?.cancel();
    _subscription = null;
    developer.log('Stopped S Pen listener', name: 'SwitchBox');
  }
}
