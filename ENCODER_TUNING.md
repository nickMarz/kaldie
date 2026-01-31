# Encoder Tuning Guide

## Current Issue: Too Many Rotations Needed

Your KY-040 encoders have 20 detents (clicks) per full rotation. To reach 100 with default settings would take 5 full rotations. Here's how to fix this:

## Quick Fixes

### Option 1: Increase Step Size (Easiest)
In `Config.h`, change:
```cpp
#define VIRTUAL_ROTATION_STEP 5   // Current value
```
To:
```cpp
#define VIRTUAL_ROTATION_STEP 10  // Reach 100 in 10 clicks (½ rotation)
// OR
#define VIRTUAL_ROTATION_STEP 20  // Reach 100 in 5 clicks (¼ rotation)
```

### Option 2: Enhanced Acceleration
The acceleration feature is already enabled. With fast spinning:
- Slow rotation: Step by 1×STEP (fine control)
- Medium speed: Step by 2-5×STEP
- Fast spin: Step by 10×STEP (quick navigation)

### Option 3: Different Scales for Different Encoders

For **Encoder 1** (Mode Selection - only 6 modes):
- Already wraps 0-5, perfect as-is
- One click = one mode

For **Encoder 2** (Intensity 0-100):
```cpp
// In EncoderHandler.cpp, modify handleEncoder2():
// Add larger steps based on speed
if (timeDiff < 50) {
  encoder2Speed = 20;  // Super fast = jump by 20
} else if (timeDiff < 100) {
  encoder2Speed = 10;  // Fast = jump by 10
} else if (timeDiff < 200) {
  encoder2Speed = 5;   // Medium = jump by 5
} else {
  encoder2Speed = 1;   // Slow = fine control
}
```

## Recommended Settings

### For Your Use Case (0-100 intensity control):

**Best Balance:**
```cpp
#define VIRTUAL_ROTATION_STEP 10  // Base step of 10
// With acceleration:
// - Slow turn: 10 per click (10 clicks to 100)
// - Fast spin: 50-100 per click (2 clicks to 100)
```

**For Fine Control:**
```cpp
#define VIRTUAL_ROTATION_STEP 5   // Current setting
// Good if you need precise adjustment
// Use acceleration (spin fast) to jump quickly
```

**For Quick Changes:**
```cpp
#define VIRTUAL_ROTATION_STEP 20  // Big jumps
// Reach any value in ¼ rotation
// Less precision but very fast
```

## Testing Your Settings

1. Upload the `encoder_test_accelerated.ino`
2. Try different rotation speeds
3. Adjust `VIRTUAL_ROTATION_STEP` in Config.h
4. Re-upload and test again

## Pro Tips

### Button Shortcuts
Consider using the buttons for quick jumps:
```cpp
// Encoder 2 button could jump to preset values
if (encoder2ButtonPressed) {
  // Cycle through: 0% → 25% → 50% → 75% → 100%
  static int presets[] = {0, 25, 50, 75, 100};
  static int presetIndex = 2; // Start at 50%
  presetIndex = (presetIndex + 1) % 5;
  encoder2Value = presets[presetIndex];
}
```

### Different Modes, Different Sensitivity
```cpp
// In main loop, adjust sensitivity per mode
switch(currentMode) {
  case MODE_SPARKLE:
    // Sparkle needs fine control (0-100 for density)
    virtualRotationStep = 5;
    break;
  case MODE_FIRE:
    // Fire only needs rough control
    virtualRotationStep = 20;
    break;
}
```

## Common Encoder Models

| Encoder Type | Detents/Revolution | Clicks to 100 (step=5) |
|--------------|-------------------|------------------------|
| KY-040 | 20 | 20 clicks (1 rotation) |
| EC11 | 24 | 20 clicks (⅚ rotation) |
| PEC11R | 24 | 20 clicks (⅚ rotation) |
| Bourns PEC16 | 16 | 20 clicks (1¼ rotations) |

## Final Recommendation

For immediate improvement, change in `Config.h`:
```cpp
#define VIRTUAL_ROTATION_STEP 10  // Was 5
```

This gives you:
- **10 clicks** (half rotation) to go 0→100
- **Fine control** with slow rotation (step by 10)
- **Quick jumps** with fast rotation (step by 50-100)
- **Good balance** between speed and precision

Upload and test - you should find it much more responsive!