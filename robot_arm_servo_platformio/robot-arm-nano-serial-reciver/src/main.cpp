#include <Arduino.h>
#include <Servo.h>

// ============================================================================
// SERVO TEST - Arduino Nano
// ============================================================================
// This is a simple test program that sweeps a servo through its full range
// and provides serial feedback.
//
// Servo connection:
//   Signal (yellow/white) -> Pin 3 (PWM)
//   VCC (red)             -> 5V
//   GND (brown/black)     -> GND
// ============================================================================

#define SERVO_PIN 5                 // PWM pin for servo control
#define SERIAL_BAUD 115200          // Serial monitor baud rate
#define TEST_DELAY 1000             // Delay between positions (ms)

// Calibration variables for microsecond pulse control
uint16_t minPulseWidth = 500;      // Pulse width for 0 degrees (microseconds)
uint16_t maxPulseWidth = 2500;      // Pulse width for 180 degrees (microseconds)

Servo servo;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD);
  delay(1000);  // Wait for serial monitor to open
  
  // Attach servo to pin
  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  
  // Print startup message
  Serial.println("\n========================================");
  Serial.println("   SERVO CONTROL - Microseconds");
  Serial.println("========================================\n");
  Serial.print("Servo attached to pin: ");
  Serial.println(SERVO_PIN);
  Serial.print("Min pulse width (0°): ");
  Serial.print(minPulseWidth);
  Serial.println(" µs");
  Serial.print("Max pulse width (180°): ");
  Serial.print(maxPulseWidth);
  Serial.println(" µs");
  Serial.println("\nStarting servo test...\n");
  
  delay(500);
}

void loop() {
  // Test 1: Move to 0 degrees (min pulse width)
  Serial.println("Moving servo to 0 degrees...");
  servo.writeMicroseconds(minPulseWidth);
  delay(TEST_DELAY);
  Serial.print("Position: 0 degrees (");
  Serial.print(minPulseWidth);
  Serial.println(" µs)");
  
  // Test 2: Move to 45 degrees (25% of range)
  Serial.println("\nMoving servo to 45 degrees...");
  uint16_t pulse45 = map(45, 0, 180, minPulseWidth, maxPulseWidth);
  servo.writeMicroseconds(pulse45);
  delay(TEST_DELAY);
  Serial.print("Position: 45 degrees (");
  Serial.print(pulse45);
  Serial.println(" µs)");
  
  // Test 3: Move to 90 degrees (center)
  Serial.println("\nMoving servo to 90 degrees (center)...");
  uint16_t pulse90 = map(90, 0, 180, minPulseWidth, maxPulseWidth);
  servo.writeMicroseconds(pulse90);
  delay(TEST_DELAY);
  Serial.print("Position: 90 degrees (");
  Serial.print(pulse90);
  Serial.println(" µs)");
  
  // Test 4: Move to 135 degrees (75% of range)
  Serial.println("\nMoving servo to 135 degrees...");
  uint16_t pulse135 = map(135, 0, 180, minPulseWidth, maxPulseWidth);
  servo.writeMicroseconds(pulse135);
  delay(TEST_DELAY);
  Serial.print("Position: 135 degrees (");
  Serial.print(pulse135);
  Serial.println(" µs)");
  
  // Test 5: Move to 180 degrees (max pulse width)
  Serial.println("\nMoving servo to 180 degrees...");
  servo.writeMicroseconds(maxPulseWidth);
  delay(TEST_DELAY);
  Serial.print("Position: 180 degrees (");
  Serial.print(maxPulseWidth);
  Serial.println(" µs)");
  
  // Test 6: Smooth sweep from 0 to 180
  Serial.println("\nSmooth sweep from 0 to 180 degrees...");
  for (int angle = 0; angle <= 180; angle += 5) {
    uint16_t pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
    servo.writeMicroseconds(pulseWidth);
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("° (");
    Serial.print(pulseWidth);
    Serial.println(" µs)");
    delay(100);  // 100ms per step
  }
  
  // Test 7: Smooth sweep from 180 to 0
  Serial.println("\nSmooth sweep from 180 to 0 degrees...");
  for (int angle = 180; angle >= 0; angle -= 5) {
    uint16_t pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
    servo.writeMicroseconds(pulseWidth);
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print("° (");
    Serial.print(pulseWidth);
    Serial.println(" µs)");
    delay(100);  // 100ms per step
  }
  
  // Return to center position
  Serial.println("\nReturning to center position (90 degrees)...");
  servo.writeMicroseconds(pulse90);
  delay(500);
  
  // Wait before repeating test cycle
  Serial.println("\n========================================");
  Serial.println("Test cycle complete. Repeating...\n");
  delay(3000);
}
