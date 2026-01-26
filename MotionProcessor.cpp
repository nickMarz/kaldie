#include "MotionProcessor.h"
#include <Arduino.h>

MotionProcessor::MotionProcessor()
  : calibrated(false),
    accelXOffset(0), accelYOffset(0), accelZOffset(0),
    prevTilt(0), prevRotation(0), prevShake(0) {
}

bool MotionProcessor::begin() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    return false;
  }

  Serial.println("MPU6050 Found!");

  // Set accelerometer range to ±8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // Set gyro range to ±500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // Set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
  return true;
}

void MotionProcessor::calibrate() {
  Serial.println("Calibrating MPU6050...");
  Serial.println("Keep device still!");

  float sumX = 0, sumY = 0, sumZ = 0;

  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    sumX += a.acceleration.x;
    sumY += a.acceleration.y;
    sumZ += a.acceleration.z;

    delay(10);
  }

  accelXOffset = sumX / CALIBRATION_SAMPLES;
  accelYOffset = sumY / CALIBRATION_SAMPLES;
  accelZOffset = (sumZ / CALIBRATION_SAMPLES) - 9.81; // Subtract gravity

  calibrated = true;
  Serial.println("Calibration complete!");
  Serial.print("Offsets: X=");
  Serial.print(accelXOffset);
  Serial.print(" Y=");
  Serial.print(accelYOffset);
  Serial.print(" Z=");
  Serial.println(accelZOffset);
}

bool MotionProcessor::update() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Store raw data with calibration offsets
  motionData.accelX = a.acceleration.x - accelXOffset;
  motionData.accelY = a.acceleration.y - accelYOffset;
  motionData.accelZ = a.acceleration.z - accelZOffset;

  motionData.gyroX = g.gyro.x;
  motionData.gyroY = g.gyro.y;
  motionData.gyroZ = g.gyro.z;

  motionData.temp = temp.temperature;

  // Calculate orientation and motion characteristics
  calculateOrientation();
  calculateMotionCharacteristics();
  applySmoothing();

  return true;
}

void MotionProcessor::calculateOrientation() {
  // Calculate pitch and roll from accelerometer
  // Pitch (rotation around Y axis)
  motionData.pitch = atan2(motionData.accelY,
                           sqrt(motionData.accelX * motionData.accelX +
                                motionData.accelZ * motionData.accelZ)) * 180.0 / PI;

  // Roll (rotation around X axis)
  motionData.roll = atan2(-motionData.accelX, motionData.accelZ) * 180.0 / PI;

  // Calculate total tilt angle
  motionData.tiltAngle = sqrt(motionData.pitch * motionData.pitch +
                              motionData.roll * motionData.roll);
}

void MotionProcessor::calculateMotionCharacteristics() {
  // Convert gyro from rad/s to deg/s and calculate total rotation speed
  float gyroXdeg = motionData.gyroX * 180.0 / PI;
  float gyroYdeg = motionData.gyroY * 180.0 / PI;
  float gyroZdeg = motionData.gyroZ * 180.0 / PI;

  motionData.rotationSpeed = sqrt(gyroXdeg * gyroXdeg +
                                  gyroYdeg * gyroYdeg +
                                  gyroZdeg * gyroZdeg);

  // Calculate shake intensity from total acceleration magnitude deviation from 1G
  float totalAccel = sqrt(motionData.accelX * motionData.accelX +
                         motionData.accelY * motionData.accelY +
                         motionData.accelZ * motionData.accelZ);
  motionData.shakeIntensity = abs(totalAccel - 9.81) / 9.81;

  // Normalize values for animation (0-1)
  motionData.tiltNormalized = mapToNormalized(motionData.tiltAngle, TILT_THRESHOLD, 90.0);

  // Use virtual rotation if enabled, otherwise use physical rotation
  if (motionData.useVirtualRotation) {
    motionData.rotationNormalized = motionData.virtualRotation / VIRTUAL_ROTATION_MAX;
  } else {
    motionData.rotationNormalized = mapToNormalized(motionData.rotationSpeed, ROTATION_THRESHOLD, 500.0);
  }

  motionData.shakeNormalized = mapToNormalized(motionData.shakeIntensity, SHAKE_THRESHOLD / 10.0, SHAKE_THRESHOLD);

  // Calculate pan normalized (using roll as pan for now)
  motionData.pan = motionData.roll;  // Use roll as pan indicator
  motionData.panNormalized = mapToNormalized(abs(motionData.pan), 15.0, 90.0);
}

void MotionProcessor::applySmoothing() {
  // Apply exponential moving average for smoothing
  float alpha = MOTION_SMOOTHING;

  motionData.tiltAngle = alpha * motionData.tiltAngle + (1 - alpha) * prevTilt;
  motionData.rotationSpeed = alpha * motionData.rotationSpeed + (1 - alpha) * prevRotation;
  motionData.shakeIntensity = alpha * motionData.shakeIntensity + (1 - alpha) * prevShake;

  prevTilt = motionData.tiltAngle;
  prevRotation = motionData.rotationSpeed;
  prevShake = motionData.shakeIntensity;
}

float MotionProcessor::mapToNormalized(float value, float threshold, float maxValue) {
  if (value < threshold) return 0.0;
  float normalized = (value - threshold) / (maxValue - threshold);
  return constrain(normalized, 0.0, 1.0);
}
