#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

#define SERVO_PIN 9                    // PWM pin for servo (Arduino Nano: 3, 5, 6, 9, 10, 11)
#define SERIAL_BAUD 115200             // Serial communication baud rate
#define MIN_PULSE_WIDTH 1000           // Minimum pulse width in microseconds (0 degrees)
#define MAX_PULSE_WIDTH 2000           // Maximum pulse width in microseconds (180 degrees)
#define DEFAULT_MIN_ANGLE 0            // Default minimum angle
#define DEFAULT_MAX_ANGLE 180          // Default maximum angle

// EEPROM storage addresses for calibration data
#define EEPROM_ADDR_MIN_PULSE 0
#define EEPROM_ADDR_MAX_PULSE 2
#define EEPROM_ADDR_VALID_FLAG 4

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

Servo servo;
uint16_t minPulseWidth = MIN_PULSE_WIDTH;
uint16_t maxPulseWidth = MAX_PULSE_WIDTH;
uint8_t currentAngle = 90;             // Current servo angle

// ============================================================================
// EEPROM FUNCTIONS
// ============================================================================

void saveCalibration() {
  EEPROM.put(EEPROM_ADDR_MIN_PULSE, minPulseWidth);
  EEPROM.put(EEPROM_ADDR_MAX_PULSE, maxPulseWidth);
  EEPROM.put(EEPROM_ADDR_VALID_FLAG, 0xAB);  // Valid flag
  Serial.println("F:Calibration saved to EEPROM");
}

void loadCalibration() {
  uint8_t validFlag = EEPROM.read(EEPROM_ADDR_VALID_FLAG);
  
  if (validFlag == 0xAB) {
    EEPROM.get(EEPROM_ADDR_MIN_PULSE, minPulseWidth);
    EEPROM.get(EEPROM_ADDR_MAX_PULSE, maxPulseWidth);
    Serial.println("F:Calibration loaded from EEPROM");
  } else {
    Serial.println("F:No valid calibration found, using defaults");
  }
}

void resetCalibration() {
  minPulseWidth = MIN_PULSE_WIDTH;
  maxPulseWidth = MAX_PULSE_WIDTH;
  EEPROM.put(EEPROM_ADDR_VALID_FLAG, 0xFF);  // Invalid flag
  Serial.println("F:Calibration reset to defaults");
}

// ============================================================================
// SERVO CONTROL FUNCTIONS
// ============================================================================

void setServoAngle(uint8_t angle) {
  // Constrain angle between 0 and 180
  angle = constrain(angle, 0, 180);
  currentAngle = angle;
  
  // Calculate pulse width based on angle
  uint16_t pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
  
  // Write microseconds to servo (direct PWM control)
  servo.writeMicroseconds(pulseWidth);
  
  Serial.print("F:Servo angle set to ");
  Serial.print(angle);
  Serial.println(" degrees");
}

void setPulseMicroseconds(uint16_t microseconds) {
  // Constrain pulse width
  microseconds = constrain(microseconds, 500, 2500);
  servo.writeMicroseconds(microseconds);
  
  // Calculate and display equivalent angle
  uint8_t angle = map(microseconds, minPulseWidth, maxPulseWidth, 0, 180);
  Serial.print("F:Pulse width set to ");
  Serial.print(microseconds);
  Serial.print(" microseconds (approx ");
  Serial.print(angle);
  Serial.println(" degrees)");
}

// ============================================================================
// COMMAND PARSING
// ============================================================================

void printHelp() {
  Serial.println("\n========== SERVO CONTROL CLI ==========");
  Serial.println("Commands:");
  Serial.println("  angle <0-180>        - Set servo angle (0-180 degrees)");
  Serial.println("  pulse <500-2500>     - Set pulse width in microseconds");
  Serial.println("  calib_min <us>       - Set minimum pulse width (0 degrees)");
  Serial.println("  calib_max <us>       - Set maximum pulse width (180 degrees)");
  Serial.println("  calib_save           - Save calibration to EEPROM");
  Serial.println("  calib_load           - Load calibration from EEPROM");
  Serial.println("  calib_reset          - Reset calibration to defaults");
  Serial.println("  calib_show           - Show current calibration values");
  Serial.println("  sweep <start> <end>  - Sweep servo from start to end angle");
  Serial.println("  status               - Show current servo status");
  Serial.println("  help                 - Show this help message");
  Serial.println("========================================\n");
}

void showStatus() {
  Serial.println("\n========== SERVO STATUS ==========");
  Serial.print("Current Angle: ");
  Serial.print(currentAngle);
  Serial.println(" degrees");
  Serial.print("Min Pulse Width: ");
  Serial.print(minPulseWidth);
  Serial.println(" µs");
  Serial.print("Max Pulse Width: ");
  Serial.print(maxPulseWidth);
  Serial.println(" µs");
  Serial.print("Angle Range: 0 - 180 degrees");
  Serial.println("\n==================================\n");
}

