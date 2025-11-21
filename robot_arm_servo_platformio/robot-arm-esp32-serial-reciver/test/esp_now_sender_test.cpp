/*
  ESP-NOW Sender Test
  
  Simple ESP-NOW sender that:
  - Sends servo command messages to a receiver
  - Shows confirmation of sent messages
  - Interactive mode to send custom commands
  
  IMPORTANT: Update RECEIVER_MAC below with the receiver's MAC address
  
  Compile & Upload:
    platformio run -e esp32_c6_dev --target upload --target monitor
*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Received message structure (must match receiver)
typedef struct {
    uint8_t servoId;
    float targetAngle;
} esp_now_message_t;

// ============================================
// CONFIGURATION - UPDATE THIS WITH RECEIVER MAC
// ============================================
// Example receiver MAC (get this from receiver's serial output)
uint8_t receiverMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Change this!

// Global variables
uint8_t sentCount = 0;
esp_now_peer_info_t peerInfo = {};

// Callback for send confirmation
void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    sentCount++;
    
    Serial.print("Message #");
    Serial.print(sentCount);
    Serial.print(" sent to ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", macAddr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.print(" - Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS ✓" : "FAILED ✗");
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println(String(61, '='));
    Serial.println("ESP-NOW SENDER TEST");
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
    
    Serial.println("\nReceiver MAC Address:");
    Serial.print("  ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", receiverMAC[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();
    
    Serial.println("\n** UPDATE receiverMAC[] WITH ACTUAL RECEIVER MAC **\n");
    
    // Set WiFi to STA mode
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
    
    // Register send callback
    esp_now_register_send_cb(onDataSent);
    
    // Add receiver as peer
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("ERROR: Failed to add peer!");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("Peer: Added");
    Serial.println("\nReady to send messages.");
    Serial.println("Commands:");
    Serial.println("  S<id>,<angle>  - Send servo command (e.g., S0,90)");
    Serial.println("  T              - Send test message to servo 0");
    Serial.println();
}

void loop() {
    // Check for serial input
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input.length() == 0) {
            return;
        }
        
        // Parse command
        if (input.startsWith("S") || input.startsWith("s")) {
            // Parse S<id>,<angle>
            int commaIdx = input.indexOf(',');
            if (commaIdx > 0) {
                int servoId = input.substring(1, commaIdx).toInt();
                float angle = input.substring(commaIdx + 1).toFloat();
                
                if (servoId >= 0 && servoId <= 6 && angle >= 0 && angle <= 180) {
                    esp_now_message_t message;
                    message.servoId = servoId;
                    message.targetAngle = angle;
                    
                    Serial.printf("\nSending: Servo %d → %.1f°\n", servoId, angle);
                    esp_now_send(receiverMAC, (uint8_t *)&message, sizeof(message));
                } else {
                    Serial.println("Invalid servo ID (0-6) or angle (0-180)");
                }
            } else {
                Serial.println("Format: S<id>,<angle> (e.g., S0,90)");
            }
        } 
        else if (input == "T" || input == "t") {
            // Send test message
            esp_now_message_t message;
            message.servoId = 0;
            message.targetAngle = 90.0;
            
            Serial.println("\nSending test message: Servo 0 → 90°");
            esp_now_send(receiverMAC, (uint8_t *)&message, sizeof(message));
        }
        else {
            Serial.println("Unknown command. Use 'S<id>,<angle>' or 'T'");
        }
    }
    
    delay(100);
}
