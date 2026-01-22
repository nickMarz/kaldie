# Hardware Setup & Troubleshooting Guide

## LED Strip Specifications

### WS2818 LED Strip Details
- **Total LEDs**: 209
- **Protocol**: WS2812B (despite being WS2818 chips)
- **Color Order**: GRB (Green-Red-Blue)
- **Data Pin**: Digital Pin 4 on Arduino Mega
- **Operating Voltage**: 5V
- **Data Rate**: 800kHz

### Segment Configuration
The 209 LEDs are divided into 3 segments for the kaleidoscope effect:
- **Segment 0**: LEDs 0-69 (70 LEDs, forward direction)
- **Segment 1**: LEDs 70-139 (70 LEDs, reversed/folded back)
- **Segment 2**: LEDs 140-208 (69 LEDs, forward direction)

## Power Requirements

### Calculating Power Needs
- **Per LED**: ~60mA at full white brightness
- **209 LEDs at full white**: 209 × 0.06A = 12.54A
- **Power needed**: 12.54A × 5V = 62.7W
- **Recommended PSU**: 5V 10-15A (50-75W) minimum

### Realistic Power Usage
- Most animations use colors, not white (much lower power than white)
- Brightness limited to 50/255 max, 20/255 default (very conservative)
- Typical usage with current settings: 1-3A (5-15W)
- Maximum possible with current brightness: ~2.5A (12.5W)

## Critical Wiring Points

### ⚠️ MOST IMPORTANT: Common Ground
**The #1 cause of LEDs not working is missing common ground!**

```
Power Supply GND ─────┬───── LED Strip GND
                      │
                      └───── Arduino GND
```

Without connecting all grounds together, the data signal will not work!

### Complete Wiring Diagram

```
POWER SUPPLY (5V 10A+)          ARDUINO MEGA            LED STRIP (WS2818)
┌─────────────────┐            ┌──────────┐            ┌─────────────┐
│                 │            │          │            │             │
│      5V (+) ────┼────────────┼──────────┼────────────┤ 5V (Red)    │
│                 │            │          │            │             │
│     GND (-) ────┼────────────┤ GND      │────────────┤ GND (White) │
│                 │            │          │            │             │
└─────────────────┘            │  Pin 4 ──┼────────────┤ DIN (Green) │
                               │          │            │             │
                               └──────────┘            └─────────────┘

MPU-6050 SENSOR
┌─────────────┐
│ VCC ────────┤ 5V (Arduino)
│ GND ────────┤ GND (Arduino)
│ SCL ────────┤ Pin 21 (Arduino)
│ SDA ────────┤ Pin 20 (Arduino)
└─────────────┘
```

## Troubleshooting Guide

### Problem: No LEDs Light Up

1. **Check Power**
   - Is the power supply plugged in and turned on?
   - Measure voltage at LED strip: Should be 5V ±0.25V
   - Check power supply capacity (needs 10A+ for 209 LEDs)

2. **Check Ground Connection** ⭐ MOST COMMON ISSUE
   - Arduino GND MUST connect to LED strip GND
   - Arduino GND MUST connect to Power Supply GND
   - Use multimeter to verify continuity between all grounds

3. **Check Data Connection**
   - Ensure connected to DIN (Data IN), not DOUT
   - Look for arrows on strip showing data direction →
   - Try adding 330-470Ω resistor between Pin 4 and DIN
   - Check for loose/broken connections

4. **Check LED Configuration in Code**
   ```cpp
   // In LEDController.cpp, line 9 should be:
   FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
   ```

### Problem: Wrong Colors Display

If colors are wrong, change the color order in `LEDController.cpp`:

| If you see | When expecting | Change GRB to |
|------------|---------------|---------------|
| Green      | Red           | RGB           |
| Blue       | Red           | BGR           |
| Red        | Green         | GRB (correct) |
| Blue       | Green         | BRG           |
| Red        | Blue          | RBG           |
| Green      | Blue          | GBR           |

### Problem: Only First Few LEDs Work

