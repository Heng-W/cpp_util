#ifndef UTIL_TYPE_3D_H
#define UTIL_TYPE_3D_H

#include <cmath>
#include <array>
#include <vector>
#include <iostream>

#include "matrix.hpp"

namespace util
{

constexpr double PI = 3.14159265359;

template <class T>
struct Point3_
{
    friend std::ostream& operator <<(std::ostream& out, const Point3_& p)
    {
        out << p.x << " " << p.y << " " << p.z;
        return out;
    }
    friend std::istream& operator >>(std::istream& in, Point3_& p)
    {
        in >> p.x >> p.y >> p.z;
        return in;
    }

    union
    {
        struct
        {
            T x, y, z;
        };
        std::array<T, 3> point;
        Matrix<T, 3, 1> mat;
    };

    Point3_(): x(0), y(0), z(0) {}
    Point3_(T _x, T _y, T _z): x(_x), y(_y), z(_z) {}
    Point3_(const std::array<T, 3>& _point): point(_point) {}
    Point3_(const Matrix<T, 3, 1>& _mat): mat(_mat) {}

    T& operator[](int i) { return point[i]; }
    const T& operator[](int i) const { return point[i]; }

    Point3_<double> rotateByAxis(const Point3_<double>& axis, double angle) const;
};

using Point3i = Point3_<int>;
using Point3f = Point3_<float>;
using Point3d = Point3_<double>;

using Vec3f = Point3f;
using Vec3d = Point3d;

using PointCloud = std::vector<Point3f>;


//欧拉角
template <class T>
struct Attitude_
{
    friend std::ostream& operator <<(std::ostream& out, const Attitude_& att)
    {
        out << att.yaw << " " << att.pitch << " " << att.roll;
        return out;
    }
    friend std::istream& operator >>(std::istream& in, Attitude_& att)
    {
        in >> att.yaw >> att.pitch >> att.roll;
        return in;
    }

    T yaw = 0; // 右偏为正
    T pitch = 0; // 抬头为正
    T roll = 0; // 右滚为正
};

using Attitude = Attitude_<double>;


struct State3D
{
    friend std::ostream& operator <<(std::ostream& out, const State3D& state)
    {
        out << "position: " << state.pos << std::endl;
        out << "euler_angles: " << state.attitude;
        return out;
    }
    friend std::istream& operator >>(std::istream& in, State3D& state)
    {
        std::string tmp;
        in >> tmp >> state.pos >> tmp >> state.attitude;
        return in;
    }

    Point3d pos;
    Attitude attitude;
};


inline Vec3d normalize(const Vec3d& v)
{
    double norm = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / norm, v.y / norm, v.z / norm};
}

inline Matrix3d rotationMatrix(const Vec3d& axis, double angle)
{
    auto v = ::normalize(axis);

    double S = std::sin(angle);
    double C = std::cos(angle);

    double XX = v.x * v.x;
    double YY = v.y * v.y;
    double ZZ = v.z * v.z;

    double XY = v.x * v.y;
    double YZ = v.y * v.z;
    double ZX = v.z * v.x;

    double XS = v.x * S;
    double YS = v.y * S;
    double ZS = v.z * S;

    double OMC = 1 - C;

    return
    {
        OMC* XX + C, OMC* XY - ZS, OMC* ZX + YS,
        OMC* XY + ZS, OMC* YY + C, OMC* YZ - XS,
        OMC* ZX - YS, OMC* YZ + XS, OMC* ZZ + C
    };
}

template <class T>
inline Vec3d Point3_<T>::rotateByAxis(const Vec3d& axis, double angle) const
{
    return rotationMatrix(axis, angle) * this->mat;
}

inline double angleBetweenLine(const Vec3d& a, const Vec3d& b)
{
    double aa = a.x * a.x + a.y * a.y + a.z * a.z;
    double bb = b.x * b.x + b.y * b.y + b.z * b.z;
    double delta = (a.x * b.x + a.y * b.y + a.z * b.z) / std::sqrt(aa) / std::sqrt(bb);
    return std::acos(delta);
}

} // namespace util

#endif // UTIL_TYPE_3D_H
