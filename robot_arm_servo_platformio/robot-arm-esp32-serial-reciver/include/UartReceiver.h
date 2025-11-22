#pragma once

#include "CommandReceiver.h"
#include "Config.h"
#include "MotionControl.h"

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
            Serial.println("Command format: CSV of 7 angles for servos 1..7");
            Serial.println("Example: 90,90,90,10,20,30,40\n  (sets servos 0..6 respectively)");
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
    bool parseCommand(const String &cmd) {
        // Expect a CSV of exactly NUM_SERVOS numeric values: "90,90,90,10,20,30,40"
        String work = cmd;
        work.trim();
        if (work.length() == 0) return false;

        int commaCount = 0;
        for (int i = 0; i < work.length(); i++) if (work[i] == ',') commaCount++;
        if (commaCount != (NUM_SERVOS - 1)) {
            if (DEBUG_SERIAL) Serial.println("UART: Invalid CSV format (need 7 values)");
            return false;
        }

        float vals[NUM_SERVOS];
        int start = 0;
        for (int i = 0; i < NUM_SERVOS; i++) {
            int commaPos = work.indexOf(',', start);
            String tok;
            if (commaPos == -1) tok = work.substring(start);
            else tok = work.substring(start, commaPos);
            tok.trim();
            if (tok.length() == 0) {
                if (DEBUG_SERIAL) Serial.println("UART: Empty token in CSV");
                return false;
            }
            vals[i] = tok.toFloat();
            start = (commaPos == -1) ? work.length() : commaPos + 1;
        }

        // Validate against per-servo profile limits and apply
        for (uint8_t i = 0; i < NUM_SERVOS; i++) {
            float a = vals[i];
            const ServoProfile &profile = SERVO_PROFILES[i];
            if (a < (float)profile.minAngleLimit || a > (float)profile.maxAngleLimit) {
                if (DEBUG_SERIAL) Serial.printf("UART: Angle %.1f out of range for servo %d (allowed %.1f-%.1f)\n",
                                               a, i+1, profile.minAngleLimit, profile.maxAngleLimit);
                return false;
            }
        }

        for (uint8_t i = 0; i < NUM_SERVOS; i++) {
            setServoTarget(i, (double)vals[i]);
            if (DEBUG_SERIAL) Serial.printf("UART CSV: Set servo%d -> %.1f°\n", i+1, vals[i]);
        }

        lastServoId = NUM_SERVOS - 1;
        lastTargetAngle = vals[NUM_SERVOS - 1];
        return true;
    }
};
