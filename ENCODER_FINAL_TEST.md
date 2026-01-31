# Encoder Final Test Checklist

## Pre-Flight Check
- [ ] LEDs connected to pin 4
- [ ] MPU-6050 connected (I2C pins 20, 21)
- [ ] Encoder 1 on pins 3, 5, 7
- [ ] Encoder 2 on pins 18, 19, 17
- [ ] Power supply connected

## Upload Main Sketch
1. Open `kaleidoscope.ino`
2. Verify these settings in `Config.h`:
   - `NUM_LEDS 209`
   - `LED_PIN 4`
   - `USE_ENCODERS true`
   - `VIRTUAL_ROTATION_STEP 10`
3. Upload to Arduino Mega

## Test Sequence

### 1. Initial Power On
- [ ] Random mode starts (should not be mode 0)
- [ ] LEDs light up with animation
- [ ] Serial monitor shows "Encoders initialized"

### 2. Encoder 1 - Mode Selection
- [ ] Rotate CW: Mode increases (0→1→2→3→4→5→0)
- [ ] Rotate CCW: Mode decreases (5→4→3→2→1→0→5)
- [ ] Press button: Resets to Kaleidoscope mode
- [ ] Each mode shows different animation

### 3. Encoder 2 - Intensity Control
- [ ] Rotate CW: Intensity increases (0→100%)
- [ ] Rotate CCW: Intensity decreases (100→0%)
- [ ] Fast spin: Large jumps (acceleration working)
- [ ] Slow turn: Fine control
- [ ] Effects vary by mode:
  - Rainbow: Color rotation speed
  - Sparkle: Sparkle density
  - Wave: Wave speed
  - Fire: Flame intensity
  - Pulse: Pulse rate
  - Kaleidoscope: Pattern complexity

### 4. Motion Integration
- [ ] Tilt tube: Animations respond to tilt
- [ ] Pan tube: Colors/patterns shift
- [ ] Encoder 2 provides "virtual rotation" axis

### 5. Auto-Cycle Test
- [ ] Don't touch encoders for 20 seconds
- [ ] Mode should auto-advance
- [ ] Touch encoder: Auto-cycle stops

## Troubleshooting

### LEDs don't light:
- Check power supply
- Verify pin 4 connection
- Check NUM_LEDS setting (209)

### Encoders not responding:
- Check Serial Monitor for "Encoders initialized"
- Run `encoder_test.ino` to verify wiring
- Check interrupt pins (3 and 18)

### Direction issues:
- Encoder only goes one way: Check DT pin connection
- Values jump erratically: Increase ENCODER_DEBOUNCE_MS

### Motion sensor issues:
- Run calibration (hold still on startup)
- Check I2C connections (pins 20, 21)

## Success Indicators
✅ All 6 modes accessible via Encoder 1
✅ Smooth intensity control via Encoder 2
✅ Motion affects animations
✅ Acceleration makes navigation fast
✅ System responsive and stable

## Next Steps
Once everything works:
1. Implement Color Modes (Encoder 2 button)
2. Add brightness presets
3. Consider sound reactivity
4. Create custom animations