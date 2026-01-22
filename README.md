# Kaleidoscope - Motion-Reactive LED Art Installation

An interactive art installation featuring a motion-reactive LED kaleidoscope. The system uses an accelerometer/gyroscope to detect tilt, rotation, and shake movements, which dynamically control LED animations within a mirrored tube structure.

## Hardware Components

- **ELEGOO MEGA R3 Board ATmega 2560** - Main microcontroller
- **MPU-6050 GY-521** - 3-axis accelerometer and gyroscope module
- **WS2818 LED Strip** - 209 addressable RGB LEDs
  - Uses WS2812B protocol with GRB color order
  - Data pin: Digital Pin 4
- Tripod mount with tilt/pan capabilities
- Large tube with internal mirrors creating infinity mirror effect
- Magnifying glass for viewing

## Physical Design

The LED strip is arranged in 3 segments (70 + 70 + 69 LEDs) running back and forth within a tube. Three mirrors form a triangle inside the tube, creating an infinity mirror kaleidoscope effect. The tube is mounted on a tripod allowing tilting and rotation, with a magnifying glass at one end for viewing.

## Features

### Motion-Reactive Animations

1. **Rainbow** - Color rotation speed changes with movement
2. **Sparkle** - Density and color react to shake and tilt
3. **Wave** - Wave patterns morph based on tilt angle and rotation
4. **Fire** - Flame intensity responds to shake, color shifts with rotation
5. **Pulse** - Pulsing speed and brightness react to all motion types
6. **Kaleidoscope** - Symmetrical patterns across segments with motion control

### Motion Detection

- **Tilt Angle** - Forward/backward and left/right tilt (0-90°)
- **Rotation Speed** - How fast the tube is spinning (0-500°/s)
- **Shake Intensity** - Vibration and sudden movements (0-2G)

All motion data is smoothed and normalized (0-1) for smooth animation control.

## Installation

### 1. Required Arduino Libraries

Install these libraries through Arduino IDE's Library Manager (Sketch → Include Library → Manage Libraries):

- **FastLED** (by Daniel Garcia)
- **Adafruit MPU6050** (by Adafruit)
- **Adafruit Unified Sensor** (by Adafruit)

### 2. Wiring

#### MPU6050 Connections
```
MPU6050    →    Arduino Mega
VCC        →    5V
GND        →    GND
SCL        →    SCL (Pin 21)
SDA        →    SDA (Pin 20)
```

#### WS2818 LED Strip
```
LED Strip  →    Arduino Mega
5V         →    External 5V Power Supply (REQUIRED)
GND        →    GND (connect to both Arduino and Power Supply)
DIN        →    Pin 4
```

#### Optional: Mode Switch Button
```
Button     →    Arduino Mega
One side   →    Pin 2
Other side →    GND
```

**Important:** For 209 LEDs, use an external 5V power supply (at least 50W/10A recommended, 63W/12.5A for full white). Always connect Arduino GND to power supply GND - this is critical for data signal to work!

### 3. Upload Code

1. Download all project files to a folder named `kaleidoscope`
2. Open `kaleidoscope.ino` in Arduino IDE
3. Select **Board**: Arduino Mega or Mega 2560
4. Select the correct **Port**
5. Click **Upload**

### 4. Calibration

On first startup:
1. Place the kaleidoscope in its neutral position (level)
2. Keep it completely still
3. The system will automatically calibrate (LEDs show blue during calibration)
4. After calibration, the kaleidoscope is ready to use

## Usage

### Operation

- **Default Mode**: Kaleidoscope (most visually interesting)
- **Mode Switching**: Press the button on Pin 2 to cycle through animation modes
- **Motion Control**: Tilt, rotate, or shake the tube to see animations react

### Debug Output

Open Serial Monitor (115200 baud) to see:
- Motion sensor data (tilt, rotation, shake)
- Current animation mode
- System status messages

### Customization

#### Adjust LED Brightness

In `Config.h`:
```cpp
#define MAX_BRIGHTNESS 200     // 0-255
#define DEFAULT_BRIGHTNESS 150
```

