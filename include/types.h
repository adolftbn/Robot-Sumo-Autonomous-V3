#pragma once

struct LineSensors {
  bool frontLeft;
  bool frontRight;
  bool left;
  bool right;
  bool back;
};

enum Direction { DIR_LEFT, DIR_RIGHT };
enum SearchState { SEARCH_FORWARD, SEARCH_ROTATE };
enum EscapeState {
  ESCAPE_NONE,
  ESCAPE_FRONT_BRAKE,
  ESCAPE_FRONT_BACK,
  ESCAPE_FRONT_TURN,
  ESCAPE_SIDE_TURN,
  ESCAPE_BACK_FORWARD
};
enum LastSeen { NONE, LEFT, RIGHT };
enum RobotState { STATE_SEARCH, STATE_TRACK_LEFT, STATE_TRACK_RIGHT, STATE_APPROACH, STATE_ATTACK, STATE_ESCAPE };
enum MotorCommand { MOTOR_STOP, MOTOR_BRAKE, MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_ROTATE_LEFT, MOTOR_ROTATE_RIGHT };
