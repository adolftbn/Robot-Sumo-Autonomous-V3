#include <Arduino.h>
#include "config.h"
#include "motor.h"
#include "sensors.h"
#include "telemetry.h"

static unsigned long debugTimer = 0;
void initTelemetry() { if (DEBUG_TELEMETRY) Serial.println("TEL,t_ms,state,line,dist_left_cm,dist_right_cm,rpm_left,pwm,motor,loop_us"); }
void sendTelemetry(RobotState state, const LineSensors &line, unsigned long loopTimeUs) {
  unsigned long now = millis(); if (!DEBUG_TELEMETRY || now - debugTimer < DEBUG_INTERVAL_MS) return;
  uint8_t lineMask = (line.frontLeft << 0) | (line.frontRight << 1) | (line.left << 2) | (line.right << 3) | (line.back << 4);
  Serial.print("TEL,"); Serial.print(now); Serial.print(','); Serial.print(state); Serial.print(','); Serial.print(lineMask); Serial.print(',');
  Serial.print(leftDistanceCm()); Serial.print(','); Serial.print(rightDistanceCm()); Serial.print(','); Serial.print(leftWheelRpm(), 2); Serial.print(',');
  Serial.print(currentMotorPwm()); Serial.print(','); Serial.print(currentMotorCommand()); Serial.print(','); Serial.println(loopTimeUs);
  debugTimer = now;
}
void sendScenarioAEvent(const char *event, unsigned long tUs, float rpm, unsigned long responseUs) {
  Serial.print("A_EVENT,"); Serial.print(SCENARIO_A_TRIAL_ID); Serial.print(','); Serial.print(event); Serial.print(',');
  Serial.print(tUs); Serial.print(','); Serial.print(SCENARIO_A_TARGET_PWM); Serial.print(','); Serial.print(rpm, 2); Serial.print(','); Serial.println(responseUs);
}
void sendScenarioASample(unsigned long tUs, float rpm, unsigned long loopTimeUs) {
  Serial.print("A_SAMPLE,"); Serial.print(SCENARIO_A_TRIAL_ID); Serial.print(','); Serial.print(tUs); Serial.print(',');
  Serial.print(SCENARIO_A_TARGET_PWM); Serial.print(','); Serial.print(rpm, 2); Serial.print(','); Serial.print(currentMotorCommand()); Serial.print(',');
  Serial.print(currentMotorPwm()); Serial.print(','); Serial.println(loopTimeUs);
}
