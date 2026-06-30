#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <WiFiUDP.h>
#include "config.h"

class Network {
public:
    Network();
    void begin();
    void update();  // Call in loop
    
    bool isConnected();
    bool hasClient();  // Phone connected?
    
    // Send button event to phone
    void sendButtonEvent(uint8_t buttonId, const char *action);
    
    // Check for incoming display data from phone
    // Returns true if packet received, caller parses JSON
    bool receivePacket(char *buffer, size_t bufLen);
    
    // Heartbeat
    void sendHeartbeat();
    
private:
    WiFiUDP udp;
    unsigned long lastHeartbeat;
    unsigned long lastClientCheck;
    bool clientConnected;
    
    void startAccessPoint();
    void checkForClient();
};

#endif