1. **Power Issue**
   - Voltage drop along strip (measure at middle and end)
   - Inject power at multiple points for long strips
   - Use thicker wires for power (18-20 AWG)

2. **Data Signal Degradation**
   - Add 330-470Ω resistor at data line start
   - Keep data wire short (<2 meters)
   - Use twisted pair or shielded cable for long data runs

### Problem: Flickering or Glitching

1. **Power Supply Issues**
   - Insufficient current capacity
   - Add 1000µF capacitor across power supply terminals
   - Check for loose connections

2. **Data Issues**
   - Ensure solid ground connection
   - Add 0.1µF capacitor between data and ground at strip
   - Reduce brightness in Config.h

### Problem: MPU-6050 Sensor Not Working

1. **Check I2C Connections**
   - SDA to Pin 20 (Arduino Mega)
   - SCL to Pin 21 (Arduino Mega)
   - VCC to 5V, GND to GND

2. **Check Serial Monitor** (115200 baud)
   - Look for "ERROR: Failed to initialize MPU6050!"
   - LEDs will flash red if sensor fails

3. **Try I2C Scanner**
   - Upload I2C scanner sketch to find sensor address
   - Should be 0x68 or 0x69

## Testing Procedures

### 1. Basic LED Test (No Sensor Needed)
Upload the test sketch from `/kaleidoscope_test/ws2818_test/` to verify:
- LED strip works
- Color order is correct
- All 209 LEDs are addressable

### 2. Power Test
```cpp
// In test sketch, set all LEDs to dim white:
fill_solid(leds, NUM_LEDS, CRGB(30, 30, 30));
```
If this works, power supply is adequate for normal use.

### 3. Segment Test
Verify segments are mapped correctly:
- Segment 0 (0-69): Should light first section
- Segment 1 (70-139): Should light middle (reversed)
- Segment 2 (140-208): Should light last section

## LED Strip Variations

### Different LED Types
| LED Type | Protocol | Typical Color Order | Notes |
|----------|----------|-------------------|-------|
| WS2812B  | WS2812B  | GRB              | Most common |
| WS2818   | WS2812B  | GRB              | Your strip |
| WS2811   | WS2811   | RGB              | Older, 12V option |
| WS2813   | WS2812B  | GRB              | Backup data line |
| SK6812   | WS2812B  | GRB              | RGBW variant available |

## Best Practices

1. **Always connect grounds first** when wiring
2. **Power up in sequence**: Power supply → Arduino → Upload code
3. **Start with low brightness** for testing (50-100/255)
4. **Use quality connectors** - JST-SM or similar
5. **Keep data wire away from power wires** to reduce noise
6. **Test with small sections first** before full strip
7. **Document your working configuration** for future reference

## Configuration Quick Reference

Current working configuration:
```cpp
// Config.h
#define LED_PIN 4              // Data pin
#define NUM_LEDS 209          // Total LEDs
#define LEDS_PER_SEGMENT 70   // LEDs per segment
#define NUM_SEGMENTS 3        // Number of segments
#define MAX_BRIGHTNESS 50     // Maximum brightness (reduced for safety)
#define DEFAULT_BRIGHTNESS 20 // Default brightness

// Performance Settings
#define TARGET_FPS 120        // Animation frame rate
#define MPU_UPDATE_RATE 100   // Motion sensor Hz
#define MOTION_SMOOTHING 0.15 // Response smoothing

// Mode Settings
#define AUTO_CYCLE_MODES true  // Auto-cycle animations
#define MODE_DURATION_MS 20000 // 20 seconds per mode
#define RANDOM_START_MODE true // Random mode on startup

// LEDController.cpp
FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
```

## Safety Notes

⚠️ **Fire Hazard**: Never run LEDs at full brightness without proper power supply
⚠️ **Eye Safety**: Bright LEDs can cause eye strain - avoid direct viewing
⚠️ **Heat**: LEDs and power supply will get warm - ensure ventilation
⚠️ **Electrical**: Always disconnect power before modifying wiring

---

Last Updated: 2024
Tested Configuration: WS2818 strip, 209 LEDs, Arduino Mega 2560, 5V 10A PSU