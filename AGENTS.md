# Kaleidoscope - Motion-Reactive LED Art Installation

## Project Overview

Arduino C++ project for a motion-reactive LED kaleidoscope art installation. A tube with 209 WS2818 LEDs, 3 internal mirrors forming an infinity-mirror kaleidoscope effect, mounted on a tripod with tilt/pan. Motion from an MPU-6050 accelerometer/gyroscope drives 6 animation modes. Two KY-040 rotary encoders provide manual control.

## Hardware

- **Board**: ELEGOO MEGA R3 (ATmega 2560)
- **LEDs**: 209 WS2818 (uses WS2812B protocol, GRB color order) on pin 4
  - 3 segments: 70 + 70 + 69 LEDs, back-and-forth layout (segment 1 reversed)
- **Motion sensor**: MPU-6050 GY-521 on I2C (SDA=20, SCL=21), mounted at viewing end of tube
- **Encoder 1** (mode select): CLK=3, DT=5, SW=7
- **Encoder 2** (intensity/virtual rotation): CLK=18, DT=19, SW=17
- **Power**: External 5V 10A+ PSU for LEDs; Arduino 5V for sensors/encoders

## Build & Upload

```bash
# Install Arduino CLI (if needed)
brew install arduino-cli

# Install required libraries
arduino-cli lib install "FastLED"
arduino-cli lib install "Adafruit MPU6050"
arduino-cli lib install "Adafruit Unified Sensor"

# Compile
arduino-cli compile --fqbn arduino:avr:mega kaleidoscope

# Upload (port varies)
arduino-cli upload -p /dev/cu.usbmodem* --fqbn arduino:avr:mega kaleidoscope

# Serial monitor
arduino-cli monitor -p /dev/cu.usbmodem* -c baudrate=115200
```

Or use Arduino IDE: open `kaleidoscope.ino`, select Board: Arduino Mega 2560, upload.

## File Structure

```
kaleidoscope.ino          # Main sketch (setup/loop, mode switching, animation dispatch)
Config.h                  # All configuration constants (pins, thresholds, timing)
LEDController.cpp/h       # LED strip management, segment mapping, color utilities
Animations.cpp/h          # 6 motion-reactive animations + primitives
MotionProcessor.cpp/h     # MPU-6050 reading, orientation, smoothing, normalization
EncoderHandler.cpp/h      # Dual encoder with interrupts, acceleration, buttons
ColorModes.h              # Color palette manager (7 palettes, not yet integrated)
encoder_test/             # Basic encoder test sketch
encoder_tests/            # Additional encoder test/diagnostic sketches
led_tests/                # LED test sketches (minimal, simple)
```

## Key Architecture

- **MotionData struct** (MotionProcessor.h): Central data structure with raw sensor values, processed orientation (pitch/roll/tiltAngle), normalized 0-1 values (tiltNormalized, rotationNormalized, shakeNormalized), pan, and virtual rotation from encoder 2.
- **Animations**: Each `motion*()` method takes `const MotionData&` and `unsigned long time`, maps motion to visual params. Tilt/pan are primary inputs (no physical rotation axis). Virtual rotation via encoder 2 supplements.
- **Segments**: LEDController maps 3 physical segments with segment 1 reversed. Use `setSegmentPixel()` for segment-aware addressing.
- **AnimationMode enum** in kaleidoscope.ino: MODE_RAINBOW through MODE_KALEIDOSCOPE (6 modes).

## Current Config (Config.h)

- `DEFAULT_BRIGHTNESS 10` (very low for safe power)
- `MAX_BRIGHTNESS 50`
- `TARGET_FPS 120`
- `MPU_UPDATE_RATE 100` Hz
- `MOTION_SMOOTHING 0.25`
- `TILT_THRESHOLD 3.0` (very sensitive)
- `SENSOR_AT_END true` with `SHAKE_DAMPENING 0.7`
- `AUTO_CYCLE_MODES false` (encoders control modes)
- `VIRTUAL_ROTATION_STEP 10`

## Code Conventions

- Arduino C++ style (`.ino`, `.cpp`, `.h`)
- Header guards with `#ifndef`/`#define`
- Classes for each subsystem, global instances in `.ino`
- FastLED types: `CRGB`, `CHSV`, `fill_solid`, `rgb2hsv_approximate`
- Motion values normalized to 0.0-1.0 for animation mapping
- `constrain()` for bounds, `map()` for range conversion
- Serial debug output at 115200 baud
- Interrupt-driven encoder reading (ISR with static instance pointer pattern)

## Unimplemented Features

- ColorModes.h exists but is not integrated into animations yet (encoder 2 button -> color palette toggle)
- Sound reactivity (MSGEQ7 module, see SOUND_REACTIVE_ADDON.md)
- Symmetry modes, effect layers, freeze frame (see BUTTON_FEATURES_ROADMAP.md)

## Important Notes

- WS2818 LEDs use WS2812B protocol with GRB color order — do not change
- Test sketches must live in subfolders (encoder_tests/, led_tests/) to avoid compilation conflicts with main .ino
- Common ground between Arduino, LED strip, and PSU is critical
- Sensor is at the viewing end of the tube (amplifies tilt/shake), compensated in code
