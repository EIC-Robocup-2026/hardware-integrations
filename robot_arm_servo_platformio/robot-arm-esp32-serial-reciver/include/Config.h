#pragma once

// ============================================
// COMMAND RECEPTION MODE (select one)
// ============================================
#define COMMAND_MODE_UART    0
#define COMMAND_MODE_ESPNOW  1

// Select which mode to use
#define ACTIVE_COMMAND_MODE  COMMAND_MODE_UART      // Change to COMMAND_MODE_ESPNOW for ESP-NOW
// #define ACTIVE_COMMAND_MODE  COMMAND_MODE_ESPNOW 


// ============================================
// UART Configuration
// ============================================
#define UART_BAUD_RATE      115200

// ============================================
// I2C Configuration
// ============================================
#define I2C_SDA_PIN         5
#define I2C_SCL_PIN         6
#define PCA9685_ADDRESS     0x40
#define PCA9685_FREQUENCY   50  // Hz (standard for servos)

// ============================================
// Servo Configuration
// ============================================
#define NUM_SERVOS          7

// Servo profile structure for each servo
struct ServoProfile {
    uint8_t  pwmChannel;       // PCA9685 channel (0-15)
    double   initAngle;        // Initial position (degrees, 0-180)
    double   maxVelocity;      // Max angular velocity (degrees/second)
    double   maxAcceleration;  // Max angular acceleration (degrees/second²)
    uint16_t minPulseUs;       // Minimum pulse width (microseconds)
    uint16_t maxPulseUs;       // Maximum pulse width (microseconds)
};

// Servo profiles for all 7 servos
// Adjust these values based on your specific servos and requirements
static const ServoProfile SERVO_PROFILES[NUM_SERVOS] = {
    // Servo 0 - Base rotation
    {0,   90.0,  45.0,  15.0,  500,  2500},
    // Servo 1 - Shoulder
    {1,   90.0,  30.0,  10.0,  500,  2500},
    // Servo 2 - Elbow
    {2,   90.0,  30.0,  10.0,  500,  2500},
    // Servo 3 - Wrist pitch
    {3,   90.0,  30.0,  10.0,  500,  2500},
    // Servo 4 - Wrist roll
    {4,   90.0,  30.0,  10.0,  500,  2500},
    // Servo 5 - Wrist yaw
    {5,   90.0,  30.0,  10.0,  500,  2500},
    // Servo 6 - Gripper
    {6,   90.0,  60.0,  20.0,  500,  2500},
};

// ============================================
// Motion Control Parameters
// ============================================
#define MOTION_UPDATE_INTERVAL_MS  20  // Update servos every 20ms (~50Hz)
#define ANGLE_TOLERANCE            0.1  // Consider servo "at target" within this tolerance

// ============================================
// ESP-NOW Configuration (if using ESP-NOW mode)
// ============================================
#define ESPNOW_CHANNEL       1
#define ESPNOW_WIFI_MODE     WIFI_MODE_STA

// MAC address of the sender device (configure this)
// Example: {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
static const uint8_t ESPNOW_SENDER_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ============================================
// ESP-NOW Command Format
// ============================================
struct EspNowCommand {
    uint8_t servoId;       // Servo index (0-6)
    float   targetAngle;   // Target angle in degrees (0-180)
};

// ============================================
// UART Command Format
// ============================================
// Format: "S<id>,<angle>\n"
// Examples:
//   S0,90.5\n   - Set servo 0 to 90.5 degrees
//   S3,120\n    - Set servo 3 to 120 degrees
//   S6,45.2\n   - Set servo 6 (gripper) to 45.2 degrees

// ============================================
// Debug & Serial Output
// ============================================
#define DEBUG_SERIAL         1  // 1 to enable debug messages, 0 to disable
#define DEBUG_BAUD_RATE      115200
