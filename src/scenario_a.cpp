#include <Arduino.h>
#include "config.h"
#include "escape.h"
#include "motor.h"
#include "scenario_a.h"
#include "sensors.h"
#include "telemetry.h"

enum ScenarioAPhase { A_DISABLED, A_RUN_FORWARD, A_BRAKING, A_BACKING, A_COMPLETE };
static ScenarioAPhase phase = A_DISABLED;
static unsigned long lastSampleMs = 0;
static unsigned long lineDetectedUs = 0;
static unsigned long trialStartMs = 0;
static unsigned long backingStartMs = 0;
static float rpmBeforeLine = 0;
static bool stableRpmLogged = false;

void initScenarioA() {
  if (!SCENARIO_A_ENABLED) return;
  phase = A_RUN_FORWARD;
  trialStartMs = millis();
  Serial.println("A_INFO,trial,event,t_us,target_pwm,rpm_left,response_us");
  Serial.println("A_SAMPLE,trial,t_us,target_pwm,rpm_left,motor_command,motor_pwm,loop_us");
  sendScenarioAEvent("TRIAL_START", micros(), 0);
}

bool scenarioAActive() { return phase != A_DISABLED; }

void runScenarioA(const LineSensors &line, unsigned long loopStartUs) {
  updateLeftWheelRpm(SCENARIO_A_RPM_SAMPLE_INTERVAL_MS);
  unsigned long nowUs = micros();
  unsigned long nowMs = millis();

  if (phase != A_COMPLETE && nowMs - lastSampleMs >= SCENARIO_A_RPM_SAMPLE_INTERVAL_MS) {
    sendScenarioASample(nowUs, leftWheelRpm(), nowUs - loopStartUs);
    lastSampleMs = nowMs;
  }

  if (phase == A_RUN_FORWARD) {
    forward(SCENARIO_A_TARGET_PWM);
    if (!stableRpmLogged && nowMs - trialStartMs >= SCENARIO_A_RPM_STABLE_TIME_MS) {
      sendScenarioAEvent("RPM_STABLE", nowUs, leftWheelRpm());
      stableRpmLogged = true;
    }

    if (line.frontLeft || line.frontRight) {
      lineDetectedUs = micros();
      rpmBeforeLine = leftWheelRpm();
      brakeMotor();
      unsigned long motorCommandUs = micros();
      sendScenarioAEvent("LINE_DETECTED", lineDetectedUs, rpmBeforeLine, motorCommandUs - lineDetectedUs);
      phase = A_BRAKING;
    }
    return;
  }

  if (phase == A_BRAKING) {
    brakeMotor();
    unsigned long brakeTimeUs = nowUs - lineDetectedUs;
    if (leftWheelRpm() == 0 || brakeTimeUs >= SCENARIO_A_MAX_BRAKE_TIME_MS * 1000UL) {
      sendScenarioAEvent(leftWheelRpm() == 0 ? "RPM_ZERO" : "BRAKE_TIMEOUT", nowUs, leftWheelRpm(), brakeTimeUs);
      backward(PWM_ESCAPE_BACK);
      backingStartMs = nowMs;
      phase = A_BACKING;
    }
    return;
  }

  if (phase == A_BACKING) {
    if (line.back) {
      stopMotor();
      sendScenarioAEvent("BACK_LINE_DETECTED", nowUs, leftWheelRpm(), nowUs - lineDetectedUs);
      phase = A_COMPLETE;
    } else if (nowMs - backingStartMs >= SCENARIO_A_MIN_BACK_TIME_MS && !line.frontLeft && !line.frontRight) {
      stopMotor();
      sendScenarioAEvent("RECOVERY_COMPLETE", nowUs, leftWheelRpm(), nowUs - lineDetectedUs);
      phase = A_COMPLETE;
    } else {
      backward(PWM_ESCAPE_BACK);
    }
    return;
  }

  if (phase == A_COMPLETE) stopMotor();
}
