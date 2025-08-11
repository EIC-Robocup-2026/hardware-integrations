#include "C:\Users\Admin\Desktop\hardware-integrations\header\Vector2D.h"
#include <cmath>
#include <iostream>

using namespace std;


Vector2D::Vector2D(double X, double Y){
    x = X;
    y = Y;
}


double Vector2D::getMagnitude() const{
    return(sqrt(x*x + y*y));
}

Vector2D Vector2D::add(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::minus(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::scalarDivide(const double other) const {
    return Vector2D(x / other, y / other);
}

Vector2D Vector2D::scalarMultiply(const double other) const{
    return Vector2D(x * other, y * other);
}

double dotProduct(const Vector2D& v1, const Vector2D& v2){
    return v1.x * v2.x + v1.y * v2.y;
}

Vector2D Vector2D::normalize() const{
    double mag = getMagnitude();
    if (mag == 0) {
        return Vector2D(0, 0); // avoid division by zero
    }
    return Vector2D(x/mag , y/mag);
}

double Vector2D::angleBetweenVector(const Vector2D& w){
    return atan2(x * w.y - y * w.x, x * w.x +   y * w.y);
}

//test
// int main(){
//     Vector2D vec1 = Vector2D(3,4);
//     Vector2D vec2 = Vector2D(-5,8);

//     cout << vec2.angleBetweenVector(vec1) * 180 / M_PI << endl;
//     return 0; 
// }