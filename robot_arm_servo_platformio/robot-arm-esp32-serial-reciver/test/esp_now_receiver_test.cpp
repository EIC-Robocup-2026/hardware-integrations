/*
  ESP-NOW Receiver Test
  
  Simple ESP-NOW receiver that:
  - Prints the ESP32 MAC address on startup
  - Listens for incoming ESP-NOW messages
  - Displays sender MAC and received data
  
  Compile & Upload:
    platformio run -e esp32_c6_dev --target upload --target monitor
    
  Or with Arduino IDE:
    - Select ESP32-C6-DevKitC board
    - Set baud rate to 115200
    - Upload and open Serial Monitor
*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Received message structure
typedef struct {
    uint8_t servoId;
    float targetAngle;
} esp_now_message_t;

// Global variables
uint8_t receivedCount = 0;
esp_now_peer_info_t peerInfo = {};

// Callback function for received data
void onDataReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int dataLen) {
    receivedCount++;
    
    Serial.println("\n" + String(61, '='));
    Serial.printf("Message #%d Received\n", receivedCount);
    Serial.println(String(61, '='));
    
    // Display sender MAC
    Serial.print("Sender MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", recvInfo->src_addr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
    
    // Display signal strength
    Serial.printf("RSSI: %d dBm\n", recvInfo->rx_ctrl->rssi);
    
    // Parse message if correct size
    if (dataLen == sizeof(esp_now_message_t)) {
        esp_now_message_t *message = (esp_now_message_t *)data;
        Serial.printf("Servo ID: %d\n", message->servoId);
        Serial.printf("Target Angle: %.1f°\n", message->targetAngle);
    } else {
        // Display raw data
        Serial.printf("Data Length: %d bytes\n", dataLen);
        Serial.print("Raw Data: ");
        for (int i = 0; i < dataLen; i++) {
            Serial.printf("%02X ", data[i]);
        }
        Serial.println();
        
        // Try to interpret as string
        Serial.print("As String: ");
        for (int i = 0; i < dataLen; i++) {
            if (data[i] >= 32 && data[i] <= 126) {
                Serial.write(data[i]);
            } else {
                Serial.print(".");
            }
        }
        Serial.println();
    }
    
    Serial.println(String(61, '='));
}

void setup() {
    // Initialize Serial
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println(String(61, '='));
    Serial.println("ESP-NOW RECEIVER TEST");
    Serial.println(String(61, '='));
    
    // Get and display this device's MAC address
    uint8_t macAddr[6];
    WiFi.macAddress(macAddr);
    
    Serial.println("\nThis Device MAC Address:");
    Serial.print("  ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", macAddr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
    Serial.println("\n(Use this MAC in the sender's configuration)\n");
    
    // Set WiFi to STA mode (required for ESP-NOW)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    Serial.println("WiFi Mode: STATION");
    Serial.printf("WiFi Channel: %d\n", WiFi.channel());
    
    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("\nERROR: Failed to initialize ESP-NOW!");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("ESP-NOW: Initialized");
    
    // Register receive callback
    esp_now_register_recv_cb(onDataReceive);
    
    Serial.println("\nWaiting for incoming messages...\n");
    Serial.println(String(61, '='));
}

void loop() {
    // Print status periodically
    static unsigned long lastStatus = 0;
    unsigned long now = millis();
    
    if (now - lastStatus > 10000) {  // Every 10 seconds
        lastStatus = now;
        
        Serial.printf("\n[Status] Received %d messages so far...\n", receivedCount);
        Serial.printf("[Status] WiFi Channel: %d\n", WiFi.channel());
        Serial.printf("[Status] Free Heap: %u bytes\n\n", ESP.getFreeHeap());
    }
    
    delay(100);
}
