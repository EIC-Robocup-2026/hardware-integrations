# Testing Directory - UART Receiver Tests

This directory contains comprehensive testing utilities for the ESP32 robot arm servo controller's UART receiver.

## 📁 Test Files

### Main Testing Scripts

1. **`uart_tester.py`** (430 lines) ⭐ **Main Script**
   - Full-featured UART tester with multiple modes
   - Auto-detects ESP32 serial port
   - Comprehensive test suites
   - Interactive command entry
   - Edge case testing
   - Performance benchmarking

2. **`run_examples.py`** (74 lines)
   - Interactive example runner
   - Menu-driven interface
   - Quick access to common test scenarios
   - Helpful for first-time users

3. **`UART_TESTING.md`** (353 lines) 📖 **Documentation**
   - Complete usage guide
   - Installation instructions
   - Multiple test modes explained
   - Troubleshooting guide
   - Performance metrics

4. **`CHEAT_SHEET.py`** (232 lines)
   - Quick reference card
   - Common commands
   - Servo mapping
   - Expected results
   - Run with: `python3 CHEAT_SHEET.py`

## 🚀 Quick Start

### Step 1: Install Dependencies
```bash
pip install pyserial
```

### Step 2: Run Quick Test
```bash
python3 uart_tester.py
```

### Step 3: Run Full Test Suite
```bash
python3 uart_tester.py --full-test
```

### Step 4: Try Interactive Mode
```bash
python3 uart_tester.py --interactive
```

## 🎯 Usage Examples

### Auto-Detect ESP32 (Default)
```bash
python3 uart_tester.py
```

### Specific Serial Port
```bash
python3 uart_tester.py --port /dev/ttyUSB0        # Linux
python3 uart_tester.py --port COM3                # Windows
```

### With Debug Output
```bash
python3 uart_tester.py --debug
python3 uart_tester.py --debug --full-test
```

### High Baud Rate
```bash
python3 uart_tester.py --baud 115200
```

### Interactive Testing
```bash
python3 uart_tester.py --interactive
```

## 📋 UART Command Format

Send commands to your ESP32:

```
S<servo_id>,<angle>
```

**Examples:**
```
S0,90      → Move servo 0 to 90°
S3,120.5   → Move servo 3 to 120.5°
S6,45      → Move servo 6 (gripper) to 45°
```

**Valid Ranges:**
- Servo ID: 0-6 (7 servos)
- Angle: 0-180 degrees

## 🧪 Test Modes

### 1. Quick Test (Default)
- Single command to servo 0
- Verify basic connectivity
- **Time**: ~10 seconds

```bash
python3 uart_tester.py
```

### 2. Full Test Suite
Tests 5 categories:
- Single servo movement
- All servos simultaneously
- Servo range sweep (0-180°)
- Edge cases and errors
- Command rate benchmarking

**Time**: ~1-2 minutes

```bash
python3 uart_tester.py --full-test
```

### 3. Interactive Mode
Manual command entry with options:
- `S<id>,<angle>` - Send command
- `A` - Test all servos
- `R` - Servo range sweep
- `E` - Edge case testing
- `Q` - Quit

```bash
python3 uart_tester.py --interactive
```

### 4. Debug Mode
Real-time serial data logging:

```bash
python3 uart_tester.py --debug
```

## 🧪 What Each Test Checks

### Single Servo Test
✓ Individual servo responds to commands  
✓ Servo moves to specified angles  
✓ Response includes command confirmation  

### All Servos Test
✓ Multiple servos move simultaneously  
✓ No conflicts on I2C bus  
✓ All 7 servos operational  

### Servo Range Test
✓ Full 0-180° movement works  
✓ Smooth motion across range  
✓ End-stop positions safe  
✓ Decimal angles supported  

### Edge Cases
✓ Invalid servo IDs rejected  
✓ Out-of-range angles rejected  
✓ Boundary conditions handled  
✓ Error messages clear  

### Command Rate
✓ Throughput measurement  
✓ Latency acceptable  
✓ No command drops  
✓ Stable performance  

## 📊 Expected Results

| Test | Expected | Time |
|------|----------|------|
| Quick | ✓ All pass | 10s |
| Full | ✓ All pass | 2min |
| Interactive | ✓ Commands work | ∞ |
| Edge Cases | ✓ Proper validation | 30s |
| Command Rate | 10-20 cmd/sec | 20s |

## 🔧 Troubleshooting