void showCalibration() {
  Serial.println("\n========== CALIBRATION DATA ==========");
  Serial.print("Min Pulse Width (0°): ");
  Serial.print(minPulseWidth);
  Serial.println(" µs");
  Serial.print("Max Pulse Width (180°): ");
  Serial.print(maxPulseWidth);
  Serial.println(" µs");
  Serial.println("======================================\n");
}

void processCommand(String command) {
  command.trim();
  command.toLowerCase();
  
  // Parse command
  int spaceIndex = command.indexOf(' ');
  String cmd = spaceIndex > 0 ? command.substring(0, spaceIndex) : command;
  String args = spaceIndex > 0 ? command.substring(spaceIndex + 1) : "";
  
  // ===== ANGLE CONTROL =====
  if (cmd == "angle") {
    if (args.length() > 0) {
      int angle = args.toInt();
      setServoAngle(angle);
    } else {
      Serial.println("E:Usage: angle <0-180>");
    }
  }
  
  // ===== PULSE WIDTH CONTROL =====
  else if (cmd == "pulse") {
    if (args.length() > 0) {
      int pulse = args.toInt();
      setPulseMicroseconds(pulse);
    } else {
      Serial.println("E:Usage: pulse <500-2500>");
    }
  }
  
  // ===== CALIBRATION: SET MIN PULSE =====
  else if (cmd == "calib_min") {
    if (args.length() > 0) {
      uint16_t pulse = args.toInt();
      minPulseWidth = constrain(pulse, 500, 2000);
      Serial.print("F:Min pulse width set to ");
      Serial.print(minPulseWidth);
      Serial.println(" µs");
    } else {
      Serial.println("E:Usage: calib_min <500-2000>");
    }
  }
  
  // ===== CALIBRATION: SET MAX PULSE =====
  else if (cmd == "calib_max") {
    if (args.length() > 0) {
      uint16_t pulse = args.toInt();
      maxPulseWidth = constrain(pulse, 1000, 2500);
      Serial.print("F:Max pulse width set to ");
      Serial.print(maxPulseWidth);
      Serial.println(" µs");
    } else {
      Serial.println("E:Usage: calib_max <1000-2500>");
    }
  }
  
  // ===== CALIBRATION: SAVE =====
  else if (cmd == "calib_save") {
    saveCalibration();
  }
  
  // ===== CALIBRATION: LOAD =====
  else if (cmd == "calib_load") {
    loadCalibration();
  }
  
  // ===== CALIBRATION: RESET =====
  else if (cmd == "calib_reset") {
    resetCalibration();
  }
  
  // ===== CALIBRATION: SHOW =====
  else if (cmd == "calib_show") {
    showCalibration();
  }
  
  // ===== SWEEP =====
  else if (cmd == "sweep") {
    // Parse start and end angles
    int firstSpace = args.indexOf(' ');
    if (firstSpace > 0) {
      String startStr = args.substring(0, firstSpace);
      String endStr = args.substring(firstSpace + 1);
      int startAngle = startStr.toInt();
      int endAngle = endStr.toInt();
      
      startAngle = constrain(startAngle, 0, 180);
      endAngle = constrain(endAngle, 0, 180);
      
      Serial.print("F:Sweeping from ");
      Serial.print(startAngle);
      Serial.print(" to ");
      Serial.println(endAngle);
      
      if (startAngle <= endAngle) {
        for (int angle = startAngle; angle <= endAngle; angle++) {
          setServoAngle(angle);
          delay(50);  // 50ms per step
        }
      } else {
        for (int angle = startAngle; angle >= endAngle; angle--) {
          setServoAngle(angle);
          delay(50);  // 50ms per step
        }
      }
      Serial.println("F:Sweep complete");
    } else {
      Serial.println("E:Usage: sweep <start> <end>");
    }
  }
  
  // ===== STATUS =====
  else if (cmd == "status") {
    showStatus();
  }
  
  // ===== HELP =====
  else if (cmd == "help") {
    printHelp();
  }
  
  // ===== UNKNOWN COMMAND =====
  else {
    Serial.println("E:Unknown command. Type 'help' for available commands.");
  }
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD);
  delay(500);
  
  // Attach servo to pin
  servo.attach(SERVO_PIN, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  
  // Load calibration from EEPROM
  loadCalibration();
  
  // Initialize servo to middle position
  setServoAngle(90);
  
  // Print welcome message
  Serial.println("\n========================================");
  Serial.println("   SERVO CALIBRATION & CONTROL CLI");
  Serial.println("========================================");
  printHelp();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Check for incoming serial data
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    
    if (command.length() > 0) {
      Serial.print(">> ");
      Serial.println(command);
      processCommand(command);
    }
  }
  
  delay(10);  // Small delay to prevent CPU hogging
}
