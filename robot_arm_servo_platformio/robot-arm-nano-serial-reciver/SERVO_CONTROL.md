# Servo Calibration & Control CLI

This program allows you to calibrate and control a servo motor connected to an Arduino Nano via serial commands.

## Hardware Setup

### Wiring
- **Servo Signal Pin**: GPIO 9 (PWM pin on Arduino Nano)
- **Servo VCC**: 5V (or external power supply)
- **Servo GND**: GND
- **USB**: For serial communication and power

### Supported PWM Pins on Arduino Nano
- Pin 3, 5, 6, 9, 10, 11

## Serial Configuration

- **Baud Rate**: 115200
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Flow Control**: None

## Available Commands

### Control Commands

#### `angle <0-180>`
Set servo angle (0-180 degrees)
```
angle 90        # Move to 90 degrees
angle 0         # Move to 0 degrees
angle 180       # Move to 180 degrees
```

#### `pulse <500-2500>`
Set pulse width directly in microseconds (for manual PWM control)
```
pulse 1500      # Center position (~1500 µs typical)
pulse 1000      # Near minimum
pulse 2000      # Near maximum
```

#### `sweep <start> <end>`
Sweep servo from start angle to end angle (smooth motion)
```
sweep 0 180     # Sweep from 0 to 180 degrees
sweep 180 0     # Sweep from 180 to 0 degrees
sweep 45 135    # Sweep from 45 to 135 degrees
```

### Calibration Commands

#### `calib_min <microseconds>`
Set the pulse width for 0 degrees (minimum calibration point)
```
calib_min 1000  # Set 0° to 1000 µs
```

#### `calib_max <microseconds>`
Set the pulse width for 180 degrees (maximum calibration point)
```
calib_max 2000  # Set 180° to 2000 µs
```

#### `calib_save`
Save current calibration values to EEPROM (persistent storage)
```
calib_save      # Calibration will survive power cycles
```

#### `calib_load`
Load calibration from EEPROM
```
calib_load      # Restore saved calibration
```

#### `calib_show`
Display current calibration values
```
calib_show      # Show min/max pulse widths
```

#### `calib_reset`
Reset calibration to factory defaults
```
calib_reset     # Restore default 1000-2000 µs range
```

### Status & Information

#### `status`
Show current servo status (angle and calibration values)
```
status          # Display servo state
```

#### `help`
Display all available commands
```
help            # Show command list
```

## Calibration Procedure

### For Standard Servo (1000-2000 µs range)

1. Connect servo and power on
2. Open serial monitor at 115200 baud
3. Type: `angle 0` - servo should move to one extreme
4. If servo doesn't move to expected position, adjust:
   ```
   calib_min 950   # Fine-tune minimum
   ```
5. Type: `angle 180` - servo should move to other extreme
6. If needed, adjust:
   ```
   calib_max 2050  # Fine-tune maximum
   ```
7. Test middle position: `angle 90`
8. Once satisfied, save: `calib_save`

### For Non-Standard Servo

1. Find the servo's data sheet for its pulse width range
2. Set minimum: `calib_min <your_min_us>`
3. Set maximum: `calib_max <your_max_us>`
4. Test with sweep: `sweep 0 180`
5. Save: `calib_save`

## Example Session

```
>> help
========== SERVO CONTROL CLI ==========
Commands:
  angle <0-180>        - Set servo angle (0-180 degrees)
  pulse <500-2500>     - Set pulse width in microseconds
  calib_min <us>       - Set minimum pulse width (0 degrees)
  calib_max <us>       - Set maximum pulse width (180 degrees)
  calib_save           - Save calibration to EEPROM
  calib_load           - Load calibration from EEPROM
  calib_reset          - Reset calibration to defaults
  calib_show           - Show current calibration values
  sweep <start> <end>  - Sweep servo from start to end angle
  status               - Show current servo status
  help                 - Show this help message

>> status
Current Angle: 90 degrees
Min Pulse Width: 1000 µs
Max Pulse Width: 2000 µs

>> angle 0
F:Servo angle set to 0 degrees

>> angle 180
F:Servo angle set to 180 degrees

>> calib_show
Min Pulse Width (0°): 1000 µs
Max Pulse Width (180°): 2000 µs

>> calib_save
F:Calibration saved to EEPROM
```

## Features

- ✅ **PWM Control**: Direct control via GPIO pin 9
- ✅ **Angle-based Interface**: Easy 0-180 degree control
- ✅ **Pulse Width Control**: Fine-grained microsecond-level control
- ✅ **EEPROM Storage**: Calibration persists across power cycles
- ✅ **Smooth Sweeping**: Gradual servo movement
- ✅ **Calibration Tools**: Flexible calibration for different servo types
- ✅ **Status Feedback**: Real-time servo state monitoring
- ✅ **Serial CLI**: Simple text-based interface

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Servo doesn't respond | Check wiring, verify pin 9 is connected, check baud rate |
| Servo jitters | Ensure stable power supply (at least 500mA) |
| Wrong angle range | Run `calib_reset` then re-calibrate |
| Servo makes noise | Reduce calibration pulse width range if it's forcing the motor |
| Commands not recognized | Type `help` to verify command syntax (case-insensitive) |

## Default Settings

- **Default Min Pulse Width**: 1000 µs (0 degrees)
- **Default Max Pulse Width**: 2000 µs (180 degrees)
- **Initial Angle**: 90 degrees (center position)
- **Baud Rate**: 115200

## Building & Uploading

```bash
# Build the project
platformio run -e nanoatmega328

# Build and upload
platformio run -e nanoatmega328 -t upload

# Monitor serial output
platformio device monitor -b 115200
```

## Technical Details

- **Microcontroller**: ATmega328P (Arduino Nano)
- **Framework**: Arduino
- **Language**: C++
- **EEPROM Usage**: 5 bytes (addresses 0-4)
  - Address 0-1: Min pulse width (uint16_t)
  - Address 2-3: Max pulse width (uint16_t)
  - Address 4: Valid flag (uint8_t)
