# Sound-Reactive Add-On Guide

## 🎵 Recommended: MSGEQ7 Spectrum Analyzer Module

### Why MSGEQ7 is Perfect for Kaleidoscope

- **7 frequency bands** for detailed music visualization
- **Minimal pin usage** (only 3 pins needed)
- **No complex FFT** calculations required
- **Works alongside** encoders and motion sensors
- **Real-time** frequency analysis

### Hardware Options

1. **SparkFun Spectrum Shield** (~$25)
   - Plug-and-play Arduino shield
   - Includes two MSGEQ7 chips (stereo)
   - Has 3.5mm input jacks

2. **Generic MSGEQ7 Module** (~$10-15)
   - Smaller footprint
   - Single channel (mono)
   - Requires external audio input

3. **DIY MSGEQ7 Circuit** (~$5-8)
   - Build from scratch
   - Most customizable
   - Requires more work

## 🔌 Wiring to Arduino Mega

### MSGEQ7 Module Connections

```
     MSGEQ7 MODULE                 ARDUINO MEGA
    ┌─────────────┐
    │ OUT → → → → │────────────→ A0 (Analog Read)
    │ STROBE → → →│────────────→ Pin 8 (Digital)
    │ RESET → → → │────────────→ Pin 9 (Digital)
    │ VCC → → → → │────────────→ 5V
    │ GND → → → → │────────────→ GND
    │             │
    │ AUDIO IN    │←───── 3.5mm Jack or Microphone
    └─────────────┘
```

### Complete Pin Usage (with all components)

```
Arduino Mega Pin Assignments:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━
COMPONENT          PINS USED
──────────────────────────────
Encoder 1:         3, 5, 7
Encoder 2:         18, 19, 17
MPU-6050:          20, 21 (I2C)
LED Strip:         4
MSGEQ7:           A0, 8, 9    ← NEW!
──────────────────────────────
Still Available:   Many pins!
```

## 💻 Code Integration

### Add to Config.h

```cpp
// Sound Reactive Configuration
#define USE_SOUND true
#define MSGEQ7_OUT A0      // Analog input pin
#define MSGEQ7_STROBE 8    // Strobe pin
#define MSGEQ7_RESET 9     // Reset pin

// Frequency band indices
#define BAND_63HZ 0     // Sub-bass
#define BAND_160HZ 1    // Bass
#define BAND_400HZ 2    // Low-mid
#define BAND_1KHZ 3     // Mid
#define BAND_2_5KHZ 4   // High-mid
#define BAND_6_25KHZ 5  // High
#define BAND_16KHZ 6    // Brilliance

// Sound sensitivity
#define SOUND_THRESHOLD 50   // Minimum level to react
#define SOUND_GAIN 2.0       // Amplification factor
#define BASS_BOOST 1.5       // Extra boost for bass frequencies
```

### SoundProcessor Class (Conceptual)

```cpp
class SoundProcessor {
public:
  void begin() {
    pinMode(MSGEQ7_RESET, OUTPUT);
    pinMode(MSGEQ7_STROBE, OUTPUT);
    digitalWrite(MSGEQ7_RESET, LOW);
    digitalWrite(MSGEQ7_STROBE, HIGH);
  }

  void update() {
    // Reset the MSGEQ7
    digitalWrite(MSGEQ7_RESET, HIGH);
    digitalWrite(MSGEQ7_RESET, LOW);

    // Read all 7 bands
    for (int i = 0; i < 7; i++) {
      digitalWrite(MSGEQ7_STROBE, LOW);
      delayMicroseconds(30);

      frequencies[i] = analogRead(MSGEQ7_OUT);

      digitalWrite(MSGEQ7_STROBE, HIGH);
    }

    // Calculate overall volume
    volume = 0;
    for (int i = 0; i < 7; i++) {
      volume += frequencies[i];
    }
    volume /= 7;

    // Detect beat (bass)
    beatDetected = (frequencies[BAND_63HZ] + frequencies[BAND_160HZ]) > beatThreshold;
  }

  int frequencies[7];      // Raw frequency values (0-1023)
  float volume;           // Average volume
  bool beatDetected;      // Beat detection flag

private:
  int beatThreshold = 600;
};
```

## 🎨 Sound-Reactive Animation Ideas