#### Change Animation Speed

In `Config.h`:
```cpp
#define TARGET_FPS 60  // Frames per second (30-60 recommended)
```

#### Modify Motion Sensitivity

In `Config.h`:
```cpp
#define TILT_THRESHOLD 15.0      // Degrees - lower = more sensitive
#define SHAKE_THRESHOLD 2.0      // G-force
#define ROTATION_THRESHOLD 50    // Degrees/sec
#define MOTION_SMOOTHING 0.2     // 0-1, lower = more smoothing
```

#### Auto-Cycle Modes

Uncomment these lines in `kaleidoscope.ino` to automatically change modes every 30 seconds:
```cpp
// if (currentTime - lastModeChange >= MODE_CHANGE_INTERVAL) {
//   nextMode();
// }
```

## Code Architecture

### Core Classes

- **`Config.h`** - Configuration constants and settings
- **`MotionProcessor`** - Handles MPU6050 sensor reading and motion data processing
- **`LEDController`** - Manages WS2812B LED strip and segment mapping
- **`Animations`** - Animation primitives and motion-reactive effects

### Motion Processing Pipeline

1. Raw sensor data (accelerometer + gyroscope)
2. Orientation calculation (pitch, roll, tilt angle)
3. Motion characteristics (rotation speed, shake intensity)
4. Smoothing and normalization (0-1 values)
5. Animation parameter mapping

### LED Segment Mapping

The 300 LEDs are divided into 3 segments of 100 LEDs each:
- **Segment 0**: LEDs 0-99 (forward direction)
- **Segment 1**: LEDs 100-199 (reversed - folded back)
- **Segment 2**: LEDs 200-299 (forward direction)

This accounts for the back-and-forth physical layout of the strip.

## Troubleshooting

### LEDs Don't Light Up
- Check power supply (300 LEDs need substantial current)
- Verify wiring connections
- Ensure FastLED library is installed

### Motion Sensor Not Working
- Check I2C connections (SDA/SCL)
- Verify MPU6050 address (should be 0x68)
- Try recalibrating

### Jittery Animations
- Increase `MOTION_SMOOTHING` in Config.h (try 0.3-0.4)
- Reduce `TARGET_FPS` if Arduino can't keep up

### Serial Monitor Shows Errors
- Ensure baud rate is set to 115200
- Check that all required libraries are installed

## Creating Custom Animations

To add your own motion-reactive animation:

1. Add a new function in `Animations.h` and `Animations.cpp`:
```cpp
void motionCustom(const MotionData& motion, unsigned long time);
```

2. Add a new mode in `kaleidoscope.ino`:
```cpp
enum AnimationMode {
  // ... existing modes ...
  MODE_CUSTOM,
  MODE_COUNT
};
```

3. Add case in `runCurrentAnimation()`:
```cpp
case MODE_CUSTOM:
  animations.motionCustom(motion, time);
  break;
```

## Performance Tips

- Keep `TARGET_FPS` at 60 or lower
- Use `fadeToBlackBy()` instead of `clear()` for smoother fading
- Avoid `delay()` in animations - use time-based calculations instead
- Test with lower brightness first to avoid power issues

## Power Considerations

**209 WS2818 LEDs at full white**: ~63W (12.5A at 5V)

Recommendations:
- Use external 5V power supply rated for at least 50W (10A)
- `MAX_BRIGHTNESS` is set to 200 to reduce power draw
- Most animations use colored LEDs (much lower power than white)
- **CRITICAL**: Connect Arduino GND to both LED strip GND and Power Supply GND
- See HARDWARE_SETUP.md for detailed wiring and troubleshooting

## License

This is an open-source art project. Feel free to modify and adapt for your own installations!

## Credits

Built using:
- [FastLED](https://fastled.io/) - LED animation library
- [Adafruit MPU6050](https://github.com/adafruit/Adafruit_MPU6050) - Motion sensor library

---

**Have fun creating mesmerizing motion-reactive art! 🌈✨**
