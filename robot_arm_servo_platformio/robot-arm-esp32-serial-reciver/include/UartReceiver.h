#pragma once

#include "CommandReceiver.h"
#include "Config.h"

// ============================================
// UART Command Receiver Implementation
// ============================================

class UartReceiver : public CommandReceiver {
private:
    uint8_t  lastServoId;
    double   lastTargetAngle;
    bool     commandAvailable;
    String   buffer;
    
public:
    UartReceiver() : lastServoId(0), lastTargetAngle(90.0), commandAvailable(false), buffer("") {}
    
    void init() override {
        Serial.begin(UART_BAUD_RATE);
        delay(500);  // Wait for serial to stabilize
        
        if (DEBUG_SERIAL) {
            Serial.println("\n=== UART Receiver Initialized ===");
            Serial.println("Command format: S<id>,<angle>\\n");
            Serial.println("Example: S2,120.5\\n");
        }
    }
    
    void update() override {
        commandAvailable = false;
        
        // Read incoming serial data
        while (Serial.available() > 0) {
            char c = Serial.read();
            
            if (c == '\n' || c == '\r') {
                // Process complete command
                if (buffer.length() > 0) {
                    if (parseCommand(buffer)) {
                        commandAvailable = true;
                        if (DEBUG_SERIAL) {
                            Serial.printf("Command parsed: Servo %d -> %.1f°\n", lastServoId, lastTargetAngle);
                        }
                    } else {
                        if (DEBUG_SERIAL) {
                            Serial.printf("ERROR: Invalid command format: %s\n", buffer.c_str());
                        }
                    }
                    buffer = "";  // Clear buffer
                }
            } else if (c == ' ') {
                // Ignore spaces
                continue;
            } else {
                buffer += c;
            }
        }
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

private:
    /**
     * Parse UART command format: "S<id>,<angle>"
     * Example: "S2,120.5" → servo 2, angle 120.5°
     */
    bool parseCommand(const String &cmd) {
        // Command must start with 'S' or 's'
        if (cmd.length() < 3 || (cmd[0] != 'S' && cmd[0] != 's')) {
            return false;
        }
        
        // Find the comma separator
        int commaPos = cmd.indexOf(',', 1);
        if (commaPos < 0) {
            return false;
        }
        
        // Extract servo ID
        String idStr = cmd.substring(1, commaPos);
        uint8_t id = idStr.toInt();
        
        if (id >= NUM_SERVOS) {
            if (DEBUG_SERIAL) {
                Serial.printf("ERROR: Servo ID %d out of range (0-%d)\n", id, NUM_SERVOS - 1);
            }
            return false;
        }
        
        // Extract angle
        String angleStr = cmd.substring(commaPos + 1);
        double angle = angleStr.toFloat();
        
        // Validate angle range
        if (angle < 0.0 || angle > 180.0) {
            if (DEBUG_SERIAL) {
                Serial.printf("ERROR: Angle %.1f out of range (0-180)\n", angle);
            }
            return false;
        }
        
        lastServoId = id;
        lastTargetAngle = angle;
        
        return true;
    }
};
