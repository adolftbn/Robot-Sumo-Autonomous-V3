#include <Arduino.h>
#include "config.h"
#include "escape.h"
#include "behavior.h"
#include "motor.h"

static EscapeState escapeState = ESCAPE_NONE;
static Direction escapeDirection = DIR_RIGHT;
static unsigned long escapeTimer = 0, rearWarningTimer = 0;
static void beginEscape(EscapeState state, Direction direction) { escapeState = state; escapeDirection = direction; escapeTimer = millis(); rearWarningTimer = 0; }
bool handleEscape(const LineSensors &line) {
  unsigned long now = millis();
  if (escapeState == ESCAPE_NONE) {
    if (line.frontLeft || line.frontRight) beginEscape(ESCAPE_FRONT_BRAKE, (line.frontRight && !line.frontLeft) ? DIR_LEFT : DIR_RIGHT);
    else if (line.left) beginEscape(ESCAPE_SIDE_TURN, DIR_RIGHT);
    else if (line.right) beginEscape(ESCAPE_SIDE_TURN, DIR_LEFT);
    else if (line.back) beginEscape(ESCAPE_BACK_FORWARD, DIR_RIGHT);
    else return false;
  }
  switch (escapeState) {
    case ESCAPE_FRONT_BRAKE: brakeMotor(); if (now - escapeTimer >= ESCAPE_FRONT_BRAKE_TIME_MS) { escapeState = ESCAPE_FRONT_BACK; escapeTimer = now; } break;
    case ESCAPE_FRONT_BACK:
      if (line.back && rearWarningTimer == 0) rearWarningTimer = now; else if (!line.back) rearWarningTimer = 0;
      if ((rearWarningTimer && now - rearWarningTimer >= REAR_CONFIRM_TIME_MS) || now - escapeTimer >= ESCAPE_FRONT_MAX_BACK_TIME_MS) { escapeState = ESCAPE_FRONT_TURN; escapeTimer = now; rotate(escapeDirection, PWM_ESCAPE_TURN); }
      else if (now - escapeTimer >= ESCAPE_FRONT_MIN_BACK_TIME_MS && !line.frontLeft && !line.frontRight) { escapeState = ESCAPE_FRONT_TURN; escapeTimer = now; rotate(escapeDirection, PWM_ESCAPE_TURN); }
      else backward(PWM_ESCAPE_BACK);
      break;
    case ESCAPE_FRONT_TURN: rotate(escapeDirection, PWM_ESCAPE_TURN); if (now - escapeTimer >= ESCAPE_FRONT_TURN_TIME_MS) { escapeState = ESCAPE_NONE; resetBehaviorSearch(); } break;
    case ESCAPE_SIDE_TURN: rotate(escapeDirection, PWM_ESCAPE_TURN); if (now - escapeTimer >= ESCAPE_SIDE_TURN_TIME_MS) { escapeState = ESCAPE_NONE; resetBehaviorSearch(); } break;
    case ESCAPE_BACK_FORWARD:
      if (line.frontLeft || line.frontRight) { stopMotor(); escapeState = ESCAPE_FRONT_TURN; escapeTimer = now; }
      else { forward(PWM_ESCAPE_BACK); if (now - escapeTimer >= ESCAPE_BACK_FORWARD_TIME_MS && !line.back) { escapeState = ESCAPE_NONE; resetBehaviorSearch(); } }
      break;
    case ESCAPE_NONE: break;
  }
  return true;
}
EscapeState currentEscapeState() { return escapeState; }
