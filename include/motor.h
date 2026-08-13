#pragma once
#include "types.h"

void initMotor();
void stopMotor();
void brakeMotor();
void forward(int pwm);
void forward(int leftPwm, int rightPwm);
void forwardChannelAOnly(int pwm);
void forwardChannelBOnly(int pwm);
void backward(int pwm);
void rotateLeft(int pwm);
void rotateRight(int pwm);
void rotate(Direction direction, int pwm);
MotorCommand currentMotorCommand();
int currentMotorPwm();
