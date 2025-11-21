#!/usr/bin/env python3
"""
UART Tester Quick Reference & Cheat Sheet
"""

QUICK_COMMANDS = {
    "Quick Start": """
    # Auto-detect ESP32 and send test command
    python3 uart_tester.py
    """,
    
    "Full Test": """
    # Run comprehensive test suite
    python3 uart_tester.py --full-test
    """,
    
    "Interactive": """
    # Manual command entry mode
    python3 uart_tester.py --interactive
    """,
    
    "Debug Mode": """
    # Show detailed serial output
    python3 uart_tester.py --debug
    """,
    
    "Linux Port": """
    # Specify USB port on Linux
    python3 uart_tester.py --port /dev/ttyUSB0
    """,
    
    "Windows Port": """
    # Specify COM port on Windows
    python3 uart_tester.py --port COM3
    """,
    
    "Custom Baud": """
    # Use different baud rate
    python3 uart_tester.py --baud 115200
    """,
    
    "Debug + Test": """
    # Full test with debug output
    python3 uart_tester.py --debug --full-test
    """,
}

COMMAND_FORMAT = """
UART COMMAND FORMAT:
  S<servo_id>,<angle>

EXAMPLES:
  S0,90       Move servo 0 to 90°
  S3,120.5    Move servo 3 to 120.5°
  S6,45       Move servo 6 (gripper) to 45°
  S1,0        Move servo 1 to 0° (minimum)
  S5,180      Move servo 5 to 180° (maximum)

VALID RANGES:
  Servo ID: 0-6
  Angle: 0-180 degrees
"""

INTERACTIVE_COMMANDS = """
INTERACTIVE MODE COMMANDS:
  S<id>,<angle>    Send servo command
  A                Test all servos
  R                Test servo range (full sweep)
  E                Test edge cases & errors
  Q                Quit

EXAMPLE SEQUENCE:
  → S0,90          (Move servo 0 to 90°)
  → S3,120.5       (Move servo 3 to 120.5°)
  → A              (Move all servos to 90°)
  → R              (Sweep servo 0 from 0-180°)
  → E              (Test error handling)
  → Q              (Quit)
"""

TROUBLESHOOTING = """
ISSUE: "No ESP32 found"
  SOLUTION: Specify port with --port
  python3 uart_tester.py --port /dev/ttyUSB0

ISSUE: "Connection refused"
  CHECK: USB cable is connected
  CHECK: Port is not in use by another app
  TRY: python3 -m serial.tools.list_ports

ISSUE: No response from ESP32
  CHECK: ESP32 is powered on
  CHECK: Correct baud rate (default 9600)
  TRY: python3 uart_tester.py --debug

ISSUE: Commands don't work
  CHECK: Command format: S<id>,<angle>
  CHECK: Servo ID is 0-6
  CHECK: Angle is 0-180
  TRY: python3 uart_tester.py --interactive --debug

ISSUE: Port not found on Windows
  TRY: Look in Device Manager for COM port
  TRY: python3 uart_tester.py --port COM3

ISSUE: "Permission denied" on Linux
  TRY: Add your user to dialout group
  sudo usermod -a -G dialout $USER
  (Then restart terminal)
"""

TEST_MODES = """
TEST MODES AVAILABLE:

1. QUICK TEST (Default)
   ✓ Single command to servo 0
   ✓ Fast connectivity check
   ✓ 10-15 seconds

2. FULL TEST SUITE (--full-test)
   ✓ Single servo movement
   ✓ All servos test
   ✓ Servo range sweep
   ✓ Edge cases
   ✓ Command rate
   ✓ 1-2 minutes

3. INTERACTIVE MODE (--interactive)
   ✓ Manual command entry
   ✓ Real-time responses
   ✓ Unlimited duration
   ✓ Great for troubleshooting

4. DEBUG MODE (--debug)
   ✓ Serial data logging
   ✓ Real-time output
   ✓ Works with any mode
   ✓ Useful for debugging
"""

SERVO_IDS = """
SERVO MAPPING (7 servos):
  ID  Purpose
  0   Base rotation
  1   Shoulder
  2   Elbow
  3   Wrist pitch
  4   Wrist roll
  5   Wrist yaw
  6   Gripper
"""

EXPECTED_RESULTS = """
EXPECTED TEST RESULTS:

✓ Quick Test
  - Command sends successfully
  - ESP32 responds (or shows no response)
  - No errors
  - <5 seconds

✓ Full Test Suite
  - All 5 test categories pass
  - No servo ID 0/6 errors
  - Angle validation working
  - Command rate: 10-20 cmd/sec
  - <2 minutes

✓ Interactive Mode
  - Commands parse correctly
  - Servo ID validation working
  - Angle range validation working
  - Invalid commands rejected properly

✓ Edge Cases
  - Servo ID 0 & 6 work
  - Angle 0 & 180 work
  - Invalid IDs rejected
  - Invalid angles rejected
  - Decimal angles work
"""

if __name__ == '__main__':
    print("""
╔════════════════════════════════════════════════════════════╗
║     ESP32 SERVO CONTROLLER - UART TESTER QUICK REFERENCE  ║
╚════════════════════════════════════════════════════════════╝
    """)
    
    # Print all sections
    for title, content in [
        ("QUICK COMMANDS", QUICK_COMMANDS),
        ("COMMAND FORMAT", COMMAND_FORMAT),
        ("INTERACTIVE COMMANDS", INTERACTIVE_COMMANDS),
        ("TEST MODES", TEST_MODES),
        ("SERVO MAPPING", SERVO_IDS),
        ("EXPECTED RESULTS", EXPECTED_RESULTS),
        ("TROUBLESHOOTING", TROUBLESHOOTING),
    ]:
        print(f"\n{'='*60}")
        print(f"  {title}")
        print(f"{'='*60}")
        
        if isinstance(content, dict):
            for key, value in content.items():
                print(f"\n▶ {key}:\n{value}")
        else:
            print(content)
    
    print(f"\n{'='*60}")
    print("  QUICK START EXAMPLES")
    print(f"{'='*60}")
    print("""
    # 1. First time - auto-detect:
    python3 uart_tester.py

    # 2. Comprehensive test:
    python3 uart_tester.py --full-test

    # 3. Manual testing:
    python3 uart_tester.py --interactive

    # 4. With detailed output:
    python3 uart_tester.py --debug

    # 5. Specific port + debug:
    python3 uart_tester.py --port /dev/ttyUSB0 --debug --full-test
    """)
    
    print(f"\n{'='*60}")
    print("For full documentation, see: UART_TESTING.md")
    print(f"{'='*60}\n")
