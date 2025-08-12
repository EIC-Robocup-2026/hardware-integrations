#include "Vector2D.h"

#include <iostream>
#include <cmath>
using namespace std;

double l1 = 0.105; // length of link 1 controled by servo 2
double l2 = 0.125; // length of link 2 controled by servo 3
double l3 = 0.175; // length of link 3 controled by servo 4

double dx = 0.2;
double dy = 0.5;
double dz = 1;

double theta_2_rad = 20 * M_PI / 180;
double theta_3_rad = 50 * M_PI / 180;
double theta_4_rad = 30 * M_PI / 180;

double VLA_ARM_PROPORTIONAL_GAIN = 100; // proportional gain for VLA arm movement input, ADJUSTABLE — based on dx dz
double VLA_BASE_ROTATION_GAIN = 10; // proportional gain for VLA base rotation input, ADJUSTABLE — based on dy
double VLA_GRIPPER_ROTATION_GAIN = 10; // proportional gain for VLA gripper rotation input, ADJUSTABLE — based on roll

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int main() {
  
    Vector2D vectorVLA = Vector2D(dx,dz).normalize().scalarMultiply(VLA_ARM_PROPORTIONAL_GAIN); // VLA Vector adjusted from 


    //All vector links calculation
    Vector2D link1 = Vector2D( -l1*cos(theta_2_rad) , l1*sin(theta_2_rad) );
    Vector2D link2 = Vector2D( l2*cos( M_PI_2 - theta_2_rad + theta_3_rad) , l2*sin(M_PI/2 - theta_2_rad + theta_3_rad));
    Vector2D link3 = Vector2D( l3 * cos(theta_4_rad + theta_3_rad - theta_2_rad) , l3 * sin(theta_4_rad + theta_3_rad - theta_2_rad));
    Vector2D endEffectorVector = link1.add(link2).add(link3);
    
    //Vector from joint to target location
    Vector2D link1_jointToTarget = endEffectorVector.add(vectorVLA);
    Vector2D link2_jointToTarget = link1_jointToTarget.minus(link1);
    Vector2D link3_jointToTarget = link2_jointToTarget.minus(link2);

    double angle_link1_jointToTarget = link1.angleBetweenVector(link1_jointToTarget);
    double angle_link2_jointToTarget = link2.angleBetweenVector(link2_jointToTarget);
    double angle_link3_jointToTarget = link3.angleBetweenVector(link3_jointToTarget);
    
    cout << "VLA input: " << vectorVLA.x << " " << vectorVLA.y << endl;
    cout <<"Link 1: " <<link1.x << " " << link1.y << endl;
    cout <<"Link 2: " << link2.x << " " << link2.y << endl;
    cout <<"Link 3: " << link3.x << " " << link3.y << endl;
    cout <<"End effector: " << endEffectorVector.x << " " << endEffectorVector.y << endl;

    cout <<"link1_jointToTarget: " << link1_jointToTarget.x << " " << link1_jointToTarget.y << endl;
    cout <<"link2_jointToTarget: " << link2_jointToTarget.x << " " << link2_jointToTarget.y << endl;
    cout <<"link3_jointToTarget: " << link3_jointToTarget.x << " " << link3_jointToTarget.y << endl;

    cout << "angle_link1_jointToTarget: "<< angle_link1_jointToTarget  <<endl;
    cout << "angle_link2_jointToTarget: "<< angle_link2_jointToTarget <<endl;
    cout << "angle_link3_jointToTarget: "<< angle_link3_jointToTarget <<endl;
    cout << mapDouble(1.0,-1,1,180,90)<< endl;
    return 0;
}