#pragma once
#include "types.h"

void initSensors();
LineSensors readLineSensors();
void updateUltrasonic();
long leftDistanceCm();
long rightDistanceCm();
bool leftTargetFresh(unsigned long now);
bool rightTargetFresh(unsigned long now);
void updateLeftWheelRpm();
float leftWheelRpm();
