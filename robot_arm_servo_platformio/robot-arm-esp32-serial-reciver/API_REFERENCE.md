# API Reference Card

## Motion Control API

### Initialization
```cpp
void initMotionControl();
```
Initialize I2C bus, PCA9685 PWM driver, and all servos to initial angles.
Call once in `setup()`.

---

## Motion Commands

### Set Servo Target
```cpp
bool setServoTarget(uint8_t servoId, double targetAngle);
```
**Parameters:**
- `servoId`: Servo index (0-6)
- `targetAngle`: Target angle in degrees (0-180)

**Returns:** `true` if successful, `false` if invalid servo ID

**Example:**
```cpp
setServoTarget(2, 120.5);  // Move servo 2 to 120.5°
```

---

### Update Motion
```cpp
void updateMotion();
```
Update servo motion based on velocity/acceleration profiles.
Call in main loop. Applies smooth motion with constraints.

**Example:**
```cpp
void loop() {
    // ... process commands ...
    updateMotion();  // Apply motion constraints
    delay(20);       // 50 Hz update rate
}
```

---

### Reset to Initial
```cpp
void resetToInitial();
```
Move all servos to their initial angles (defined in Config.h).

**Example:**
```cpp
resetToInitial();  // Home all servos
```

---

### Stop All Servos
```cpp
void stopAllServos();
```
Emergency stop - set all servo targets to current positions.

**Example:**
```cpp
stopAllServos();  // Stop immediately
```

---

## Status Queries

### Get Current Angle
```cpp
double getServoAngle(uint8_t servoId);
```
**Returns:** Current angle in degrees, or -1.0 if invalid servo ID

**Example:**
```cpp
double angle = getServoAngle(3);
Serial.println(angle);  // e.g., 95.5
```

---

### Get Target Angle
```cpp
double getServoTarget(uint8_t servoId);
```
**Returns:** Target angle in degrees, or -1.0 if invalid servo ID

**Example:**
```cpp
if (getServoTarget(2) < 0) {
    Serial.println("Invalid servo");
}
```

---

### Check if Servo Moving
```cpp
bool isServoMoving(uint8_t servoId);
```
**Returns:** `true` if servo is currently moving, `false` if idle

**Example:**
```cpp
if (!isServoMoving(0)) {
    Serial.println("Base servo reached target");
}
```

---

### Check All Servos at Target
```cpp
bool allServosAtTarget();
```
**Returns:** `true` if all servos have reached their targets and are idle

**Example:**
```cpp
while (!allServosAtTarget()) {
    updateMotion();
    delay(20);
}
Serial.println("All done!");
```

---

## Runtime Configuration

### Get Servo Profile
```cpp
ServoProfile getServoProfile(uint8_t servoId);
```
**Returns:** Current servo profile structure

**Profile Structure:**
```cpp
struct ServoProfile {
    uint8_t  pwmChannel;       // PCA9685 channel (0-15)
    double   initAngle;        // Initial position (0-180°)
    double   maxVelocity;      // Max velocity (°/s)
    double   maxAcceleration;  // Max acceleration (°/s²)
    uint16_t minPulseUs;       // Min pulse width (µs)
    uint16_t maxPulseUs;       // Max pulse width (µs)
};
```

**Example:**
```cpp
ServoProfile profile = getServoProfile(0);
Serial.printf("Max velocity: %.1f°/s\n", profile.maxVelocity);
```

---

### Set Servo Profile
```cpp
bool setServoProfile(uint8_t servoId, const ServoProfile &newProfile);
```
Update servo profile at runtime (velocity, acceleration, PWM range).

**Parameters:**
- `servoId`: Servo index (0-6)
- `newProfile`: New profile structure

**Returns:** `true` if successful, `false` if invalid servo ID

**Example:**
```cpp
// Make servo 0 faster
ServoProfile faster = getServoProfile(0);
faster.maxVelocity = 60.0;      // Double speed
faster.maxAcceleration = 20.0;  // Double acceleration
setServoProfile(0, faster);
```

---

### Get Number of Servos
```cpp
uint8_t getNumServos();
```
**Returns:** Total number of servos (NUM_SERVOS = 7)

**Example:**
```cpp
for (int i = 0; i < getNumServos(); i++) {
    Serial.printf("Servo %d: %.1f°\n", i, getServoAngle(i));
}
```

---

## Servo Profile Examples

### Standard Servo (Balanced)
```cpp
{2, 90.0, 30.0, 10.0, 500, 2500}
// Channel 2, 90° init, 30°/s velocity, 10°/s² accel
```

### Fast Servo (Gripper)
```cpp
{6, 90.0, 60.0, 20.0, 500, 2500}
// Fast velocity/acceleration for responsive gripper
```

### Slow Servo (Heavy Load)
```cpp
{1, 90.0, 15.0, 5.0, 500, 2500}
// Slow, gentle motion for heavy components
```

