#!/usr/bin/env python3
"""
ESP32 Robot Arm Servo Controller - UART Tester

This script tests the UART receiver on the ESP32 servo controller.
It sends servo commands and monitors the responses.

Usage:
    python3 uart_tester.py [--port PORT] [--baud BAUD] [--debug]

Examples:
    python3 uart_tester.py                          # Auto-detect port, 9600 baud
    python3 uart_tester.py --port /dev/ttyUSB0      # Specific port
    python3 uart_tester.py --port COM3 --baud 115200 # Windows COM port
    python3 uart_tester.py --debug                   # Debug output enabled
"""

import serial
import serial.tools.list_ports
import time
import argparse
import sys
from typing import Optional, List, Tuple


class ESP32ServoTester:
    """UART tester for ESP32 servo controller"""
    
    def __init__(self, port: str = None, baud: int = 9600, debug: bool = False):
        """
        Initialize the tester
        
        Args:
            port: Serial port (auto-detect if None)
            baud: Baud rate (default 9600)
            debug: Enable debug output
        """
        self.baud = baud
        self.debug = debug
        self.serial = None
        self.port = port or self._find_esp32_port()
        
        if not self.port:
            raise RuntimeError("No ESP32 found. Please specify port with --port")
    
    def _find_esp32_port(self) -> Optional[str]:
        """Auto-detect ESP32 serial port"""
        print("Searching for ESP32...")
        ports = serial.tools.list_ports.comports()
        
        for port in ports:
            if 'CH340' in port.description or 'USB' in port.description or 'ESP32' in port.description:
                print(f"Found: {port.device} - {port.description}")
                return port.device
        
        return None
    
    def connect(self) -> bool:
        """Connect to ESP32 serial port"""
        try:
            self.serial = serial.Serial(self.port, self.baud, timeout=1)
            time.sleep(2)  # Wait for ESP32 to reset
            print(f"✓ Connected to {self.port} at {self.baud} baud\n")
            return True
        except Exception as e:
            print(f"✗ Connection failed: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from serial port"""
        if self.serial:
            self.serial.close()
            print("\nDisconnected")
    
    def send_command(self, servo_id: int, angle: float) -> bool:
        """
        Send a servo command
        
        Args:
            servo_id: Servo index (0-6)
            angle: Target angle (0-180)
        
        Returns:
            True if command sent successfully
        """
        if servo_id < 0 or servo_id > 6:
            print(f"✗ Invalid servo ID: {servo_id} (must be 0-6)")
            return False
        
        if angle < 0 or angle > 180:
            print(f"✗ Invalid angle: {angle} (must be 0-180)")
            return False
        
        command = f"S{servo_id},{angle}\n"
        
        try:
            self.serial.write(command.encode())
            if self.debug:
                print(f"→ Sent: {command.strip()}")
            else:
                print(f"→ S{servo_id},{angle}")
            return True
        except Exception as e:
            print(f"✗ Send failed: {e}")
            return False
    
    def read_response(self, timeout: float = 1.0) -> str:
        """
        Read serial response
        
        Args:
            timeout: Read timeout in seconds
        
        Returns:
            Response string
        """
        start_time = time.time()
        response = ""
        
        while time.time() - start_time < timeout:
            if self.serial.in_waiting:
                try:
                    byte = self.serial.read(1).decode('utf-8', errors='ignore')
                    response += byte
                    
                    # Print in real-time if debug enabled
                    if self.debug and byte not in ['\r', '\n']:
                        print(byte, end='', flush=True)
                    
                    if byte == '\n':
                        if self.debug:
                            print()
                        return response.strip()
                except Exception as e:
                    if self.debug:
                        print(f"Read error: {e}")
                    break
            
            time.sleep(0.01)
        
        return response.strip()
    
    def test_single_servo(self, servo_id: int, angle: float, wait_time: float = 0.5):
        """Test a single servo movement"""
        print(f"\n{'='*50}")
        print(f"Testing Servo {servo_id} → {angle}°")
        print(f"{'='*50}")
        
        if self.send_command(servo_id, angle):
            print("Waiting for response...", end=' ')
            response = self.read_response(timeout=2.0)
            if response:
                print(f"← Received: {response[:100]}")
            else:
                print("(no response)")
            
            time.sleep(wait_time)
        
        return True
    
    def test_all_servos(self, angle: float = 90.0, wait_time: float = 0.5):
        """Test all 7 servos"""
        print(f"\n{'='*50}")
        print(f"Testing All Servos → {angle}°")
        print(f"{'='*50}\n")
        
        for servo_id in range(7):
            self.send_command(servo_id, angle)
            time.sleep(wait_time)
    
    def test_servo_range(self, servo_id: int, angles: List[float], wait_time: float = 1.0):
        """Test servo through a range of angles"""
        print(f"\n{'='*50}")
        print(f"Testing Servo {servo_id} Through Range")
        print(f"{'='*50}")
        
        for angle in angles:
            print(f"\nMoving to {angle}°...", end=' ')
            if self.send_command(servo_id, angle):
                response = self.read_response(timeout=1.0)
                if response:
                    print(f"OK")
                else:
                    print(f"(no response)")
            
            time.sleep(wait_time)
    
    def test_speed(self, servo_id: int, num_commands: int = 10, interval: float = 0.1):
        """Test command rate"""
        print(f"\n{'='*50}")
        print(f"Testing Command Rate (Servo {servo_id})")
        print(f"{'='*50}\n")
        
        angles = [90, 45, 135, 90]
        
        print(f"Sending {num_commands} commands with {interval}s interval...")
        start_time = time.time()
        
        for i in range(num_commands):
            angle = angles[i % len(angles)]
            self.send_command(servo_id, angle)
            time.sleep(interval)
        
        elapsed = time.time() - start_time
        rate = num_commands / elapsed
        
        print(f"\nCompleted {num_commands} commands in {elapsed:.2f}s")
        print(f"Rate: {rate:.1f} commands/second")
    
    def test_edge_cases(self):
        """Test edge cases and error handling"""
        print(f"\n{'='*50}")
        print("Testing Edge Cases & Error Handling")
        print(f"{'='*50}\n")
        
        test_cases = [
            ("Valid minimum", 0, 0.0),
            ("Valid maximum", 0, 180.0),
            ("Mid-range", 0, 90.0),
            ("Decimal angle", 0, 45.5),
            ("Another decimal", 3, 120.25),
            ("Invalid servo (high)", 7, 90.0),
            ("Invalid servo (negative)", -1, 90.0),
            ("Invalid angle (high)", 0, 181.0),
            ("Invalid angle (negative)", 0, -1.0),
        ]
        
        for test_name, servo_id, angle in test_cases:
            print(f"{test_name:25} S{servo_id},{angle}", end='... ')
            
            if servo_id < 0 or servo_id > 6 or angle < 0 or angle > 180:
                if self.send_command(servo_id, angle):
                    response = self.read_response(timeout=1.0)
                    if "ERROR" in response or "Invalid" in response:
                        print("✓ (Error handled correctly)")
                    else:
                        print("✓ (Sent, error response expected)")
                else:
                    print("✓ (Rejected locally)")
            else:
                if self.send_command(servo_id, angle):
                    response = self.read_response(timeout=1.0)
                    print("✓")
                else:
                    print("✗ (Failed to send)")
            
            time.sleep(0.1)
    
    def interactive_mode(self):
        """Interactive testing mode"""
        print(f"\n{'='*50}")
        print("INTERACTIVE MODE")
        print(f"{'='*50}")
        print("Commands:")
        print("  S<id>,<angle>  - Send servo command (e.g., S0,90)")
        print("  A              - Test all servos")
        print("  R              - Test servo range")
        print("  E              - Test edge cases")
        print("  Q              - Quit")
        print()
        
        while True:
            try:
                cmd = input("→ ").strip().upper()
                
                if cmd == 'Q':
                    break
                
                elif cmd == 'A':
                    self.test_all_servos(angle=90.0)
                
                elif cmd == 'R':
                    try:
                        servo_id = int(input("  Servo ID (0-6): "))
                        angles = [0, 45, 90, 135, 180]
                        self.test_servo_range(servo_id, angles)
                    except ValueError:
                        print("Invalid input")
                
                elif cmd == 'E':
                    self.test_edge_cases()
                
                elif cmd.startswith('S'):
                    try:
                        parts = cmd[1:].split(',')
                        servo_id = int(parts[0])
                        angle = float(parts[1])
                        self.send_command(servo_id, angle)
                        response = self.read_response(timeout=1.0)
                        if response:
                            print(f"← {response[:100]}")
                    except (IndexError, ValueError):
                        print("Format: S<id>,<angle> (e.g., S0,90)")
                
                else:
                    print("Unknown command")
            
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f"Error: {e}")
    
    def run_full_test(self):
        """Run a comprehensive test suite"""
        print("\n" + "="*50)
        print("ESP32 SERVO CONTROLLER - FULL TEST SUITE")
        print("="*50)
        
        # Test 1: Single servo movement
        print("\n[TEST 1/5] Single Servo Movement")
        self.test_single_servo(servo_id=0, angle=90.0)
        self.test_single_servo(servo_id=0, angle=45.0)
        self.test_single_servo(servo_id=0, angle=135.0)
        
        # Test 2: All servos
        print("\n[TEST 2/5] All Servos")
        self.test_all_servos(angle=90.0, wait_time=0.3)
        
        # Test 3: Servo range
        print("\n[TEST 3/5] Servo Range Test")
        self.test_servo_range(servo_id=2, angles=[0, 45, 90, 135, 180], wait_time=0.5)
        
        # Test 4: Edge cases
        print("\n[TEST 4/5] Edge Cases")
        self.test_edge_cases()
        
        # Test 5: Command rate
        print("\n[TEST 5/5] Command Rate")
        self.test_speed(servo_id=0, num_commands=20, interval=0.05)
        
        print("\n" + "="*50)
        print("TEST SUITE COMPLETE")
        print("="*50)


