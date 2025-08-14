#include <Arduino.h>
#include <Servo.h>
#include "Vector2D.h"

#define MIN_MICROSEC 500
#define MAX_MICROSEC 2500

#define VLA_ARM_PROPORTIONAL_GAIN 1     // proportional gain for VLA arm movement input, ADJUSTABLE — based on dx dz
#define VLA_BASE_ROTATION_GAIN 10       // proportional gain for VLA base rotation input, ADJUSTABLE — based on dy
#define VLA_GRIPPER_ROTATION_GAIN 1     // proportional gain for VLA gripper rotation input, ADJUSTABLE — based on roll
#define CCD_ITERATION 200               // number of iteration for ccd execution

// lengths of the robotic arm segments in millimeters
#define l1 105 // length of link 1 controled by servo 2
#define l2 125 // length of link 2 controled by servo 3
#define l3 175 // length of link 3 controled by servo 4


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

float dx, dy, dz, roll, pitch, yaw, gripper_intensity, mission_status;


void loop(){

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
            dx = input.substring(0, idx1).toFloat();
            dy = input.substring(idx1 + 1, idx2).toFloat();
            dz = input.substring(idx2 + 1, idx3).toFloat();
            roll = input.substring(idx3 + 1, idx4).toFloat();
            pitch = input.substring(idx4 + 1, idx5).toFloat();
            yaw = input.substring(idx5 + 1, idx6).toFloat();
            gripper_intensity = input.substring(idx6 + 1, idx7).toFloat();
            mission_status = input.substring(idx7 + 1).toFloat();
            
            Serial.print("dx: ");Serial.println(dx);
            Serial.print("dy: ");Serial.println(dy);
            Serial.print("dz: ");Serial.println(dz);
            Serial.print("roll: ");Serial.println(roll);
            Serial.print("pitch: ");Serial.println(pitch);
            Serial.print("yaw: ");Serial.println(yaw);
            

            //measure current servo angle — precalculation
            double currentServoAngle[6];

            for(int i = 0; i < 6; i++){
                currentServoAngle[i] = servos[i].read();
                Serial.print("Servo Angle ");Serial.print(i);Serial.print(" : ");Serial.println(currentServoAngle[i]); //print for dubug
            }

            //turn vla input into vector and multiply its magnitude by VLA_ARM_PROPORTIONAL_GAIN
            Vector2D vectorVLA = Vector2D(dx,dz).scalarMultiply(VLA_ARM_PROPORTIONAL_GAIN);

            //servo angle in rad
            double theta_2_rad = currentServoAngle[1] * M_PI / 180;
            double theta_3_rad = currentServoAngle[2] * M_PI / 180;
            double theta_4_rad = currentServoAngle[3] * M_PI / 180;

            //All current link vector calculation before CCD to determine target position
            Vector2D link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
            Vector2D link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
            Vector2D link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
            Vector2D endEffectorVector = link1.add(link2).add(link3);

            //target position in vector form from endEffector
            Vector2D targetVector = endEffectorVector.add(vectorVLA);

            Serial.print("Current End Effector Position: ( "); Serial.print(endEffectorVector.x);Serial.print(" , ") ;Serial.print(endEffectorVector.y); Serial.println(" )");
            Serial.print("Target Position: ( "); Serial.print(targetVector.x);Serial.print(" , ") ;Serial.print(targetVector.y); Serial.println(" )");
            Serial.print("Distance from End Effector to Target: ");Serial.println(vectorVLA.getMagnitude());
            //CCD initiate from root node 3 and regress to 1 and loop 
            int current_operating_rootNode = 3;

            const int STATE_1 = 1;
            const int STATE_2 = 2;
            const int STATE_3 = 3;
            
            //Cyclic Coordinate Descent Inverse Kinematic — CCD loop
            for (int i  = 0; i < CCD_ITERATION; i++){

                //re-calculation for each iteration update, i have no idea how tf to make this shit cleaner
                theta_2_rad = currentServoAngle[1] * M_PI / 180;
                theta_3_rad = currentServoAngle[2] * M_PI / 180;
                theta_4_rad = currentServoAngle[3] * M_PI / 180;
                
                link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
                link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
                link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
                endEffectorVector = link1.add(link2).add(link3);

                

                //vector from joint_i to endEffector
                Vector2D rootNode_toEndEffector_1 = endEffectorVector;
                Vector2D rootNode_toEndEffector_2  = link2.add(link3);
                Vector2D rootNode_toEndEffector_3  = link3;

                //vector from joint_i to target point
                Vector2D rootNode_toTargetPoint_1 = targetVector;
                Vector2D rootNode_toTargetPoint_2 = targetVector.minus(link1);
                Vector2D rootNode_toTargetPoint_3 = targetVector.minus( link1.add(link2) );


                //angle from joint_toEndEffector to joint_toTargetPoint — directional
                double angle_jointEndEffector_to_jointTargetPoint_1;
                double angle_jointEndEffector_to_jointTargetPoint_2;
                double angle_jointEndEffector_to_jointTargetPoint_3;

                //only operate 1 joint at a time; i know this switch state is ass im sry
                switch(current_operating_rootNode) {
                    case STATE_1:
                        angle_jointEndEffector_to_jointTargetPoint_1 = rootNode_toEndEffector_1.angleBetweenVector(rootNode_toTargetPoint_1);
                        currentServoAngle[1] += angle_jointEndEffector_to_jointTargetPoint_1 * 180 / M_PI;
                        current_operating_rootNode = STATE_3;
                        break;
                    
                    case STATE_2:
                        angle_jointEndEffector_to_jointTargetPoint_2 = rootNode_toEndEffector_2.angleBetweenVector(rootNode_toTargetPoint_2);
                        currentServoAngle[2] += angle_jointEndEffector_to_jointTargetPoint_2 * 180 / M_PI;
                        current_operating_rootNode = STATE_1;
                        break;

                    case STATE_3:
                        angle_jointEndEffector_to_jointTargetPoint_3 = rootNode_toEndEffector_3.angleBetweenVector(rootNode_toTargetPoint_3);
                        currentServoAngle[3] += angle_jointEndEffector_to_jointTargetPoint_3 * 180 / M_PI;
                        current_operating_rootNode = STATE_2;
                        break;

                }

                //clmaping servo angle between 0 and 180
                for(int i = 0; i<6 ; i++){
                    (currentServoAngle[i] > 180)? currentServoAngle[i] = 180: currentServoAngle[i];
                    (currentServoAngle[i] < 0)? currentServoAngle[i] = 0: currentServoAngle[i];
                }


            }

            //proportional dy gain as dy can only effect base servo rotation
            currentServoAngle[0] += dy * VLA_BASE_ROTATION_GAIN;

            //same shit with roll
            currentServoAngle[4] += roll * VLA_GRIPPER_ROTATION_GAIN;

            //simple grippper grab mapping logic
            currentServoAngle[5] = (1 - gripper_intensity) * 90;

            //servo write new angle
            for(int i = 0; i < 6 ; i++){
                servos[i].write(currentServoAngle[i]);
                Serial.print("New Servo Angle ");Serial.print(i);Serial.print(" : ");Serial.println(currentServoAngle[i]); //print for dubug
            }

            
            
            Serial.print("Current New Effector Position: ( "); Serial.print(endEffectorVector.x);Serial.print(" , ") ;Serial.print(endEffectorVector.y); Serial.println(" )");
        }

        else{
            Serial.println("Wrong format nigga");
        }

    }
}
