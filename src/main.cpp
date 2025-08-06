#include <Arduino.h>
#include <Servo.h>
#include <Joint.h>
#include <Point.h>

// lengths of the robotic arm segments in meters
double l1 = 0.105; // length of link 1 controled by servo 2
double l2 = 0.125; // length of link 2 controled by servo 3
double l3 = 0.175; // length of link 3 controled by servo 4

Servo servo_1;  // servo for base rotation
Servo servo_2;  // servo for first link joint
Servo servo_3;  // servo for second link joint
Servo servo_4;  // servo for third link joint
Servo servo_5;  // servo for end effector rotation
Servo servo_6;  // servo for end effector gripper

Servo servos[6] = {servo_1, servo_2, servo_3, servo_4, servo_5, servo_6};


// when initializing the servos the arm should be in the upright position with open gripper 
double initializingTheta_1 = 45;
double initializingTheta_2 = 45;
double initializingTheta_3 = 90;
double initializingTheta_4 = 90;
double initializingTheta_5 = 0;
double initializingTheta_6 = 0;

double initializingThetas[6] = {
  initializingTheta_1,
  initializingTheta_2,
  initializingTheta_3,
  initializingTheta_4,
  initializingTheta_5,
  initializingTheta_6
};

// current angles of the servos
double currentTheta_1;
double currentTheta_2;
double currentTheta_3;
double currentTheta_4;
double currentTheta_5;
double currentTheta_6;

double currentThetas[6] = {
  currentTheta_1,
  currentTheta_2,
  currentTheta_3,
  currentTheta_4,
  currentTheta_5,
  currentTheta_6
};


void setup() {

  //Lolin ESP8266 NodeMCU V3 pinout
  servo_1.attach(5,500,2500);   // D1
  servo_2.attach(4,500,2500);   // D2
  servo_3.attach(0,500,2500);   // D3
  servo_4.attach(14,500,2500);  // D5
  servo_5.attach(12,500,2500);  // D6
  servo_6.attach(13,500,2500);  // D7
  
  Serial.begin(9600);

  for (int i = 0; i < 6; i++) {
    servos[i].write(initializingThetas[i]);
  }


  Joint joint_1(initializingTheta_2, l1, servo_2);   // Joint 1 is the first link controlled by servo_2
  Joint joint_2(&joint_1, initializingTheta_3 -90 , l2, servo_3);           // Joint 2 is the second link controlled by servo_3 with parent joint as joint_1
  Joint joint_3(&joint_2, initializingTheta_4 -90 , l3, servo_4);           // Joint 3 is the third link controlled by servo_4 with parent joint as joint_2
 
  delay(3000); // delayed for servos to reach initializing positions
  Serial.println("Robotic arm initialized with servos at initial angles.");
}

void loop() {
  for (int i = 0; i < 6; i++) {
    currentThetas[i] = servos[i].read(); // update current angles
  }

  
  for (int i = 0; i < 180; i += 1) {
    servo_1.write(i);
    servo_2.write(i);
    servo_3.write(i);
    servo_4.write(i);
    servo_5.write(i);
    servo_6.write(i);
    delay(5);
  }

  for (int i = 180; i > 0; i -= 1) {
    servo_1.write(i);
    servo_2.write(i);
    servo_3.write(i);
    servo_4.write(i);
    servo_5.write(i);
    servo_6.write(i);
    delay(5);
  }

  



}

void CCD(double theta){
  
}