#pragma once
#include <Arduino.h>

struct VLAInput {
  float dx;
  float dy;
  float dz;
  float roll;
  float pitch;
  float yaw;
  float gripper_strength;
  float term;
  int err = 0;
};

VLAInput parse_input(String vla_input_str);
