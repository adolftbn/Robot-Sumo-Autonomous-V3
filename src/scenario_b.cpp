#include <Arduino.h>
#include "config.h"
#include "motor.h"
#include "scenario_b.h"
#include "sensors.h"

namespace {
enum ScenarioBPhase { B_DISABLED, B_RUN_FORWARD, B_BRAKING, B_BACKING, B_TURNING, B_RETURNING };
ScenarioBPhase phase = B_DISABLED;
unsigned long lineDetectedUs = 0;
unsigned long backingStartMs = 0;
unsigned long turnStartUs = 0;
unsigned long lastSampleMs = 0;
Direction turnDirection = DIR_RIGHT;

int leftPwm() {
  return constrain(SCENARIO_B_TARGET_PWM + SCENARIO_B_LEFT_PWM_OFFSET, 0, 255);
}

int rightPwm() {
  return constrain(SCENARIO_B_TARGET_PWM + SCENARIO_B_RIGHT_PWM_OFFSET, 0, 255);
}

void sendEvent(const char *event, unsigned long tUs, float rpm = 0, unsigned long durationUs = 0) {
  Serial.print("B_EVENT,"); Serial.print(SCENARIO_B_TRIAL_ID); Serial.print(','); Serial.print(event); Serial.print(',');
  Serial.print(tUs); Serial.print(','); Serial.print(SCENARIO_B_TARGET_PWM); Serial.print(','); Serial.print(rpm, 2); Serial.print(','); Serial.println(durationUs);
}

void sendSample(unsigned long tUs, float rpm, unsigned long loopTimeUs) {
  Serial.print("B_SAMPLE,"); Serial.print(SCENARIO_B_TRIAL_ID); Serial.print(','); Serial.print(tUs); Serial.print(',');
  Serial.print(SCENARIO_B_TARGET_PWM); Serial.print(','); Serial.print(rpm, 2); Serial.print(','); Serial.print(currentMotorCommand()); Serial.print(',');
  Serial.print(currentMotorPwm()); Serial.print(','); Serial.println(loopTimeUs);
}

const char *frontSensorEvent(const LineSensors &line) {
  if (line.frontRight && line.frontLeft) return "LINE_DETECTED_BOTH";
  return line.frontRight ? "LINE_DETECTED_RIGHT" : "LINE_DETECTED_LEFT";
}
}

void initScenarioB() {
  if (!SCENARIO_B_ENABLED) return;
  phase = B_RUN_FORWARD;
  lastSampleMs = millis();
  Serial.println("B_INFO,trial,event,t_us,target_pwm,rpm_left,duration_us");
  Serial.println("B_SAMPLE,trial,t_us,target_pwm,rpm_left,motor_command,motor_pwm,loop_us");
  sendEvent("TRIAL_START", micros());
}

bool scenarioBActive() {
  return phase != B_DISABLED;
}

void runScenarioB(const LineSensors &line, unsigned long loopStartUs) {
  (void)loopStartUs;
  updateLeftWheelRpm(SCENARIO_A_RPM_SAMPLE_INTERVAL_MS);
  unsigned long nowUs = micros();
  unsigned long nowMs = millis();

  if (nowMs - lastSampleMs >= SCENARIO_A_RPM_SAMPLE_INTERVAL_MS) {
    sendSample(nowUs, leftWheelRpm(), loopStartUs <= nowUs ? nowUs - loopStartUs : 0);
    lastSampleMs = nowMs;
  }

  if (phase == B_RUN_FORWARD) {
    forward(leftPwm(), rightPwm());
    if (line.frontLeft || line.frontRight) {
      lineDetectedUs = nowUs;
      turnDirection = (line.frontRight && !line.frontLeft) ? DIR_LEFT : DIR_RIGHT;
      brakeMotor();
      unsigned long commandChangedUs = micros();
      sendEvent(frontSensorEvent(line), lineDetectedUs, leftWheelRpm());
      sendEvent("RESPONSE_LATENCY", commandChangedUs, leftWheelRpm(), commandChangedUs - lineDetectedUs);
      phase = B_BRAKING;
    }
    return;
  }

  if (phase == B_BRAKING) {
    brakeMotor();
    if (leftWheelRpm() == 0 || nowUs - lineDetectedUs >= SCENARIO_B_MAX_BRAKE_TIME_MS * 1000UL) {
      backward(PWM_ESCAPE_BACK);
      backingStartMs = nowMs;
      phase = B_BACKING;
    }
    return;
  }

  if (phase == B_BACKING) {
    if (nowMs - backingStartMs >= SCENARIO_B_BACK_TIME_MS) {
      rotate(turnDirection, SCENARIO_B_TURN_PWM);
      turnStartUs = micros();
      sendEvent("BACKING_COMPLETE", nowUs, leftWheelRpm(), nowUs - lineDetectedUs);
      phase = B_TURNING;
    } else {
      backward(PWM_ESCAPE_BACK);
    }
    return;
  }

  if (phase == B_TURNING) {
    rotate(turnDirection, SCENARIO_B_TURN_PWM);
    if (nowUs - turnStartUs >= SCENARIO_B_TURN_TIME_MS * 1000UL) {
      forward(leftPwm(), rightPwm());
      unsigned long recoveryCompleteUs = micros();
      unsigned long turnDurationUs = recoveryCompleteUs - turnStartUs;
      sendEvent("TURN_COMPLETE", recoveryCompleteUs, leftWheelRpm(), turnDurationUs);
      sendEvent("RECOVERY_COMPLETE", recoveryCompleteUs, leftWheelRpm(), recoveryCompleteUs - lineDetectedUs);
      phase = B_RETURNING;
    }
    return;
  }

  if (phase == B_RETURNING) {
    forward(leftPwm(), rightPwm());
  }
}
