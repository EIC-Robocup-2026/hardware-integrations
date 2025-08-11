#ifndef Vector2D_H_
#define Vector2D_H_

class Vector2D {
public:
    double x;
    double y;
    
    Vector2D(double X, double Y);
    double getMagnitude() const;

    //Vector Operation
    Vector2D add(const Vector2D& other) const;

    Vector2D minus(const Vector2D& other) const;

    Vector2D scalarDivide(const double other) const;

    Vector2D scalarMultiply(const double other) const;

    double dotProduct(const Vector2D& v1, const Vector2D& v2);

    Vector2D normalize() const;

    double angleBetweenVector(const Vector2D& w);

    //no cross product cause that shit return 3D vector
};

#endif 