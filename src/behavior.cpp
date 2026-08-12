#include <Arduino.h>
#include "config.h"
#include "behavior.h"
#include "motor.h"
#include "sensors.h"

static SearchState searchState = SEARCH_ROTATE;
static LastSeen lastSeen = NONE;
static bool targetWasCentered = false, targetWasClose = false;
static unsigned long searchTimer = 0, lastTargetTime = 0;
static void resetSearch(unsigned long now) { searchState = SEARCH_FORWARD; searchTimer = now; }

void resetBehaviorSearch() { resetSearch(millis()); }

RobotState runBehavior(unsigned long now) {
  updateUltrasonic();
  bool leftTarget = leftTargetFresh(now), rightTarget = rightTargetFresh(now);
  long leftDist = leftDistanceCm(), rightDist = rightDistanceCm();
  if (leftTarget || rightTarget) { lastTargetTime = now; resetSearch(now); }
  if (leftTarget && rightTarget) {
    targetWasCentered = true; targetWasClose = leftDist < ATTACK_RANGE_CM && rightDist < ATTACK_RANGE_CM; lastSeen = leftDist < rightDist ? LEFT : RIGHT;
    forward(targetWasClose ? PWM_ATTACK : PWM_APPROACH); return targetWasClose ? STATE_ATTACK : STATE_APPROACH;
  }
  if (leftTarget) { targetWasCentered = false; targetWasClose = false; lastSeen = LEFT; rotateLeft(PWM_TRACK); return STATE_TRACK_LEFT; }
  if (rightTarget) { targetWasCentered = false; targetWasClose = false; lastSeen = RIGHT; rotateRight(PWM_TRACK); return STATE_TRACK_RIGHT; }
  if (lastTargetTime && now - lastTargetTime < TARGET_HOLD_TIME_MS) {
    if (targetWasCentered) { forward(targetWasClose ? PWM_ATTACK : PWM_APPROACH); return targetWasClose ? STATE_ATTACK : STATE_APPROACH; }
    if (lastSeen == LEFT) { rotateLeft(PWM_TRACK); return STATE_TRACK_LEFT; }
    if (lastSeen == RIGHT) { rotateRight(PWM_TRACK); return STATE_TRACK_RIGHT; }
    forward(PWM_APPROACH); return STATE_APPROACH;
  }
  if (searchState == SEARCH_FORWARD) { forward(PWM_SEARCH); if (now - searchTimer >= SEARCH_FORWARD_TIME_MS) { searchState = SEARCH_ROTATE; searchTimer = now; } }
  else { if (lastSeen == RIGHT) rotateRight(PWM_SEARCH_ROTATE); else rotateLeft(PWM_SEARCH_ROTATE); if (now - searchTimer >= SEARCH_ROTATE_TIME_MS) { searchState = SEARCH_FORWARD; searchTimer = now; } }
  return STATE_SEARCH;
}
