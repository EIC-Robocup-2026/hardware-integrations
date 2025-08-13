#include <Arduino.h>
#include <Servo.h>
#include "Vector2D.h"
#include "segment.h"
#include "utils.h"

#define SERVO_PWN_MIN 500
#define SERVO_PWN_MAX 2500
#define DOF 6
#define N_SEG 3

// when initializing the servos the arm should be in the upright position with open gripper
Servo servos[DOF];
double initializingThetas[] = { 90, 90, 90, 90, 0, 180 }; // initial angles for servos in degrees
constexpr int servo_pins[] = { 5, 4, 0, 14, 12, 13 }; // pins for servos 1-6

struct Segment segments[N_SEG];
constexpr float segment_lengths[] = { 105, 125, 175 };

//mapping value
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(9600);

  // Lolin ESP8266 NodeMCU V3 pinout
  for (int i = 0; i < DOF; i++) {
    servos[i].attach(servo_pins[i], SERVO_PWN_MIN, SERVO_PWN_MAX);
    servos[i].write(initializingThetas[i]);
  }

  for (int i = 0; i < N_SEG; i++) {
    init_segment(segments + i, servos + 1 + i, segment_lengths[i]);
  }

  delay(5000); // delayed for servos to reach initializing positions
  Serial.println("Robotic arm initialized with servos at initial angles.");
}

