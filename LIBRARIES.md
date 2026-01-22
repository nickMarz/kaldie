# Required Libraries

This document lists all the Arduino libraries required for the Kaleidoscope project.

## Installation Methods

### Method 1: Arduino IDE Library Manager (Recommended)

1. Open Arduino IDE
2. Go to **Sketch** → **Include Library** → **Manage Libraries...**
3. Search for each library name below
4. Click **Install**

### Method 2: Manual Installation

1. Download library ZIP files from the links provided
2. Go to **Sketch** → **Include Library** → **Add .ZIP Library...**
3. Select the downloaded ZIP file

## Required Libraries

### 1. FastLED
- **Author**: Daniel Garcia
- **Version**: 3.6.0 or later
- **Purpose**: Controls WS2812B addressable LED strip with high performance
- **GitHub**: https://github.com/FastLED/FastLED
- **Documentation**: https://fastled.io/

**Features Used:**
- WS2812B driver
- Color math (CRGB, CHSV)
- Built-in effects (gradients, blurs)
- FastLED.show(), FastLED.setBrightness()

### 2. Adafruit MPU6050
- **Author**: Adafruit Industries
- **Version**: 2.2.0 or later
- **Purpose**: Interface with MPU6050 accelerometer/gyroscope sensor
- **GitHub**: https://github.com/adafruit/Adafruit_MPU6050
- **Documentation**: https://learn.adafruit.com/mpu6050-6-dof-accelerometer-and-gyro

**Features Used:**
- Sensor initialization and configuration
- Reading accelerometer and gyroscope data
- Built-in calibration support

### 3. Adafruit Unified Sensor
- **Author**: Adafruit Industries
- **Version**: 1.1.9 or later
- **Purpose**: Common sensor interface (required by Adafruit MPU6050)
- **GitHub**: https://github.com/adafruit/Adafruit_Sensor

**Note:** This is a dependency of Adafruit MPU6050 and should be automatically installed, but install it manually if you get compile errors.

### 4. Wire (Built-in)
- **Purpose**: I2C communication library for Arduino
- **Note**: This is a built-in Arduino library, no installation needed

## Installation Verification

After installing the libraries, verify they're installed correctly:

1. Open Arduino IDE
2. Go to **Sketch** → **Include Library**
3. You should see these libraries listed:
   - FastLED
   - Adafruit MPU6050
   - Adafruit Unified Sensor
   - Wire (under "Built-in Libraries")

## Library Manager Search Terms

Use these exact search terms in the Library Manager:

```
FastLED
Adafruit MPU6050
Adafruit Unified Sensor
```

## Version Compatibility

### Tested Versions
This project has been tested with:
- **FastLED**: 3.6.0
- **Adafruit MPU6050**: 2.2.4
- **Adafruit Unified Sensor**: 1.1.14
- **Arduino IDE**: 2.2.1

### Minimum Versions
- **FastLED**: 3.5.0
- **Adafruit MPU6050**: 2.2.0
- **Adafruit Unified Sensor**: 1.1.0

## Troubleshooting Library Issues

### "Library not found" error
1. Restart Arduino IDE after installing libraries
2. Check that library is in the correct folder:
   - Windows: `Documents\Arduino\libraries\`
   - Mac: `~/Documents/Arduino/libraries/`
   - Linux: `~/Arduino/libraries/`

### "Multiple libraries found" warning
If you see this warning, you have duplicate library versions installed. Keep only the latest version.

### Compilation errors after library update
Sometimes library updates introduce breaking changes:
1. Try using the tested versions listed above
2. Check library documentation for changes
3. Update all libraries to their latest versions

### Can't find Library Manager
**Arduino IDE 1.x**: Sketch → Include Library → Manage Libraries...
**Arduino IDE 2.x**: Library Manager icon in left sidebar

## Quick Install Script

If you're comfortable with command line, you can use Arduino CLI:

```bash
# Install Arduino CLI first: https://arduino.github.io/arduino-cli/

# Install libraries
arduino-cli lib install "FastLED"
arduino-cli lib install "Adafruit MPU6050"
arduino-cli lib install "Adafruit Unified Sensor"
```

## Additional Resources

### FastLED
- [FastLED Wiki](https://github.com/FastLED/FastLED/wiki)
- [Color Reference](https://github.com/FastLED/FastLED/wiki/Pixel-reference)
- [Basic Usage](https://github.com/FastLED/FastLED/wiki/Basic-usage)

### Adafruit MPU6050
- [Product Guide](https://learn.adafruit.com/mpu6050-6-dof-accelerometer-and-gyro)
- [Arduino Library Docs](https://adafruit.github.io/Adafruit_MPU6050/html/index.html)
- [MPU6050 Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)

### I2C Communication
- [Arduino Wire Library](https://www.arduino.cc/reference/en/language/functions/communication/wire/)
- [I2C Protocol Basics](https://learn.sparkfun.com/tutorials/i2c/all)

## License Information

All libraries used in this project are open-source:
- **FastLED**: MIT License
- **Adafruit MPU6050**: BSD License
- **Adafruit Unified Sensor**: Apache License 2.0

## Updates and Maintenance

Check for library updates regularly:
1. Open Library Manager
2. Look for "Update" button next to installed libraries
3. Review changelog before updating
4. Test after updates to ensure compatibility

---

**Installation Support**: If you encounter issues installing these libraries, check the Arduino forums or each library's GitHub issues page.
