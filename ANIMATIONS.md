# Animation Modes Reference

This guide explains each animation mode and how it responds to different types of motion.

## Motion Inputs

The system detects three types of motion:

| Motion Type | What It Detects | Range | Example |
|-------------|-----------------|-------|---------|
| **Tilt** | Angle from neutral position | 0-90° | Tilting tube up/down or left/right |
| **Rotation** | Speed of spinning | 0-500°/s | Rotating tube around its axis |
| **Shake** | Vibration intensity | 0-2G | Shaking or sudden movements |

All values are normalized to 0-1 for smooth animation control.

---

## Animation Modes

### 1. Rainbow 🌈

**Description**: Classic rainbow color wheel effect

**Motion Response**:
- **Rotation** → Changes rainbow speed (faster rotation = faster rainbow)
- **Tilt > 30%** → Switches to segmented mode (each segment different color)
- **Tilt < 30%** → Continuous rainbow across all LEDs

**Visual Effect**:
- Smooth: Gentle rainbow flow when still
- Dynamic: Rainbow segments when tilted
- Fast: Rapid color changes when spinning

**Best For**: General viewing, demonstrating basic motion control

---

### 2. Sparkle ✨

**Description**: Random twinkling lights like stars

**Motion Response**:
- **Shake** → Controls sparkle density (more shake = more sparkles)
- **Tilt Angle** → Changes color hue (0° = red, 90° = cyan)
- **Still** → Minimal sparse sparkles

**Visual Effect**:
- Calm: Few sparkles, stable color
- Shaken: Dense field of twinkling lights
- Tilted: Color shifts across spectrum

**Best For**: Interactive play, responding to quick movements

---

### 3. Wave 🌊

**Description**: Sine wave patterns flowing through LEDs

**Motion Response**:
- **Tilt (Pitch)** → Controls base color hue
- **Rotation** → Changes wave speed (1-4x multiplier)
- **Tilt Amount** → Adjusts wave width (more tilt = narrower waves)

**Visual Effect**:
- Smooth: Gentle rolling waves
- Dynamic: Fast-moving patterns
- Morphing: Wave characteristics change with angle

**Best For**: Smooth, meditative viewing

---

### 4. Fire 🔥

**Description**: Realistic fire and flame effect

**Motion Response**:
- **Shake** → Increases flame sparking/intensity (0-100% boost)
- **Tilt** → Affects cooling rate (changes flame height)
- **Rotation** → Shifts color hue (makes fire blue/green/purple)

**Visual Effect**:
- Still: Gentle flickering flames
- Shaken: Intense, roaring fire
- Spinning: Colored fire (blue flames, etc.)

**Best For**: Dramatic, intense viewing

---

### 5. Pulse 💓

**Description**: Breathing/pulsing effect

**Motion Response**:
- **Rotation** → Controls pulse speed (1-4x BPM)
- **Tilt Angle** → Changes color hue
- **Shake** → Sets minimum brightness (brighter base)

**Visual Effect**:
- Calm: Slow breathing effect
- Active: Fast pulsing heartbeat
- Varies: Color and brightness change

**Best For**: Rhythmic, hypnotic patterns

---

### 6. Kaleidoscope 🔮 (Default)

**Description**: Symmetrical patterns across three segments creating true kaleidoscope effect

**Motion Response**:
- **Rotation** → Controls pattern animation speed (1-3x)
- **Tilt Angle** → Shifts color palette
- **Shake** → Modulates brightness/intensity
- **Time** → Two sine waves create complex patterns

**Visual Effect**:
- Symmetrical: Each segment mirrors pattern
- Dynamic: Multiple waves create interference patterns
- Colorful: Rich color transitions
- Complex: Most visually interesting mode

**Technical Details**:
- Uses dual sine waves with different periods
- Position-based color mapping
- Segment symmetry maintained
- Perfect for the physical kaleidoscope design

**Best For**: Primary viewing mode, most impressive effect, true kaleidoscope experience

---

## Animation Comparison Chart

| Mode | Complexity | Motion Sensitivity | Color Variation | Best Use Case |
|------|------------|-------------------|-----------------|---------------|
| Rainbow | Low | Medium | High | Testing, demos |
| Sparkle | Medium | Very High | Medium | Interactive play |
| Wave | Medium | High | Medium | Ambient viewing |
| Fire | High | High | Medium | Drama, impact |
| Pulse | Low | Medium | Medium | Rhythmic effects |
| Kaleidoscope | Very High | High | Very High | Primary viewing |