### Custom PWM Range
```cpp
{3, 90.0, 30.0, 10.0, 600, 2400}
// Custom min/max pulse widths for specific servo model
```

---

## Command Receiver API

### UART Receiver
```cpp
#define ACTIVE_COMMAND_MODE COMMAND_MODE_UART

// In setup():
UartReceiver uartReceiver;
uartReceiver.init();

// In loop():
uartReceiver.update();
uint8_t servoId;
double angle;
if (uartReceiver.getCommand(servoId, angle)) {
    setServoTarget(servoId, angle);
}
```

**UART Command Format:**
```
S<servo_id>,<angle>\n
```

**Examples:**
```
S0,90\n      → Servo 0 to 90°
S6,45.5\n    → Servo 6 to 45.5°
```

---

### ESP-NOW Receiver
```cpp
#define ACTIVE_COMMAND_MODE COMMAND_MODE_ESPNOW

// In setup():
EspNowReceiver espNowReceiver;
espNowReceiver.init();

// In loop():
espNowReceiver.update();  // Not needed (interrupt-driven)
uint8_t servoId;
double angle;
if (espNowReceiver.getCommand(servoId, angle)) {
    setServoTarget(servoId, angle);
}
```

**Command Format (Binary):**
```cpp
struct EspNowCommand {
    uint8_t servoId;       // 0-6
    float   targetAngle;   // 0-180
};
```

---

## Complete Example: Coordinated Motion

```cpp
#include <Arduino.h>
#include "MotionControl.h"
#include "UartReceiver.h"
#include "Config.h"

UartReceiver receiver;

void setup() {
    Serial.begin(115200);
    initMotionControl();
    receiver.init();
}

void loop() {
    // Process commands
    receiver.update();
    uint8_t id;
    double angle;
    if (receiver.getCommand(id, angle)) {
        setServoTarget(id, angle);
    }
    
    // Update motion
    updateMotion();
    
    // Print status every second
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        for (int i = 0; i < getNumServos(); i++) {
            Serial.printf("S%d: %6.1f→%6.1f° %s\n",
                i,
                getServoAngle(i),
                getServoTarget(i),
                isServoMoving(i) ? "moving" : "idle"
            );
        }
        lastPrint = millis();
    }
    
    delay(MOTION_UPDATE_INTERVAL_MS);
}
```

---

## Configuration Macros (Config.h)

```cpp
// Communication mode
#define ACTIVE_COMMAND_MODE         COMMAND_MODE_UART
// or COMMAND_MODE_ESPNOW

// UART settings
#define UART_BAUD_RATE              9600
#define DEBUG_BAUD_RATE             115200

// I2C settings
#define I2C_SDA_PIN                 21
#define I2C_SCL_PIN                 22
#define PCA9685_ADDRESS             0x40
#define PCA9685_FREQUENCY           50

// Motion parameters
#define MOTION_UPDATE_INTERVAL_MS   20
#define ANGLE_TOLERANCE             0.1

// Debug
#define DEBUG_SERIAL                1
```

---

## Common Patterns

### Wait for servo to finish
```cpp
while (isServoMoving(servoId)) {
    updateMotion();
    delay(MOTION_UPDATE_INTERVAL_MS);
}
```

### Sequential servo movements
```cpp
setServoTarget(0, 120);
while (isServoMoving(0)) {
    updateMotion();
    delay(20);
}

setServoTarget(1, 90);
while (isServoMoving(1)) {
    updateMotion();
    delay(20);
}
```

### Parallel servo movements
```cpp
setServoTarget(0, 120);
setServoTarget(1, 90);
setServoTarget(2, 135);

while (!allServosAtTarget()) {
    updateMotion();
    delay(20);
}
```

### Get all servo angles
```cpp
for (int i = 0; i < getNumServos(); i++) {
    angles[i] = getServoAngle(i);
}
```

---

## Error Handling

```cpp
// Validate servo ID
if (servoId >= getNumServos()) {
    Serial.println("ERROR: Invalid servo ID");
    return;
}

// Validate angle
if (targetAngle < 0.0 || targetAngle > 180.0) {
    Serial.println("ERROR: Angle out of range");
    return;
}

// Set target safely
if (setServoTarget(servoId, targetAngle)) {
    Serial.println("Command accepted");
} else {
    Serial.println("ERROR: Failed to set target");
}
```

---

## Performance Characteristics

| Parameter | Value |
|-----------|-------|
| Update Rate | 50 Hz (20ms) |
| Angle Resolution | 0.1° |
| Response Time | ~40ms |
| Max Velocity | Configurable (default 30-60°/s) |
| Max Acceleration | Configurable (default 10-20°/s²) |
| Communication Latency | <10ms (UART), <5ms (ESP-NOW) |

---

**Ready to integrate? Start with `QUICK_START.md`!**
