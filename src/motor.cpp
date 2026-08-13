#include <Arduino.h>
#include "config.h"
#include "motor.h"

static MotorCommand command = MOTOR_STOP;
static int pwmValue = 0;

static void setMotorState(MotorCommand newCommand, int pwm) { command = newCommand; pwmValue = pwm; }

void initMotor() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  ledcSetup(PWM_CHANNEL_A, 1000, 8); ledcAttachPin(ENA, PWM_CHANNEL_A);
  ledcSetup(PWM_CHANNEL_B, 1000, 8); ledcAttachPin(ENB, PWM_CHANNEL_B);
  stopMotor();
}
void stopMotor() { ledcWrite(PWM_CHANNEL_A, 0); ledcWrite(PWM_CHANNEL_B, 0); setMotorState(MOTOR_STOP, 0); }
void brakeMotor() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CHANNEL_A, 255); ledcWrite(PWM_CHANNEL_B, 255); setMotorState(MOTOR_BRAKE, 255);
}
void forward(int pwm) { forward(pwm, pwm); }
void forward(int leftPwm, int rightPwm) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_CHANNEL_A, leftPwm); ledcWrite(PWM_CHANNEL_B, rightPwm); setMotorState(MOTOR_FORWARD, leftPwm);
}
void forwardChannelAOnly(int pwm) { forward(pwm, 0); }
void forwardChannelBOnly(int pwm) { forward(0, pwm); }
void backward(int pwm) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CHANNEL_A, pwm); ledcWrite(PWM_CHANNEL_B, pwm); setMotorState(MOTOR_BACKWARD, pwm);
}
void rotateLeft(int pwm) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_CHANNEL_A, pwm); ledcWrite(PWM_CHANNEL_B, pwm); setMotorState(MOTOR_ROTATE_LEFT, pwm);
}
void rotateRight(int pwm) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CHANNEL_A, pwm); ledcWrite(PWM_CHANNEL_B, pwm); setMotorState(MOTOR_ROTATE_RIGHT, pwm);
}
void rotate(Direction direction, int pwm) { direction == DIR_LEFT ? rotateLeft(pwm) : rotateRight(pwm); }
MotorCommand currentMotorCommand() { return command; }
int currentMotorPwm() { return pwmValue; }
