#include <Arduino.h>
#include <Servo.h>
#include "Vector2D.h"

// lengths of the robotic arm segments in millimeters
double l1 = 105; // length of link 1 controled by servo 2
double l2 = 125; // length of link 2 controled by servo 3
double l3 = 175; // length of link 3 controled by servo 4

Servo servo_1; // servo for base rotation
Servo servo_2; // servo for first link joint
Servo servo_3; // servo for second link joint
Servo servo_4; // servo for third link joint
Servo servo_5; // servo for end effector rotation
Servo servo_6; // servo for end effector gripper

Servo servos[6] = {servo_1, servo_2, servo_3, servo_4, servo_5, servo_6};

// when initializing the servos the arm should be in the upright position with open gripper
double initializingTheta_1 = 90;
double initializingTheta_2 = 90;
double initializingTheta_3 = 90;
double initializingTheta_4 = 90;
double initializingTheta_5 = 0;
double initializingTheta_6 = 90;

double initializingThetas[6] = {
    initializingTheta_1,
    initializingTheta_2,
    initializingTheta_3,
    initializingTheta_4,
    initializingTheta_5,
    initializingTheta_6
};

void setup()
{

  // Lolin ESP8266 NodeMCU V3 pinout
  servo_1.attach(5, 500, 2500);  // D1
  servo_2.attach(4, 500, 2500);  // D2
  servo_3.attach(0, 500, 2500);  // D3
  servo_4.attach(14, 500, 2500); // D5
  servo_5.attach(12, 500, 2500); // D6
  servo_6.attach(13, 500, 2500); // D7

  Serial.begin(9600);

  for (int i = 0; i < 6; i++)
  {
    servos[i].write(initializingThetas[i]);
  }

  delay(5000); // delayed for servos to reach initializing positions
  Serial.println("Robotic arm initialized with servos at initial angles.");
}