### "No ESP32 found"
```bash
# Specify port manually
python3 uart_tester.py --port /dev/ttyUSB0

# Or find port
python3 -m serial.tools.list_ports
```

### "Connection refused"
- Check USB cable
- Check port is not in use
- Install CH340 drivers if needed

### No response from ESP32
```bash
# Try debug mode
python3 uart_tester.py --debug

# Check initialization messages
# Look for: "Motion control initialized"
```

### Commands not accepted
- Check format: `S<id>,<angle>`
- Verify servo ID (0-6)
- Verify angle (0-180)
- Enable debug mode

## 💡 Pro Tips

### Quick Verification
```bash
# Test all in one command
python3 uart_tester.py --full-test --debug
```

### Find Your Port
```bash
python3 -m serial.tools.list_ports
```

### Monitor Real-Time
```bash
# Keep testing while observing ESP32 behavior
python3 uart_tester.py --interactive --debug
```

### Batch Testing
Create a shell script:
```bash
#!/bin/bash
python3 uart_tester.py --port /dev/ttyUSB0 --full-test
```

## 📝 Test Session Example

```
$ python3 uart_tester.py --full-test

==================================================
ESP32 SERVO CONTROLLER - FULL TEST SUITE
==================================================

[TEST 1/5] Single Servo Movement
→ S0,90
← Received: Command parsed: Servo 0 -> 90.0°

[TEST 2/5] All Servos
→ S0,90
→ S1,90
...

[TEST 3/5] Servo Range Test
→ S2,0
→ S2,45
→ S2,90
→ S2,135
→ S2,180

[TEST 4/5] Edge Cases
Testing Invalid servo ID 7... ✓ (Rejected)
Testing Angle 181... ✓ (Rejected)

[TEST 5/5] Command Rate
Completed 20 commands in 1.45s
Rate: 13.8 commands/second

==================================================
TEST SUITE COMPLETE
==================================================
```

## 🎓 Learning Resources

- **UART_TESTING.md** - Complete testing guide
- **CHEAT_SHEET.py** - Quick reference
- **../API_REFERENCE.md** - UART command format details
- **../QUICK_START.md** - System getting started

## ✅ Pre-Deployment Checklist

Before deploying to production:

- [ ] Run `uart_tester.py --full-test`
- [ ] All 7 servos respond correctly
- [ ] Servo angles are accurate
- [ ] Motion is smooth
- [ ] No command errors
- [ ] Response times acceptable
- [ ] Edge case handling works

## 🚀 Next Steps

After successful testing:

1. **Calibrate Servos**
   - Verify actual vs commanded angles
   - Adjust PWM ranges if needed

2. **Test Motion Smoothness**
   - Observe servo movement
   - Adjust velocity/acceleration profiles

3. **Integrate with Application**
   - Use command format in your code
   - Test coordinated movements
   - Deploy to robot

## 📞 Support

### Getting Help
1. Check **UART_TESTING.md** for detailed guide
2. Run with `--debug` flag for details
3. Review **../REFACTORING_NOTES.md** troubleshooting

### Common Issues
See **UART_TESTING.md** "Troubleshooting" section

### Reporting Issues
- Include test output with `--debug`
- Specify port and baud rate
- Show command that failed
- Share ESP32 serial initialization output

## 📄 File Manifest

```
test/
├── uart_tester.py          (430 lines) - Main tester
├── run_examples.py         (74 lines)  - Example runner
├── CHEAT_SHEET.py          (232 lines) - Quick reference
├── UART_TESTING.md         (353 lines) - Full documentation
├── 2D_manipulator_ik.py    (old file)  - Kinematics test
├── CyclicCoordinateDescent2D.py (old)  - Kinematics test
├── test.cpp                (old file)  - C++ tests
├── manipulator.xml         (old file)  - Config file
└── README.md               (this file) - Directory guide
```

---

## 🎉 Summary

You now have comprehensive UART testing capabilities:

✅ **Main Tester** - Full-featured uart_tester.py  
✅ **Multiple Modes** - Quick, Full, Interactive, Debug  
✅ **Auto-Detection** - Finds ESP32 automatically  
✅ **Complete Docs** - UART_TESTING.md guide  
✅ **Quick Reference** - CHEAT_SHEET.py  
✅ **Edge Case Tests** - Validates error handling  
✅ **Performance Metrics** - Throughput benchmarking  

**Status: Ready to test! 🚀**

Start with: `python3 uart_tester.py --full-test`
