# Encoder 2 Button - Quick Start Guide

## 🎯 Recommended: Color Mode Toggle

The simplest and most impactful feature for the Encoder 2 button is **Color Mode switching**.

## 📦 What You Already Have

✅ `ColorModes.h` - Complete color mode manager class
✅ `EncoderHandler.h` - Encoder reading infrastructure
✅ Working animations ready for color transformation

## 🚀 5-Minute Implementation

### 1. Include ColorModes.h in kaleidoscope.ino

```cpp
#include "ColorModes.h"
ColorModeManager colorModes;  // Add as global variable
```

### 2. Add Button Check to EncoderHandler

In `EncoderHandler.h`, add to the `update()` method:
```cpp
// Check encoder 2 button
if (digitalRead(ENCODER_2_SW) == LOW) {
  if (!encoder2ButtonPressed) {
    encoder2ButtonPressed = true;
    encoder2ButtonPressTime = millis();
  }
}
```

### 3. Handle Button in Main Loop

In `kaleidoscope.ino` loop():
```cpp
if (encoderHandler.getEncoder2ButtonPressed()) {
  colorModes.nextMode();
}
```

### 4. Apply Colors in ONE Animation First

Start with `motionRainbow()` in `Animations.cpp`:

```cpp
// OLD CODE:
CRGB color = CHSV(hue, 255, 255);

// NEW CODE:
CRGB color = colorModes.applyColorMode(hue, 255, 255);
```

That's it! Test it with Rainbow mode first.

## 🎨 Available Color Modes

Press the button to cycle through:

1. **Full Spectrum** - Normal rainbow
2. **Warm** - Sunset colors (red/orange/yellow)
3. **Cool** - Ocean colors (blue/green/purple)
4. **Fire** - Realistic fire palette
5. **Ocean** - Deep sea blues
6. **Party** - Random vibrant colors
7. **Monochrome** - Single color variations

## 📊 Button Function Comparison

| Feature | Impact | Difficulty | Time |
|---------|--------|-----------|------|
| **Color Modes** | ⭐⭐⭐⭐⭐ | Easy | 30min |
| Brightness Presets | ⭐⭐⭐ | Easy | 15min |
| Speed Control | ⭐⭐⭐ | Easy | 15min |
| Freeze Frame | ⭐⭐ | Easy | 20min |
| Symmetry Modes | ⭐⭐⭐⭐ | Medium | 1hr |
| Effect Layers | ⭐⭐⭐ | Hard | 2hr |

## 💡 Testing Order

1. **Test with Rainbow first** - Easiest to see color changes
2. **Try with Fire mode** - See dramatic palette shifts
3. **Test with Sparkle** - Random colors become themed
4. **Apply to Kaleidoscope** - Full symmetrical beauty

## 🔧 Troubleshooting

**Button not responding?**
- Check pin 17 connection (ENCODER_2_SW)
- Verify internal pullup: `pinMode(ENCODER_2_SW, INPUT_PULLUP)`

**Colors look wrong?**
- Make sure you're calling `colorModes.applyColorMode()`
- Check that hue values are 0-255

**Want to customize palettes?**
- Edit color ranges in `ColorModes.h`
- Adjust the hue mapping values

## 📈 Next Steps

Once Color Modes work:
1. Add visual feedback (flash on change)
2. Apply to all 6 animations
3. Consider adding brightness presets as hold function
4. Add status LED indicator

## 🎮 Pro Tips

- **Quick Demo**: Party mode + Sparkle animation = instant crowd pleaser
- **Elegant**: Monochrome + Wave = sophisticated display
- **Energy**: Warm colors + Fire = intense experience
- **Calm**: Cool colors + Pulse = meditation mode

## 📄 Related Files

- `BUTTON_FEATURES_ROADMAP.md` - Complete feature list
- `ENCODER2_IMPLEMENTATION.cpp` - Full code examples
- `ColorModes.h` - Color mode manager class
- `ENCODER2_BUTTON_IDEAS.md` - Original brainstorm

---

**Start with Color Modes - it's the biggest bang for your buck!**