# Encoder 2 Button Function Ideas

## Current Implementation Target: Color Mode Toggle

The button on Encoder 2 cycles through different color palettes, giving instant visual variety without changing the animation pattern.

## Color Modes Available

1. **Full Spectrum** - Normal rainbow colors
2. **Warm** - Reds, oranges, yellows (sunset/fire vibes)
3. **Cool** - Blues, greens, purples (underwater/ice feel)
4. **Fire** - Realistic fire colors
5. **Ocean** - Deep blues to turquoise
6. **Party** - Random vibrant colors
7. **Monochrome** - Single color with brightness variations

## How It Works

```
Encoder 2 Rotation: Controls effect intensity (0-100%)
Encoder 2 Button: Cycles color modes
```

This creates a 2D control:
- X-axis (rotation): How intense/fast the effect is
- Y-axis (button): What colors are used

## Integration Example

```cpp
// In your animation
void motionRainbow(const MotionData& motion, unsigned long time) {
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t hue = i * 256 / NUM_LEDS + offset;

    // Apply color mode transformation
    CRGB color = colorModes.applyColorMode(hue, 255, 255);

    leds[i] = color;
  }
}

// In main loop - handle button press
if (encoderHandler.getEncoder2Button()) {
  colorModes.nextMode();
  // Brief flash to show new mode
}
```

## Alternative Ideas (For Future)

### Quick Implementation Options

1. **Brightness Presets**
   - Cycle: Dim → Normal → Bright → Demo
   - One button to rule lighting conditions

2. **Speed Multiplier**
   - 0.25x → 0.5x → 1x → 2x → 4x
   - Slow motion to hyperspeed

3. **Segment Mode**
   - All Independent → Mirror → Clone → Reverse
   - Changes how the 3 segments relate

### Advanced Features

4. **Effect Layers**
   ```cpp
   // Add/remove effects on top
   - Base animation
   + Sparkles (toggle)
   + Fade trails (toggle)
   + Blur effect (toggle)
   ```

5. **Freeze Frame**
   - Press to pause/unpause
   - Hold to capture and loop 1 second

6. **Strobe Mode**
   - Off → Slow → Medium → Fast → Music Sync

7. **Direction Control**
   - Forward → Reverse → Bounce → Random

### Interactive Features

8. **Recording Mode**
   - Press to start recording motion
   - Press again to loop recorded motion
   - Great for demos

9. **Preset Selector**
   - Save current settings to preset
   - Cycle through saved presets

10. **Party Mode**
    - Activates special effects
    - Random mode changes
    - Maximum sparkle

## Recommended Implementation Order

1. **Color Mode** (ColorModes.h) - Biggest visual impact ✓
2. **Brightness Presets** - Practical for different environments
3. **Segment Modes** - Changes the kaleidoscope symmetry
4. **Effect Layers** - Adds variety to existing animations
5. **Freeze/Record** - Great for demonstrations

## Code Structure

```cpp
// In EncoderHandler
bool getEncoder2ButtonPressed() {
  // Returns true once per press
}

bool getEncoder2ButtonHeld() {
  // Returns true if held > 1 second
}

// In main loop
if (encoderHandler.getEncoder2ButtonPressed()) {
  handleEncoder2Press();
}

if (encoderHandler.getEncoder2ButtonHeld()) {
  handleEncoder2Hold();  // Different function for hold
}
```

## User Experience

**Single Press Actions** (instant feedback):
- Color mode change
- Brightness preset
- Speed change
- Effect toggle

**Hold Actions** (1+ second):
- Enter/exit special mode
- Save preset
- Reset to defaults
- Activate demo sequence

**Double-Click Actions** (advanced):
- Switch between encoder functions
- Toggle auto-mode
- Emergency reset

## Visual Feedback

Always provide immediate visual feedback:
- Flash the new color mode briefly
- Pulse brightness for brightness change
- Sparkle effect for effect toggle
- Smooth transition for speed changes

This makes the button feel responsive and helps users understand what changed.