def main():
    parser = argparse.ArgumentParser(
        description="ESP32 Robot Arm Servo Controller - UART Tester",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                                    # Auto-detect port, 9600 baud
  %(prog)s --port /dev/ttyUSB0                # Specific port
  %(prog)s --port COM3 --baud 115200          # Windows COM port
  %(prog)s --debug                            # Debug output
  %(prog)s --interactive                      # Interactive mode
        """
    )
    
    parser.add_argument(
        '--port',
        type=str,
        default=None,
        help='Serial port (auto-detect if not specified)'
    )
    
    parser.add_argument(
        '--baud',
        type=int,
        default=115200,
        help='Baud rate (default: 115200)'
    )
    
    parser.add_argument(
        '--debug',
        action='store_true',
        help='Enable debug output'
    )
    
    parser.add_argument(
        '--interactive',
        action='store_true',
        help='Interactive testing mode'
    )
    
    parser.add_argument(
        '--full-test',
        action='store_true',
        help='Run full test suite'
    )
    
    args = parser.parse_args()
    
    try:
        # Create tester instance
        tester = ESP32ServoTester(
            port=args.port,
            baud=args.baud,
            debug=args.debug
        )
        
        # Connect
        if not tester.connect():
            sys.exit(1)
        
        try:
            if args.full_test:
                # Run full test suite
                tester.run_full_test()
            
            elif args.interactive:
                # Interactive mode
                tester.interactive_mode()
            
            else:
                # Quick test: move servo 0 to 90°
                print("Running quick test...")
                tester.send_command(0, 90.0)
                response = tester.read_response(timeout=1.0)
                if response:
                    print(f"Response: {response[:100]}")
                
                print("\nFor more options, use:")
                print("  python3 uart_tester.py --full-test    # Full test suite")
                print("  python3 uart_tester.py --interactive   # Interactive mode")
                print("  python3 uart_tester.py --help          # Help")
        
        finally:
            tester.disconnect()
    
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(0)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
