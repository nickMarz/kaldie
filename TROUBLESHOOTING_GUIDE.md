# Troubleshooting Guide - Quick Solutions

## Common Issues & Fixes

### LEDs Not Lighting
1. **Check power** - External 5V supply connected and ON?
2. **Check GND** - Arduino GND connected to LED GND AND power supply GND?
3. **Check data pin** - Pin 4 connected to LED DIN?
4. **Check for shorts** - Any exposed wires touching?

**Solution that worked**: Found a short in the wiring

### Compilation Errors
**Problem**: Multiple .ino files cause "redefinition" errors
**Solution**: Move test sketches to subfolders:
- `encoder_tests/`
- `led_tests/`

### Encoder Only Goes One Direction
**Problem**: Encoder 2 only increments
**Solution**: Check DT pin (pin 19) connection - was loose

### Too Many Rotations Needed
**Problem**: Takes 5 full rotations to reach 100
**Solution**: Increased VIRTUAL_ROTATION_STEP from 5 to 10

### Animations Not Reactive Enough
**Problem**: Tilt/pan movements don't affect animations much
**Solution**:
- Increased motion multipliers (10x for tilt, 5x for pan)
- Lowered thresholds (TILT_THRESHOLD to 3°)
- Added pan effects to all animations

### First LED Flash Then Nothing
**Problem**: LEDs flash once then stay dark
**Solution**: Wrong protocol - use WS2812B with GRB for WS2818 LEDs

### Motion Sensor Jittery
**Problem**: Animations jump around
**Solution**:
- Increased MOTION_SMOOTHING to 0.25
- Added SENSOR_AT_END compensation
- Added SHAKE_DAMPENING 0.7

---

## Quick Command Reference

### Check Serial Output
```bash
# See what's happening
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=115200
```

### Test Individual Components
```bash
# Test encoders
Open encoder_tests/encoder_test.ino

# Test LEDs with minimal power
Open led_tests/led_minimal_test.ino

# Test motion sensor
Check Serial Monitor for motion data
```

### Verify Wiring
```
LED Strip:
- DIN → Pin 4
- GND → Arduino GND + Power GND
- 5V → External Power

Encoder 1:
- CLK → Pin 3
- DT → Pin 5
- SW → Pin 7

Encoder 2:
- CLK → Pin 18
- DT → Pin 19 (check this if direction issues!)
- SW → Pin 17

MPU-6050:
- SDA → Pin 20
- SCL → Pin 21
```

---

## Configuration Adjustments

### Too Bright/Dim
Edit Config.h:
```cpp
#define DEFAULT_BRIGHTNESS 10  // Change this (1-255)
```

### Motion Too Sensitive
Edit Config.h:
```cpp
#define TILT_THRESHOLD 3.0     // Increase for less sensitive
#define MOTION_SMOOTHING 0.25  // Increase for smoother
```

### Encoder Acceleration Too Fast
Edit Config.h:
```cpp
#define VIRTUAL_ROTATION_STEP 10  // Lower for finer control
```

---

## Power Management

### Safe Testing Power
- Brightness 10 = ~0.4-0.8A (Arduino can handle temporarily)
- Brightness 20 = ~0.8-1.5A (need external supply)
- Brightness 50+ = Need proper 5V 5A+ supply

### Battery Runtime
At brightness 10:
- 20,000mAh = 30+ hours
- 10,000mAh = 15+ hours

---

## If Nothing Else Works

1. **Start minimal**: Upload led_minimal_test.ino
2. **Check basics**: Power, ground, data pin
3. **Look for shorts**: Exposed wires touching
4. **Try different protocol**: WS2811 instead of WS2812B
5. **Check Serial Monitor**: Error messages?
6. **Reduce LED count**: Try NUM_LEDS 10 for testing

---

## Working Configuration
Known working setup as of Jan 31, 2025:
- Arduino Mega 2560
- 209 WS2818 LEDs (using WS2812B protocol, GRB)
- Pin 4 for LED data
- Brightness 10 for safe power
- 120 FPS animation
- Encoders on pins 3,5,7 and 18,19,17
- MPU-6050 on I2C (20,21)