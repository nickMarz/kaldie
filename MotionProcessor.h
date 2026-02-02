#ifndef MOTION_PROCESSOR_H
#define MOTION_PROCESSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Config.h"

struct MotionData {
  // Raw sensor data
  float accelX, accelY, accelZ;  // Acceleration in m/s^2
  float gyroX, gyroY, gyroZ;     // Rotation in rad/s
  float temp;                     // Temperature in C

  // Processed orientation data
  float pitch;                    // Tilt forward/backward (degrees)
  float roll;                     // Tilt left/right (degrees)

  // Motion characteristics
  float tiltAngle;               // Total tilt from neutral (degrees)
  float rotationSpeed;           // Total rotation speed (deg/s)
  float shakeIntensity;          // Shake/vibration intensity (0-1)

  // Normalized values for animation (0-1)
  float tiltNormalized;          // 0 = neutral, 1 = max tilt
  float rotationNormalized;      // 0 = still, 1 = fast rotation (or virtual)
  float shakeNormalized;         // 0 = still, 1 = intense shake

  // Pan (horizontal rotation) support
  float pan;                      // Pan angle (yaw) in degrees
  float panNormalized;           // 0-1 normalized pan

  // Virtual rotation from encoder
  float virtualRotation;         // 0-100 from encoder input
  bool useVirtualRotation;       // Flag to use virtual instead of physical
};

class MotionProcessor {
public:
  MotionProcessor();

  bool begin();
  bool update();
  void calibrate();

  MotionData getMotionData() const { return motionData; }
  bool isCalibrated() const { return calibrated; }

  // Getters for specific motion characteristics
  float getTiltAngle() const { return motionData.tiltAngle; }
  float getRotationSpeed() const { return motionData.rotationSpeed; }
  float getShakeIntensity() const { return motionData.shakeIntensity; }

  // Normalized getters (0-1) for easy animation mapping
  float getTiltNormalized() const { return motionData.tiltNormalized; }
  float getRotationNormalized() const { return motionData.rotationNormalized; }
  float getShakeNormalized() const { return motionData.shakeNormalized; }

  // Virtual rotation control
  void setVirtualRotation(float value) {
    motionData.virtualRotation = value;
    motionData.useVirtualRotation = true;
  }
  void clearVirtualRotation() {
    motionData.useVirtualRotation = false;
  }

private:
  Adafruit_MPU6050 mpu;
  MotionData motionData;

  // Calibration offsets
  float accelXOffset, accelYOffset, accelZOffset;
  bool calibrated;

  // Smoothing
  float prevTilt, prevRotation, prevShake;

  // Helper functions
  void calculateOrientation();
  void calculateMotionCharacteristics();
  void applySmoothing();
  float mapToNormalized(float value, float threshold, float maxValue);
};

#endif
