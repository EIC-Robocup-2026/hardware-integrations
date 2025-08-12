#pragma once
#include <Servo.h>
#include <ArduinoEigen.h>

typedef struct Joint {
  Servo servo; // joint's servo
  float minAngle; // minimum angle of the joint
  float maxAngle; // maximum angle of the joint
  float length; // link's length
  float angle; // angle of the link relative to the previous joint
  float relativeAngle; // angle of the link relative to x axis in the z-target plane
  Eigen::Vector3f absolutePosition; // joint's cartesian coordinates
  Eigen::Vector2f relativePosition; // joint's projection on z-target plane
  Joint *next;
} Joint;
