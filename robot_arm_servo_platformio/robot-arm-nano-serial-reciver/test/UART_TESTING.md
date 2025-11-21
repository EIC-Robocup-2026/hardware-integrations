# UART Receiver Testing Guide

This directory contains Python scripts for testing the ESP32 robot arm servo controller's UART receiver.

## Files

- **`uart_tester.py`** - Main tester script with multiple testing modes
- **`run_examples.py`** - Interactive example runner

## Installation

### Requirements
```bash
pip install pyserial
```

### Verify Installation
```bash
python3 -m serial.tools.list_ports
```

## Quick Start

### 1. Auto-Detect and Quick Test
```bash
python3 uart_tester.py
```

### 2. Full Test Suite
```bash
python3 uart_tester.py --full-test
```

### 3. Interactive Mode
```bash
python3 uart_tester.py --interactive
```

### 4. With Debug Output
```bash
python3 uart_tester.py --debug
```

## Usage Examples

### Basic Command
Send servo 0 to 90°:
```bash
python3 uart_tester.py
# Then in interactive mode: S0,90
```

### Specific Port (Linux)
```bash
python3 uart_tester.py --port /dev/ttyUSB0
```

### Specific Port (Windows)
```bash
python3 uart_tester.py --port COM3
```

### Custom Baud Rate
```bash
python3 uart_tester.py --baud 115200
```

### Debug Mode
```bash
python3 uart_tester.py --debug --full-test
```

## Test Modes

### 1. Quick Test (Default)
- Sends a single command to servo 0
- Good for verifying basic connectivity

### 2. Full Test Suite
Tests 5 categories:
- **Single Servo**: Movement to different angles
- **All Servos**: Simultaneously to 90°
- **Servo Range**: Full 0-180° sweep
- **Edge Cases**: Invalid inputs, boundary values
- **Command Rate**: Throughput testing

Run with: `--full-test`

### 3. Interactive Mode
Manual command entry with options:
- `S<id>,<angle>` - Send servo command
- `A` - Test all servos
- `R` - Test servo range
- `E` - Test edge cases
- `Q` - Quit

Run with: `--interactive`

## Command Format

### UART Protocol
Format: `S<servo_id>,<angle>\n`

Examples:
```
S0,90      # Move servo 0 to 90°
S3,120.5   # Move servo 3 to 120.5°
S6,45      # Move servo 6 (gripper) to 45°
```

### Valid Range
- **Servo ID**: 0-6 (7 servos)
- **Angle**: 0-180 degrees

## Troubleshooting

### "No ESP32 found"
**Solution**: Specify port manually
```bash
python3 uart_tester.py --port /dev/ttyUSB0
```

**Windows**: Find port in Device Manager or use:
```bash
python3 -m serial.tools.list_ports
```

### "Connection failed"
**Check**:
1. USB cable is connected
2. ESP32 drivers are installed
3. Port is not used by another application

**Windows**: Download CH340 or CP210x drivers if needed

### No Response from ESP32
**Check**:
1. ESP32 is powered on
2. Correct baud rate (default 9600)
3. Serial monitor shows initialization messages

### Invalid Commands Accepted
**Check**:
1. ESP32 code has validation enabled
2. Debug output shows command parsing

## Features

### ✅ Auto-Detection
- Automatically finds ESP32 on common ports
- Works on Linux, macOS, Windows

### ✅ Multiple Test Modes
- Quick connectivity test
- Full test suite with 5 test categories
- Interactive manual testing
- Custom command support

### ✅ Edge Case Testing
- Invalid servo IDs
- Out-of-range angles
- Boundary conditions
- Decimal angles

### ✅ Performance Testing
- Command rate measurement
- Response monitoring
- Throughput analysis

### ✅ Debug Support
- Real-time serial output
- Response logging
- Error tracking

## Example Test Session

