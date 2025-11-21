#!/usr/bin/env python3
"""
Quick usage examples for uart_tester.py

Run any of these examples to test your ESP32 servo controller.
"""

import subprocess
import sys
import os

def run_example(name: str, command: str):
    """Run and display an example"""
    print(f"\n{'='*60}")
    print(f"EXAMPLE: {name}")
    print(f"{'='*60}")
    print(f"Command: {command}")
    print(f"{'='*60}\n")
    
    os.system(command)

def main():
    script = "python3 uart_tester.py"
    
    print("""
╔════════════════════════════════════════════════════════════╗
║    ESP32 SERVO CONTROLLER - UART TESTER EXAMPLES          ║
╚════════════════════════════════════════════════════════════╝

Choose an example to run:

  1. Quick Test          - Single servo command
  2. Full Test Suite     - Comprehensive testing
  3. Interactive Mode    - Manual command entry
  4. Auto-Detect Port    - Find ESP32 automatically
  5. Specific Port       - Use explicit serial port
  6. With Debug Output   - Detailed logging
  7. High Baud Rate      - Test at 115200 baud

Or type a custom command starting with 'python3'
Type 'q' or 'quit' to exit.
    """)
    
    examples = {
        '1': ("Quick Test", f"{script}"),
        '2': ("Full Test Suite", f"{script} --full-test"),
        '3': ("Interactive Mode", f"{script} --interactive"),
        '4': ("Auto-Detect Port", f"{script}"),
        '5': ("Specific Port (Linux)", f"{script} --port /dev/ttyUSB0"),
        '6': ("Specific Port (Windows)", f"{script} --port COM3"),
        '7': ("With Debug Output", f"{script} --debug"),
        '8': ("High Baud Rate", f"{script} --baud 115200"),
        '9': ("Debug + Full Test", f"{script} --debug --full-test"),
    }
    
    while True:
        choice = input("\nSelect example (1-9) or custom command: ").strip().lower()
        
        if choice in ['q', 'quit', 'exit']:
            print("Goodbye!")
            break
        
        elif choice in examples:
            name, command = examples[choice]
            run_example(name, command)
        
        elif choice.startswith('python3'):
            run_example("Custom Command", choice)
        
        else:
            print("Invalid choice. Please select 1-9 or enter a custom command.")

if __name__ == '__main__':
    main()