void loop(){

  //receive 8 input from VLA in this format: 0.2,0.3,0.5,20,60,40,-0.8,0
  if (Serial.available()) {
    double VLA_ARM_PROPORTIONAL_GAIN = 1; // proportional gain for VLA arm movement input, ADJUSTABLE — based on dx dz
    double VLA_BASE_ROTATION_GAIN = 10; // proportional gain for VLA base rotation input, ADJUSTABLE — based on dy
    double VLA_GRIPPER_ROTATION_GAIN = 10; // proportional gain for VLA gripper rotation input, ADJUSTABLE — based on roll
    int CCD_ITERATION = 2000; // number of iteration for ccd execution

    // //Weight distribution among link rotation for CCD — W_3 > W_2 > W_1 ; should be like this when adjusting
    // double W_1 = 1; // weight for link 1 rotation
    // double W_2 = 1; // weight for link 2 rotation
    // double W_3 = 1; // weight for link 3 rotation

    float currentServoAngle[DOF];
    for (int i = 0; i < DOF; i++) {
      currentServoAngle[i] = (float) servos[i].read();
      Serial.printf("Angle %d: %f\n", i, currentServoAngle[i]);
    }

    VLAInput vla_input = parse_input(Serial.readStringUntil('\n'));
    
    if (vla_input.err = 0) {

      //All calculation for Cyclic Coordinate Descent — CCD wil be done on x-z plane

      //Turns input from VLA into unit vector and scaled by VLA_ARM_PROPORTIONAL_GAIN
      Vector2D vectorVLA = Vector2D(vla_input.dx, vla_input.dz).scalarMultiply(VLA_ARM_PROPORTIONAL_GAIN);

      //calculate all vectors based on the angle of each servo
      double theta_2_rad = currentServoAngle[1] * M_PI / 180;
      double theta_3_rad = currentServoAngle[2] * M_PI / 180;
      double theta_4_rad = currentServoAngle[3] * M_PI / 180;

      //All link vectors calculation
      Vector2D link1 = Vector2D(-segment_lengths[0]*cos(theta_2_rad) , segment_lengths[0]*sin(theta_2_rad) );
      Vector2D link2 = Vector2D( segment_lengths[1]*cos( M_PI_2 - theta_2_rad + theta_3_rad) , segment_lengths[1]*sin(M_PI/2 - theta_2_rad + theta_3_rad));
      Vector2D link3 = Vector2D( segment_lengths[2] * cos(theta_4_rad + theta_3_rad - theta_2_rad) , segment_lengths[2] * sin(theta_4_rad + theta_3_rad - theta_2_rad));
      Vector2D endEffectorVector = link1.add(link2).add(link3);

      //fixing target point for CCD loop
      double targetPoint_x = endEffectorVector.x + vectorVLA.x;
      double targetPoint_y = endEffectorVector.y + vectorVLA.y;
      
      Serial.print("current end effector: "); Serial.print(endEffectorVector.x);Serial.print(" , ");Serial.println(endEffectorVector.y);
      Serial.print("target point: "); Serial.print(targetPoint_x);Serial.print(" , ");Serial.println(targetPoint_y);

      for (int i = 0 ; i < CCD_ITERATION; i++){

        //calculate all vectors based on the angle of each servo
        theta_2_rad = currentServoAngle[1] * M_PI / 180;
        theta_3_rad = currentServoAngle[2] * M_PI / 180;
        theta_4_rad = currentServoAngle[3] * M_PI / 180;

        //All link vectors calculation
        Vector2D link1 = Vector2D( -segment_lengths[0]*cos(theta_2_rad) , segment_lengths[0]*sin(theta_2_rad) );
        Vector2D link2 = Vector2D( segment_lengths[1] *cos( M_PI_2 - theta_2_rad + theta_3_rad) , segment_lengths[1]*sin(M_PI/2 - theta_2_rad + theta_3_rad));
        Vector2D link3 = Vector2D( segment_lengths[2] * cos(theta_4_rad + theta_3_rad - theta_2_rad) , segment_lengths[2] * sin(theta_4_rad + theta_3_rad - theta_2_rad));
        Vector2D endEffectorVector = link1.add(link2).add(link3);
        
        //Vector from each joint to target location
        Vector2D link1_jointToTarget = Vector2D(targetPoint_x, targetPoint_y);
        Vector2D link2_jointToTarget = link1_jointToTarget.minus(link1);
        Vector2D link3_jointToTarget = link2_jointToTarget.minus(link2);

        //Calculate angle from links to target vector directionally
        double angle_link1_jointToTarget = link1.angleBetweenVector(link1_jointToTarget) * 180 / M_PI;
        double angle_link2_jointToTarget = link2.angleBetweenVector(link2_jointToTarget) * 180 / M_PI;
        double angle_link3_jointToTarget = link3.angleBetweenVector(link3_jointToTarget) * 180 / M_PI;

        //simple logic for base rotation using only dy as the only servo 1 can cause changes in dy
        currentServoAngle[0] += vla_input.dy * VLA_BASE_ROTATION_GAIN;

        //CCD inverse kinematic for end effector movement
        currentServoAngle[1] += angle_link1_jointToTarget;
        currentServoAngle[2] += angle_link2_jointToTarget;
        currentServoAngle[3] += angle_link3_jointToTarget;
      
        //gripper grab and rotation control
        currentServoAngle[4] += vla_input.roll * VLA_GRIPPER_ROTATION_GAIN;
        currentServoAngle[5] = mapDouble(vla_input.gripper_strength, -1.0, 1.0, 180.0, 90.0);

        //setting limit for servo angle ignore my bad coding
        for (int i = 0; i < DOF; i++) {
          currentServoAngle[i] = fmin(currentServoAngle[i], 180);
          currentServoAngle[i] = fmax(currentServoAngle[i], 0);
        }

        // Vector2D distanceEndToTarget = Vector2D(targetPoint_x - endEffectorVector.x, targetPoint_y - endEffectorVector.y);
        // if (distanceEndToTarget.getMagnitude() < 5) break;
      }

      for (int i = 0; i < DOF; i++) {
        servos[i].write(currentServoAngle[i]);
      }

      //print log;
      Serial.print("dx: ");Serial.println(vla_input.dx);
      Serial.print("dy: ");Serial.println(vla_input.dy);
      Serial.print("dz: ");Serial.println(vla_input.dz);
      

      Serial.print("roll: ");Serial.println(vla_input.roll);
      Serial.print("pitch: ");Serial.println(vla_input.pitch);
      Serial.print("yaw: ");Serial.println(vla_input.yaw);

      
      Serial.print("Distance from End Effector to Target: ");Serial.println(vectorVLA.getMagnitude());

      //new link vectors calculation
      link1 = Vector2D(-segment_lengths[0]*cos(theta_2_rad) , segment_lengths[0]*sin(theta_2_rad) );
      link2 = Vector2D( segment_lengths[1]*cos( M_PI_2 - theta_2_rad + theta_3_rad) , segment_lengths[1]*sin(M_PI/2 - theta_2_rad + theta_3_rad));
      link3 = Vector2D( segment_lengths[2] * cos(theta_4_rad + theta_3_rad - theta_2_rad) , segment_lengths[2] * sin(theta_4_rad + theta_3_rad - theta_2_rad));
      endEffectorVector = link1.add(link2).add(link3);

      Serial.print("new end effector: "); Serial.print(endEffectorVector.x);Serial.print(" , ");Serial.println(endEffectorVector.y);

    }

    else {
      Serial.println("Invalid input. Please enter 8 comma-separated numbers.");
    }



  }
}  
