#pragma once

#include "CommandReceiver.h"
#include "Config.h"
#include <esp_now.h>
#include <WiFi.h>

// ============================================
// ESP-NOW Command Receiver Implementation
// ============================================

class EspNowReceiver : public CommandReceiver {
private:
    uint8_t  lastServoId;
    double   lastTargetAngle;
    bool     commandAvailable;
    static EspNowReceiver* instance;
    
    // Static callback (required by ESP-NOW)
    static void onDataReceived(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int dataLen) {
        if (instance == nullptr) return;
        
        if (dataLen != sizeof(EspNowCommand)) {
            if (DEBUG_SERIAL) {
                Serial.printf("ERROR: Received data with wrong size: %d bytes\n", dataLen);
            }
            return;
        }
        
        const EspNowCommand *cmd = (const EspNowCommand *)data;
        
        // Validate command
        if (cmd->servoId >= NUM_SERVOS) {
            if (DEBUG_SERIAL) {
                Serial.printf("ERROR: Invalid servo ID: %d\n", cmd->servoId);
            }
            return;
        }
        
        if (cmd->targetAngle < 0.0 || cmd->targetAngle > 180.0) {
            if (DEBUG_SERIAL) {
                Serial.printf("ERROR: Invalid angle: %.1f\n", cmd->targetAngle);
            }
            return;
        }
        
        // Store command
        instance->lastServoId = cmd->servoId;
        instance->lastTargetAngle = cmd->targetAngle;
        instance->commandAvailable = true;
        
        if (DEBUG_SERIAL) {
            const uint8_t *macAddr = recvInfo->src_addr;
            Serial.printf("ESP-NOW RX: Servo %d -> %.1f° from MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                         cmd->servoId, cmd->targetAngle,
                         macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        }
    }
    
public:
    EspNowReceiver() : lastServoId(0), lastTargetAngle(90.0), commandAvailable(false) {
        instance = this;
    }
    
    ~EspNowReceiver() {
        instance = nullptr;
        esp_now_deinit();
    }
    
    void init() override {
        // Set Wi-Fi mode to Station
        WiFi.mode(ESPNOW_WIFI_MODE);
        WiFi.disconnect();  // Disconnect from any connected network
        
        // Get ESP32 MAC address
        uint8_t localMac[6];
        WiFi.macAddress(localMac);
        
        if (DEBUG_SERIAL) {
            Serial.println("\n=== ESP-NOW Receiver Initialized ===");
            Serial.printf("Local MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                         localMac[0], localMac[1], localMac[2], localMac[3], localMac[4], localMac[5]);
        }
        
        // Initialize ESP-NOW
        if (esp_now_init() != ESP_OK) {
            if (DEBUG_SERIAL) {
                Serial.println("ERROR: Failed to initialize ESP-NOW");
            }
            return;
        }
        
        // Register the receive callback
        esp_now_register_recv_cb(onDataReceived);
        
        if (DEBUG_SERIAL) {
            Serial.println("Waiting for commands via ESP-NOW...");
        }
    }
    
    void update() override {
        // ESP-NOW uses interrupts, so no update needed
        // Command availability is handled in the callback
    }
    
    bool hasCommand() override {
        return commandAvailable;
    }
    
    bool getCommand(uint8_t &servoId, double &targetAngle) override {
        if (!commandAvailable) {
            return false;
        }
        
        servoId = lastServoId;
        targetAngle = lastTargetAngle;
        commandAvailable = false;
        
        return true;
    }
};

// Static instance pointer initialization
EspNowReceiver* EspNowReceiver::instance = nullptr;
