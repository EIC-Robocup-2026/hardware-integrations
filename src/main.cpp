#include <Arduino.h>
#include <Servo.h>
#include "Vector2D.h"

int MIN_MICROSEC = 500;
int MAX_MICROSEC = 2500;

// lengths of the robotic arm segments in millimeters
double l1 = 105; // length of link 1 controled by servo 2
double l2 = 125; // length of link 2 controled by servo 3
double l3 = 175; // length of link 3 controled by servo 4

// when initializing the servos the arm should be in the upright position with open gripper
Servo servos[6];
int initializingThetas[6] = {90,90,90,90,0,90};
int attachPin[6] = {5,4,0,14,12,13}; //D1,D2,D3,D5,D6,D7


// Lolin ESP8266 NodeMCU V3
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++){
    servos[i].attach(attachPin[i], MIN_MICROSEC , MAX_MICROSEC);
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
    int CCD_ITERATION = 200; // number of iteration for ccd execution

    //current servo angle array
    double currentServoAngle[6];
    for(int i = 0; i < 6; i++){
      currentServoAngle[i] = servos[i].read();
      Serial.print("Servo Angle ");Serial.print(i);Serial.print(" : ");Serial.println(currentServoAngle[i]);
    }

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
    //Turns input from VLA into vector and scale by VLA_ARM_PROPORTIONAL_GAIN
    Vector2D vectorVLA = Vector2D(dx,dz).scalarMultiply(VLA_ARM_PROPORTIONAL_GAIN);

    //calculate all vectors based on the angle of each servo
    double theta_2_rad = currentServoAngle[1] * M_PI / 180;
    double theta_3_rad = currentServoAngle[2] * M_PI / 180;
    double theta_4_rad = currentServoAngle[3] * M_PI / 180;

    //All link vectors calculation
    Vector2D link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
    Vector2D link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
    Vector2D link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
    Vector2D endEffectorVector = link1.add(link2).add(link3);

    //fixing target point for CCD loop, nigga im lazy
    double targetPoint_x = endEffectorVector.x + vectorVLA.x;
    double targetPoint_y = endEffectorVector.y + vectorVLA.y;
    
    Serial.print("current end effector: "); Serial.print(endEffectorVector.x);Serial.print(" , ");Serial.println(endEffectorVector.y);
    Serial.print("target point: "); Serial.print(targetPoint_x);Serial.print(" , ");Serial.println(targetPoint_y);

    //start CCD from link 3 and regress down to link 1
    int operatingLink = 3;
    double marginOfError = vectorVLA.getMagnitude() * 0.01; // 1 percent error allowed

    for (int i = 0 ; i < CCD_ITERATION; i++){
      //rad from servo degree
      theta_2_rad = currentServoAngle[1] * M_PI / 180;
      theta_3_rad = currentServoAngle[2] * M_PI / 180;
      theta_4_rad = currentServoAngle[3] * M_PI / 180;

      //All link vectors calculation
      link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
      link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI_2 - theta_2_rad + theta_3_rad));
      link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
      endEffectorVector = link1.add(link2).add(link3);
    
      //braek CCD algorithm if endeffector allign with target within margin of error
      // Vector2D distanceCheck = Vector2D(targetPoint_x - endEffectorVector.x, targetPoint_y - endEffectorVector.y );
      // if ( distanceCheck.getMagnitude() <= marginOfError ){
      //   break;
      // }

      //CCD one link at a time and switch to the other

      const int STATE_1 = 1 ;
      const int STATE_2 = 2 ;
      const int STATE_3 = 3 ;

      double angle_link1_jointToTarget ;
      double angle_link2_jointToTarget;
      double angle_link3_jointToTarget;

       //Vector from each joint to target location
      Vector2D link1_jointToTarget = Vector2D(targetPoint_x, targetPoint_y);
      Vector2D link2_jointToTarget = link1_jointToTarget.minus(link1);
      Vector2D link3_jointToTarget = link2_jointToTarget.minus(link2);
      Vector2D joint2_toEndEffector = link2.add(link3);

      switch(operatingLink){

        case STATE_3:{
          angle_link3_jointToTarget = link3.angleBetweenVector(link3_jointToTarget) * 180 / M_PI;
          currentServoAngle[3] += angle_link3_jointToTarget;
          operatingLink = 2;
}
        case STATE_2:{
          angle_link2_jointToTarget = joint2_toEndEffector.angleBetweenVector(link2_jointToTarget) * 180 / M_PI;
          currentServoAngle[2] += angle_link2_jointToTarget;
          operatingLink = 1;
        }

        case STATE_1:{
          angle_link1_jointToTarget = endEffectorVector.angleBetweenVector(link1_jointToTarget) * 180 / M_PI;
          currentServoAngle[1] += angle_link1_jointToTarget;
          operatingLink = 3;
        }
      }

      //capping servo angle between 0 and 180
      for (int i =1; i < 4; i++ ){
        (currentServoAngle[i] > 180) ? currentServoAngle[i] = 180: currentServoAngle[i];
        (currentServoAngle[i] < 0) ? currentServoAngle[i] = 0: currentServoAngle[i];
      }
    }

    //simple logic for base rotation using only dy as the only servo 1 can cause changes in dy
    currentServoAngle[0] += dy * VLA_BASE_ROTATION_GAIN;

    //gripper grab and rotation control
    currentServoAngle[4] += roll * VLA_GRIPPER_ROTATION_GAIN;
    currentServoAngle[5] = mapDouble(gripperStrength, -1.0, 1.0, 180.0, 90.0);

    for(int i = 0 ; i< 6; i++){
      servos[i].write(currentServoAngle[i]);
    }

    //print log;
    Serial.print("dx: ");Serial.println(dx);
    Serial.print("dy: ");Serial.println(dy);
    Serial.print("dz: ");Serial.println(dz);
    Serial.print("roll: ");Serial.println(roll);
    Serial.print("pitch: ");Serial.println(pitch);
    Serial.print("yaw: ");Serial.println(yaw);

    
    Serial.print("Distance from End Effector to Target: ");Serial.println(vectorVLA.getMagnitude());
    }

    else {
      Serial.println("Invalid input. Please enter 8 comma-separated numbers.");
    }



  }
}  
