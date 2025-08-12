#include <Arduino.h>
#include <Servo.h>
#include <Vector2D.h>

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

  double dx;
  double dy;
  double dz;
  double roll;
  double pitch;
  double yaw;
  double gripperStrength;
  double mission_accomplish;

  double VLA_ARM_PROPORTIONAL_GAIN = 20; // proportional gain for VLA arm movement input, ADJUSTABLE — based on dx dz
  double VLA_BASE_ROTATION_GAIN = 10; // proportional gain for VLA base rotation input, ADJUSTABLE — based on dy
  double VLA_GRIPPER_ROTATION_GAIN = 10; // proportional gain for VLA gripper rotation input, ADJUSTABLE — based on roll

  //Weight distribution among link rotation for CCD — W_3 > W_2 > W_1 ; should be like this when adjusting
  double W_1 = 0.3; // weight for link 1 rotation
  double W_2 = 0.6; // weight for link 2 rotation
  double W_3 = 0.9; // weight for link 3 rotation

  //receive 8 input from VLA in this format: 0.2,0.3,0.5,20,60,40,-0.8,0
  if (Serial.available()) {
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
    Vector2D vectorVLA = Vector2D(dx,dz).normalize().scalarMultiply(VLA_ARM_PROPORTIONAL_GAIN); // VLA Vector adjusted from 

    //calculate all vectors based on the angle of each servo
    double theta_2_rad = (double)servo_2.read() * M_PI / 180;
    double theta_3_rad = (double)servo_3.read() * M_PI / 180;
    double theta_4_rad = (double)servo_4.read() * M_PI / 180;

    //All link vectors calculation
    Vector2D link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
    Vector2D link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
    Vector2D link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
    Vector2D endEffectorVector = link1.add(link2).add(link3);
    
    //Vector from each joint to target location
    Vector2D link1_jointToTarget = endEffectorVector.add(vectorVLA);
    Vector2D link2_jointToTarget = link1_jointToTarget.minus(link1);
    Vector2D link3_jointToTarget = link2_jointToTarget.minus(link2);

    //Calculate angle from links to target vector directionally
    double angle_link1_jointToTarget = link1.angleBetweenVector(link1_jointToTarget) * 180 / M_PI;
    double angle_link2_jointToTarget = link2.angleBetweenVector(link2_jointToTarget) * 180 / M_PI;
    double angle_link3_jointToTarget = link3.angleBetweenVector(link3_jointToTarget) * 180 / M_PI;
    

    //simple logic for base rotation using only dy as the only servo 1 can cause changes in dy
    servo_1.write(servo_1.read() + dy * VLA_BASE_ROTATION_GAIN);

    //CCD inverse kinematic for end effector movement
    servo_2.write(servo_2.read() + angle_link1_jointToTarget);
    servo_3.write(servo_3.read() + angle_link2_jointToTarget);
    servo_4.write(servo_4.read() + angle_link3_jointToTarget);
    
    //gripper grab and rotation control
    servo_5.write(servo_5.read() + roll * VLA_GRIPPER_ROTATION_GAIN);
    servo_6.write( mapDouble(gripperStrength, -1.0, 1.0, 180.0, 90.0) );

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
