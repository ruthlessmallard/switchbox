import 'dart:convert';
import 'dart:io';
import 'package:flutter/material.dart';

/// UDP communication service for ESP32 configuration
/// Phone connects to ESP32's soft AP (192.168.4.1)
class ESP32UDPService {
  static const String _esp32Ip = '192.168.4.1';
  static const int _udpPort = 4210;
  
  RawDatagramSocket? _socket;
  bool _isInitialized = false;
  
  /// Initialize the UDP socket
  Future<void> initialize() async {
    if (_isInitialized) return;
    
    try {
      _socket = await RawDatagramSocket.bind(InternetAddress.anyIPv4, 0);
      _isInitialized = true;
    } catch (e) {
      throw Exception('Failed to bind UDP socket: $e');
    }
  }
  
  /// Dispose the UDP socket
  void dispose() {
    _socket?.close();
    _socket = null;
    _isInitialized = false;
  }
  
  /// Send brightness offset to ESP32
  /// [offset] -50 to +50, where 0 is default
  Future<bool> sendBrightnessOffset(int offset) async {
    if (!_isInitialized) {
      await initialize();
    }
    
    // Clamp to valid range
    offset = offset.clamp(-50, 50);
    
    final packet = jsonEncode({
      'type': 'config',
      'brightness_offset': offset,
    });
    
    return _sendPacket(packet);
  }
  
  /// Send background color to ESP32
  /// [color] Flutter Color, will be converted to RGB565
  Future<bool> sendColor(Color color) async {
    if (!_isInitialized) {
      await initialize();
    }
    
    // Convert to hex string "#RRGGBB"
    final hexColor = '#${color.red.toRadixString(16).padLeft(2, '0')}'
        '${color.green.toRadixString(16).padLeft(2, '0')}'
        '${color.blue.toRadixString(16).padLeft(2, '0')}';
    
    final packet = jsonEncode({
      'type': 'config',
      'bg_color': hexColor.toUpperCase(),
    });
    
    return _sendPacket(packet);
  }
  
  /// Send both brightness offset and color in one packet
  Future<bool> sendConfig({required int brightnessOffset, required Color color}) async {
    if (!_isInitialized) {
      await initialize();
    }
    
    // Clamp brightness
    brightnessOffset = brightnessOffset.clamp(-50, 50);
    
    // Convert color to hex
    final hexColor = '#${color.red.toRadixString(16).padLeft(2, '0')}'
        '${color.green.toRadixString(16).padLeft(2, '0')}'
        '${color.blue.toRadixString(16).padLeft(2, '0')}';
    
    final packet = jsonEncode({
      'type': 'config',
      'brightness_offset': brightnessOffset,
      'bg_color': hexColor.toUpperCase(),
    });
    
    return _sendPacket(packet);
  }
  
  /// Send a test flash command (sends color then reverts briefly)
  Future<bool> sendTestFlash(Color color, int brightnessOffset) async {
    // Send the test configuration
    return sendConfig(brightnessOffset: brightnessOffset, color: color);
  }
  
  /// Internal method to send UDP packet
  bool _sendPacket(String data) {
    if (_socket == null) return false;
    
    try {
      final targetAddress = InternetAddress(_esp32Ip);
      final bytes = utf8.encode(data);
      
      _socket!.send(bytes, targetAddress, _udpPort);
      
      return true;
    } catch (e) {
      return false;
    }
  }
  
  /// Check if socket is ready
  bool get isInitialized => _isInitialized;
}