```bash
$ python3 uart_tester.py --full-test

==================================================
ESP32 SERVO CONTROLLER - FULL TEST SUITE
==================================================

[TEST 1/5] Single Servo Movement
==================================================
Testing Servo 0 → 90.0°
==================================================
→ S0,90
Waiting for response... ← Received: Command parsed: Servo 0 -> 90.0°

Testing Servo 0 → 45.0°
==================================================
→ S0,45
Waiting for response... ← Received: Command parsed: Servo 0 -> 45.0°

[TEST 2/5] All Servos
...
[TEST 5/5] Command Rate
Completed 20 commands in 1.45s
Rate: 13.8 commands/second

==================================================
TEST SUITE COMPLETE
==================================================
```

## Interactive Mode Example

```bash
$ python3 uart_tester.py --interactive

==================================================
INTERACTIVE MODE
==================================================
Commands:
  S<id>,<angle>  - Send servo command (e.g., S0,90)
  A              - Test all servos
  R              - Test servo range
  E              - Test edge cases
  Q              - Quit

→ S0,90
→ Sent: S0,90
← Received: Command parsed: Servo 0 -> 90.0°

→ S3,120.5
→ Sent: S3,120.5
← Received: Command parsed: Servo 3 -> 120.5°

→ A
==================================================
Testing All Servos → 90.0°
==================================================
→ S0,90
...

→ Q
```

## Verifying ESP32 Setup

### 1. Check Initialization
```bash
python3 uart_tester.py --debug
```
Look for initialization messages:
```
Motion control initialized with 7 servos
Ready to receive commands
```

### 2. Test Basic Command
```bash
python3 uart_tester.py
```
Should complete without errors.

### 3. Run Full Test Suite
```bash
python3 uart_tester.py --full-test
```
All tests should pass or show "no response" (not errors).

### 4. Monitor Debug Output
```bash
python3 uart_tester.py --debug --full-test
```
Shows detailed command parsing and responses.

## Performance Metrics

Expected results:
- **Command latency**: <10ms
- **Command rate**: 10-20 commands/second
- **Response time**: <100ms
- **Servo update rate**: 50Hz (20ms intervals)

## Fixing Common Issues

### Servo doesn't respond
1. Check ESP32 is powered on
2. Verify I2C connections (SDA/SCL)
3. Check PCA9685 I2C address (0x40)
4. Review ESP32 serial output

### Commands rejected
1. Verify format: `S<id>,<angle>`
2. Check servo ID (0-6)
3. Check angle (0-180)
4. Enable debug mode for details

### No serial output
1. Check USB connection
2. Verify baud rate (9600)
3. Try different USB port
4. Check CH340 drivers (if needed)

## Next Steps

After successful UART testing:

1. **Calibrate Servos**
   - Test each servo individually
   - Record actual angles vs commanded angles
   - Adjust PWM ranges in Config.h if needed

2. **Test Motion Smoothness**
   - Send rapid commands
   - Observe motion quality
   - Adjust velocity/acceleration profiles

3. **Integrate with Application**
   - Use uart_tester.py as reference
   - Implement servo commands in your code
   - Test coordinated multi-servo movements

## Support

For detailed API documentation, see:
- `../API_REFERENCE.md` - Complete API docs
- `../QUICK_START.md` - Getting started guide
- `../REFACTORING_NOTES.md` - Comprehensive guide

For hardware issues:
- Check `../DEPLOYMENT_CHECKLIST.md`
- Review `../PROJECT_STRUCTURE.md`
- See `../REFACTORING_NOTES.md` troubleshooting section

## Tips & Tricks

### Rapid Testing
```bash
python3 uart_tester.py --full-test --debug
```

### Finding Your Port
```bash
python3 -m serial.tools.list_ports
```

### Custom Test Sequence
Edit `uart_tester.py` and modify the `run_full_test()` method

### Batch Testing
Create a shell script:
```bash
#!/bin/bash
python3 uart_tester.py --port /dev/ttyUSB0 --full-test
```

---

**Happy testing! 🚀**