---

## Motion-to-Visual Mapping

### Common Patterns Across Modes

```
TILT (0-90°) usually controls:
├─ Color hue selection
├─ Animation mode switching
└─ Effect intensity

ROTATION (0-500°/s) usually controls:
├─ Animation speed
├─ Pattern movement
└─ Secondary color shifts

SHAKE (0-2G) usually controls:
├─ Effect density/intensity
├─ Brightness modulation
└─ Triggering additional effects
```

### Normalized Value Usage

All motion values are normalized to 0-1:

```cpp
0.0 = Below threshold (no effect)
0.0 - 0.3 = Subtle effects
0.3 - 0.7 = Moderate effects
0.7 - 1.0 = Strong effects
1.0 = Maximum intensity
```

---

## Creating Custom Animations

### Template for Motion-Reactive Animation

```cpp
void Animations::motionCustom(const MotionData& motion, unsigned long time) {
  // Map motion to animation parameters
  float speed = 1.0 + motion.rotationNormalized * 3.0;  // 1x to 4x speed
  uint8_t hue = motion.tiltAngle * 2;                   // 0-180° to 0-255 hue
  uint8_t brightness = 100 + motion.shakeNormalized * 155; // Variable brightness

  // Your animation logic here
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    // Calculate color/pattern for each LED
    // Use time, motion, and position
  }
}
```

### Best Practices

1. **Smooth Transitions**
   - Use floating-point math for smooth interpolation
   - Apply easing functions for natural motion
   - Avoid sudden jumps in color or brightness

2. **Performance**
   - Target 60 FPS for smooth motion tracking
   - Avoid expensive calculations per LED
   - Pre-calculate values when possible

3. **Motion Mapping**
   - Use normalized values (0-1) for predictable behavior
   - Add minimum thresholds to ignore small movements
   - Combine multiple motion types for richer effects

4. **Visual Design**
   - Respect the segment structure for kaleidoscope effect
   - Use symmetry across segments when appropriate
   - Consider the infinity mirror multiplication

---

## Mode Switching

### Manual Mode Switch
Press the button on Pin 2 to cycle through modes:
```
Rainbow → Sparkle → Wave → Fire → Pulse → Kaleidoscope → (repeat)
```

### Auto-Cycle (Optional)
Uncomment this code in `kaleidoscope.ino` to auto-switch every 30 seconds:
```cpp
if (currentTime - lastModeChange >= MODE_CHANGE_INTERVAL) {
  nextMode();
}
```

### Mode Indicators
- Brief white flash when mode changes
- Serial monitor prints current mode name
- Status visible at 115200 baud

---

## Advanced: Combining Modes

You can create hybrid effects by combining animation primitives:

```cpp
// Example: Sparkles over fire
animations.motionFire(motion);
animations.sparkle(CRGB::White, CRGB::Yellow, 0.1);
```

```cpp
// Example: Pulsing rainbow
animations.rainbow(hueOffset, speed);
animations.fadeToBlackBy(pulseAmount);
```

---

## Tuning for Your Hardware

### If animations are too sensitive:
```cpp
// In Config.h
#define MOTION_SMOOTHING 0.3  // Increase smoothing
#define TILT_THRESHOLD 20.0   // Increase threshold
```

### If animations are not responsive enough:
```cpp
// In Config.h
#define MOTION_SMOOTHING 0.1  // Decrease smoothing
#define TILT_THRESHOLD 10.0   // Decrease threshold
```

### If colors are too intense:
```cpp
// In Config.h
#define MAX_BRIGHTNESS 150    // Reduce brightness
```

---

## Debugging Animations

### View Motion Data
Open Serial Monitor (115200 baud) to see:
```
=== Motion Data ===
Tilt: 23.5° (26%)
Rotation: 145°/s (45%)
Shake: 0.8 (40%)
Pitch: 15° Roll: -8°
FPS: 60
```

### Test Individual Components
```cpp
// Test motion only (no animations)
motionProcessor.update();
MotionData m = motionProcessor.getMotionData();
Serial.println(m.tiltAngle);

// Test LEDs only (static pattern)
ledController.rainbow(0, 1.0);
ledController.show();
```

---

**Tip**: Start with Kaleidoscope mode - it's specifically designed for your mirrored tube setup and creates the most impressive kaleidoscope effect!