### 1. Frequency Spectrum Mode
```cpp
void soundSpectrum() {
  for (int i = 0; i < 7; i++) {
    // Map each frequency to a segment
    int segmentSize = NUM_LEDS / 7;
    int level = map(frequencies[i], 0, 1023, 0, segmentSize);

    // Different color for each frequency band
    CRGB color = CHSV(i * 36, 255, 255);

    // Fill LEDs based on frequency level
    for (int j = 0; j < level; j++) {
      leds[i * segmentSize + j] = color;
    }
  }
}
```

### 2. Bass Pulse Mode
```cpp
void bassPulse(const MotionData& motion) {
  // Combine bass with motion
  float bassLevel = (frequencies[BAND_63HZ] + frequencies[BAND_160HZ]) / 2048.0;
  float intensity = bassLevel * (1.0 + motion.tiltNormalized);

  // Pulse all LEDs with bass
  CRGB color = CHSV(motion.pan * 2, 255, 255 * intensity);
  fill_solid(leds, NUM_LEDS, color);
}
```

### 3. Beat-Triggered Mode Changes
```cpp
void checkBeatModeChange() {
  if (beatDetected && millis() - lastBeatTime > 500) {
    nextMode();  // Change mode on strong beat
    lastBeatTime = millis();
  }
}
```

### 4. Frequency-Reactive Kaleidoscope
```cpp
void soundKaleidoscope() {
  // Each segment responds to different frequency ranges
  for (int seg = 0; seg < 3; seg++) {
    int freq1 = frequencies[seg * 2];
    int freq2 = frequencies[seg * 2 + 1];

    // Create patterns based on frequency
    uint8_t hue = map(freq1, 0, 1023, 0, 255);
    uint8_t brightness = map(freq2, 0, 1023, 50, 255);

    // Apply to segment with symmetry
    fillSegmentSymmetric(seg, CHSV(hue, 255, brightness));
  }
}
```

## 🎛️ Control Mapping

### With Encoders + Sound

- **Encoder 1**: Mode selection (including sound modes)
- **Encoder 2**: Sound sensitivity adjustment
- **Tilt**: Animation speed
- **Pan**: Color shift
- **Sound**: Brightness/pattern intensity

### New Animation Modes

Add these sound-reactive modes:
1. `MODE_SPECTRUM` - 7-band spectrum analyzer
2. `MODE_BASS_PULSE` - Pulses with bass
3. `MODE_SOUND_SPARKLE` - Sparkles on beats
4. `MODE_FREQ_WAVE` - Waves colored by frequency
5. `MODE_SOUND_FIRE` - Fire intensity from volume
6. `MODE_BEAT_STROBE` - Strobes on beat detection

## 🔊 Audio Input Options

### Option A: Line In (Best for music)
- 3.5mm jack from phone/computer
- Needs resistor divider (audio is AC, Arduino needs 0-5V DC)
- Clearest signal, no background noise

### Option B: Microphone (Best for ambient)
- Picks up room sound
- Good for parties/events
- May pick up unwanted noise

### Simple Input Circuit
```
Audio Source → 10µF Cap → 10kΩ ─┬─ 10kΩ → GND
                                 │
                                 └→ To MSGEQ7 Audio In

This creates 2.5V DC bias for the AC audio signal
```

## 🎚️ Alternative: MAX9814 for Simplicity

If MSGEQ7 seems complex, use MAX9814 for simple beat detection:

```cpp
void simpleBeatDetect() {
  int soundLevel = analogRead(A1);  // Read MAX9814

  // Simple peak detection
  if (soundLevel > threshold && millis() - lastBeat > 100) {
    triggerBeatAnimation();
    lastBeat = millis();
  }

  // Map volume to brightness
  int brightness = map(soundLevel, 0, 1023, 50, 255);
  FastLED.setBrightness(brightness);
}
```

## 🛒 Shopping List

### For MSGEQ7 Setup
- MSGEQ7 module or shield ($10-25)
- 3.5mm audio jack ($2)
- 10µF capacitor ($0.50)
- 2x 10kΩ resistors ($0.50)
- Audio cable ($5)

### For Simple Sound
- MAX9814 module ($8-12)
- That's it!

## 📈 Performance Impact

- MSGEQ7 reading: ~1ms per update
- Minimal RAM usage: ~100 bytes
- CPU impact: <5%
- Can run at 60+ FPS easily

## 🚀 Next Steps

1. Order MSGEQ7 module or MAX9814
2. Test with simple spectrum reading
3. Integrate with existing animations
4. Create dedicated sound modes
5. Add sound sensitivity to encoder 2

The sound reactivity will make your kaleidoscope absolutely mesmerizing at parties and events!