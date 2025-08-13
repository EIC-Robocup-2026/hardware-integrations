#pragma once
#include <Servo.h>
#include <ArduinoEigen.h>

struct Segment {
  Servo *servo; // joint's servo
  Eigen::Vector2f position; // joint's projection on z-target plane
  float length; // link's length
};

void init_segment(struct Segment *segment, Servo *servo, float length);
