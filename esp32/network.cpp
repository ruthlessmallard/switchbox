#include "network.h"

Network::Network() {
    clientConnected = false;
    lastHeartbeat = 0;
    lastClientCheck = 0;
}

void Network::begin() {
    startAccessPoint();
    udp.begin(UDP_PORT);
    
    Serial.println("SwitchBox AP started");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
}

void Network::startAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CHANNEL, false, AP_MAX_CLIENTS);
    
    // Static IP for ESP32
    IPAddress localIP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(localIP, gateway, subnet);
}

void Network::update() {
    unsigned long now = millis();
    
    // Check for client connection periodically
    if (now - lastClientCheck > 2000) {
        lastClientCheck = now;
        checkForClient();
    }
    
    // Send heartbeat
    if (now - lastHeartbeat > UDP_HEARTBEAT_MS) {
        lastHeartbeat = now;
        if (clientConnected) {
            sendHeartbeat();
        }
    }
}

void Network::checkForClient() {
    // WiFi.softAPgetStationNum() returns connected stations
    uint8_t stations = WiFi.softAPgetStationNum();
    bool wasConnected = clientConnected;
    clientConnected = (stations > 0);
    
    if (clientConnected && !wasConnected) {
        Serial.println("Phone connected!");
    } else if (!clientConnected && wasConnected) {
        Serial.println("Phone disconnected");
    }
}

bool Network::isConnected() {
    return (WiFi.softAPIP() != IPAddress(0, 0, 0, 0));
}

bool Network::hasClient() {
    return clientConnected;
}

void Network::sendButtonEvent(uint8_t buttonId, const char *action) {
    if (!clientConnected) return;
    
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
             "{\"type\":\"button\",\"id\":%d,\"action\":\"%s\"}", 
             buttonId, action);
    
    // Broadcast to phone (usually 192.168.4.2)
    IPAddress phoneIP(192, 168, 4, 255);  // Broadcast
    udp.beginPacket(phoneIP, UDP_PORT);
    udp.write((uint8_t*)buffer, strlen(buffer));
    udp.endPacket();
    
    Serial.print("TX: ");
    Serial.println(buffer);
}

bool Network::receivePacket(char *buffer, size_t bufLen) {
    int packetSize = udp.parsePacket();
    if (packetSize <= 0) return false;
    
    int len = udp.read((uint8_t*)buffer, bufLen - 1);
    if (len > 0) {
        buffer[len] = '\0';
        Serial.print("RX: ");
        Serial.println(buffer);
        return true;
    }
    return false;
}

void Network::sendHeartbeat() {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), 
             "{\"type\":\"heartbeat\",\"uptime\":%lu}", millis() / 1000);
    
    IPAddress phoneIP(192, 168, 4, 255);
    udp.beginPacket(phoneIP, UDP_PORT);
    udp.write((uint8_t*)buffer, strlen(buffer));
    udp.endPacket();
}