//mapping value
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop(){

  //receive 8 input from VLA in this format: 0.2,0.3,0.5,20,60,40,-0.8,0
  if (Serial.available()) {
    double dx;
    double dy;
    double dz;
    double roll;
    double pitch;
    double yaw;
    double gripperStrength;
    double mission_accomplish;

    double VLA_ARM_PROPORTIONAL_GAIN = 1; // proportional gain for VLA arm movement input, ADJUSTABLE — based on dx dz
    double VLA_BASE_ROTATION_GAIN = 10; // proportional gain for VLA base rotation input, ADJUSTABLE — based on dy
    double VLA_GRIPPER_ROTATION_GAIN = 10; // proportional gain for VLA gripper rotation input, ADJUSTABLE — based on roll
    int CCD_ITERATION = 5; // number of iteration for ccd execution

    // //Weight distribution among link rotation for CCD — W_3 > W_2 > W_1 ; should be like this when adjusting
    // double W_1 = 1; // weight for link 1 rotation
    // double W_2 = 1; // weight for link 2 rotation
    // double W_3 = 1; // weight for link 3 rotation

    double currentServoAngle_1 = servo_1.read();
    double currentServoAngle_2 = servo_2.read();
    double currentServoAngle_3 = servo_3.read();
    double currentServoAngle_4 = servo_4.read();
    double currentServoAngle_5 = servo_5.read();
    double currentServoAngle_6 = servo_6.read();
    
    Serial.print("Angle 1: ");Serial.println(currentServoAngle_1);
    Serial.print("Angle 2: ");Serial.println(currentServoAngle_2);
    Serial.print("Angle 3: ");Serial.println(currentServoAngle_3);
    Serial.print("Angle 4: ");Serial.println(currentServoAngle_4);
    Serial.print("Angle 5: ");Serial.println(currentServoAngle_5);
    Serial.print("Angle 6: ");Serial.println(currentServoAngle_6);


    String input = Serial.readStringUntil('\n');
    input.trim();

    int idx1 = input.indexOf(',');
    int idx2 = input.indexOf(',', idx1 + 1);
    int idx3 = input.indexOf(',', idx2 + 1);
    int idx4 = input.indexOf(',', idx3 + 1);
    int idx5 = input.indexOf(',', idx4 + 1);
    int idx6 = input.indexOf(',', idx5 + 1);
    int idx7 = input.indexOf(',', idx6 + 1);

    //assign each value to each variable from VLA
    if (idx1 > 0 && idx2 > idx1 && idx3 > idx2 && idx4 > idx3 && idx5 > idx4 && idx6 > idx5 && idx7 > idx6) {
      dx = input.substring(0, idx1).toDouble();
      dy = input.substring(idx1 + 1, idx2).toDouble();
      dz = input.substring(idx2 + 1, idx3).toDouble();
      roll = input.substring(idx3 + 1, idx4).toDouble();
      pitch = input.substring(idx4 + 1, idx5).toDouble();
      yaw = input.substring(idx5 + 1, idx6).toDouble();
      gripperStrength = input.substring(idx6 + 1, idx7).toDouble();
      mission_accomplish = input.substring(idx7 + 1).toDouble();

    //All calculation for Cyclic Coordinate Descent — CCD wil be done on x-z plane

    //Turns input from VLA into unit vector and scaled by VLA_ARM_PROPORTIONAL_GAIN
    Vector2D vectorVLA = Vector2D(dx,dz).scalarMultiply(VLA_ARM_PROPORTIONAL_GAIN);

    //calculate all vectors based on the angle of each servo
    double theta_2_rad = currentServoAngle_2 * M_PI / 180;
    double theta_3_rad = currentServoAngle_3 * M_PI / 180;
    double theta_4_rad = currentServoAngle_4 * M_PI / 180;

    //All link vectors calculation
    Vector2D link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
    Vector2D link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
    Vector2D link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
    Vector2D endEffectorVector = link1.add(link2).add(link3);

    //fixing target point for CCD loop
    double targetPoint_x = endEffectorVector.x + vectorVLA.x;
    double targetPoint_y = endEffectorVector.y + vectorVLA.y;
    
    Serial.print("current end effector: "); Serial.print(endEffectorVector.x);Serial.print(" , ");Serial.println(endEffectorVector.y);
    Serial.print("target point: "); Serial.print(targetPoint_x);Serial.print(" , ");Serial.println(targetPoint_y);

    for (int i = 0 ; i < CCD_ITERATION; i++){

      //calculate all vectors based on the angle of each servo
      theta_2_rad = currentServoAngle_2 * M_PI / 180;
      theta_3_rad = currentServoAngle_3 * M_PI / 180;
      theta_4_rad = currentServoAngle_4 * M_PI / 180;

      //All link vectors calculation
      Vector2D link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
      Vector2D link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
      Vector2D link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
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
      currentServoAngle_1 += dy * VLA_BASE_ROTATION_GAIN;

      //CCD inverse kinematic for end effector movement
      currentServoAngle_2 += angle_link1_jointToTarget;
      currentServoAngle_3 += angle_link2_jointToTarget;
      currentServoAngle_4 += angle_link3_jointToTarget;
    
      //gripper grab and rotation control
      currentServoAngle_5 += roll * VLA_GRIPPER_ROTATION_GAIN;
      currentServoAngle_6 = mapDouble(gripperStrength, -1.0, 1.0, 180.0, 90.0);

      //setting limit for servo angle ignore my bad coding
      if (currentServoAngle_1 > 180) currentServoAngle_1 = 180;
      if (currentServoAngle_2 > 180) currentServoAngle_2 = 180;
      if (currentServoAngle_3 > 180) currentServoAngle_3 = 180;
      if (currentServoAngle_4 > 180) currentServoAngle_4 = 180;
      if (currentServoAngle_5 > 180) currentServoAngle_5 = 180;
      if (currentServoAngle_6 > 180) currentServoAngle_6 = 180;

      if (currentServoAngle_1 < 0) currentServoAngle_1 = 0;
      if (currentServoAngle_2 < 0) currentServoAngle_2 = 0;
      if (currentServoAngle_3 < 0) currentServoAngle_3 = 0;
      if (currentServoAngle_4 < 0) currentServoAngle_4 = 0;
      if (currentServoAngle_5 < 0) currentServoAngle_5 = 0;
      if (currentServoAngle_6 < 0) currentServoAngle_6 = 0;

      // Vector2D distanceEndToTarget = Vector2D(targetPoint_x - endEffectorVector.x, targetPoint_y - endEffectorVector.y);
      // if (distanceEndToTarget.getMagnitude() < 5) break;
    }

    servo_1.write(currentServoAngle_1);
    servo_2.write(currentServoAngle_2);
    servo_3.write(currentServoAngle_3);
    servo_4.write(currentServoAngle_4);
    servo_5.write(currentServoAngle_5);
    servo_6.write(currentServoAngle_6);

    //print log;
    Serial.print("dx: ");Serial.println(dx);
    Serial.print("dy: ");Serial.println(dy);
    Serial.print("dz: ");Serial.println(dz);
    

    Serial.print("roll: ");Serial.println(roll);
    Serial.print("pitch: ");Serial.println(pitch);
    Serial.print("yaw: ");Serial.println(yaw);

    
    Serial.print("Distance from End Effector to Target: ");Serial.println(vectorVLA.getMagnitude());

    //new link vectors calculation
    link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
    link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
    link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
    endEffectorVector = link1.add(link2).add(link3);

    Serial.print("new end effector: "); Serial.print(endEffectorVector.x);Serial.print(" , ");Serial.println(endEffectorVector.y);

    }

    else {
      Serial.println("Invalid input. Please enter 8 comma-separated numbers.");
    }



  }
}  
