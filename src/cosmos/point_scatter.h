#pragma once

#include <cmath>
#include <concepts>
#include <ostream>
#include <random>
#include <numbers>

/// @brief 
#define PI  std::numbers::pi


//https://codereview.stackexchange.com/questions/272800/c-vector3d-class

template<typename T>
   // requires std::floating_point<T> or std::integral<T>
class vector2d
{
public:
    T x{};
    T y{};

    T norm() const { return std::hypot(x, y); }

    vector2d unit() const { return vector2d{*this} / norm(); }

    T dot(const vector2d& rhs) const { return x * rhs.x + y * rhs.y;}

    T length() const {return sqrt(dot(*this));};

    // vector2d cross(const vector2d& rhs) const {
    //     return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x};
    // }

    bool operator==(const vector2d&) const = default;

    vector2d& operator+=(const vector2d& rhs) { x += rhs.x; y += rhs.y;  return *this; }
    vector2d& operator-=(const vector2d& rhs) { x -= rhs.x; y -= rhs.y;  return *this; }

    vector2d& operator*=(T s) { x *= s; y *= s;  return *this; }
    vector2d& operator/=(T s) { x /= s; y /= s;  return *this; }
};

template<typename T> auto operator+(const vector2d<T>& v) { return v; }
template<typename T> auto operator-(const vector2d<T>& v) { return vector2d<T>{} - v; }

template<typename T> auto operator+(vector2d<T> lhs, const vector2d<T>& rhs) { return lhs += rhs; }
template<typename T> auto operator-(vector2d<T> lhs, const vector2d<T>& rhs) { return lhs -= rhs; }

template<typename T, typename U> auto operator*(vector2d<T> v, U s) { return v *= s; }
template<typename T, typename U> auto operator*(U s, vector2d<T> v) { return v *= s; }
template<typename T, typename U> auto operator/(vector2d<T> v, U s) { return v /= s; }

template<typename T> std::ostream& operator<<(std::ostream& os, const vector2d<T>& v) {
    return os << '[' << v.x << ", " << v.y << ']';
}

template<typename T>
    requires std::floating_point<T> or std::integral<T>
class vector3d
{
public:
    T x{};
    T y{};
    T z{};

    T norm() const { return std::hypot(x, y, z); }

    vector3d unit() const { return vector3d{*this} / norm(); }

    T dot(const vector3d& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

    T length() const {return sqrt(dot(*this));};

    vector3d cross(const vector3d& rhs) const {
        return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x};
    }

    bool operator==(const vector3d&) const = default;

    vector3d& operator+=(const vector3d& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    vector3d& operator-=(const vector3d& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    vector3d& operator*=(T s) { x *= s; y *= s; z *= s; return *this; }
    vector3d& operator/=(T s) { x /= s; y /= s; z /= s; return *this; }
    vector3d rotate_around_point(const vector3d& origin, T ax, T ay, T az, bool degree) {
        // Translated from python version
        T px = this->x - origin.x;
        T py = this->y - origin.y;
        T pz = this->z - origin.z;
        // # rotation on x, y, z
        T tx = !degree ? ax: ax * (PI / 180);
        T ty = !degree ? ay: ay * (PI / 180);
        T tz = !degree ? az: az * (PI / 180);
        // # The transformation matrices.
        T rx[9] = {1, 0, 0, 0, cos(tx), -sin(tx), 0, sin(tx), cos(tx)};
        T ry[9] = {cos(ty), 0, sin(ty), 0, 1, 0, -sin(ty), 0, cos(ty)};
        T rz[9] = {cos(tz), -sin(tz), 0, sin(tz), cos(tz), 0, 0, 0, 1};
        // //# Matrix multiplication
        T rotatedX[3] = {(rx[0] * px + rx[1] * py + rx[2] * pz), (rx[3] * px + rx[4] * py + rx[5] * pz), (rx[6] * px + rx[7] * py + rx[8] * pz)};
        px = rotatedX[0];
        py = rotatedX[1];
        pz = rotatedX[2];
        T rotatedY[3] = {(ry[0] * px + ry[1] * py + ry[2] * pz), (ry[3] * px + ry[4] * py + ry[5] * pz), (ry[6] * px + ry[7] * py + ry[8] * pz)};
        px = rotatedY[0];
        py = rotatedY[1];
        pz = rotatedY[2];
        T rotatedZ[3] = {(rz[0] * px + rz[1] * py + rz[2] * pz), (rz[3] * px + rz[4] * py + rz[5] * pz), (rz[6] * px + rz[7] * py + rz[8] * pz)};
        px = rotatedZ[0];
        py = rotatedZ[1];
        pz = rotatedZ[2];
        return vector3d(px + origin.x, py + origin.y, pz + origin.z);
    }
};

template<typename T> auto operator+(const vector3d<T>& v) { return v; }
template<typename T> auto operator-(const vector3d<T>& v) { return vector3d<T>{} - v; }

template<typename T> auto operator+(vector3d<T> lhs, const vector3d<T>& rhs) { return lhs += rhs; }
template<typename T> auto operator-(vector3d<T> lhs, const vector3d<T>& rhs) { return lhs -= rhs; }

template<typename T, typename U> auto operator*(vector3d<T> v, U s) { return v *= s; }
template<typename T, typename U> auto operator*(U s, vector3d<T> v) { return v *= s; }
template<typename T, typename U> auto operator/(vector3d<T> v, U s) { return v /= s; }

template<typename T> auto& operator<<(std::ostream& os, const vector3d<T>& v) {
    return os << '[' << v.x << ", " << v.y << ", " << v.z << ']';
}


template <typename T>
class PointScatter3d {
    T width{};
    T height{};
    T depth{};
    T center_x{};
    T center_y{};
    T center_z{};
    std::default_random_engine eng;
    long unsigned int seed;
    std::uniform_real_distribution<T> width_dist;
    std::uniform_real_distribution<T> height_dist;
    std::uniform_real_distribution<T> depth_dist;
public:
    T get_width() {return width;};
    T get_height() {return height;};
    T get_depth() {return depth;};
    T get_center_x() {return center_x;};
    T get_center_y() {return center_y;};
    T get_center_z() {return center_z;};

public:
    PointScatter3d() {}
    PointScatter3d(T cx, T cy, T cz, T width, T height, T depth, long unsigned int seed=0) {
        if (seed == 0) this->seed = static_cast<long unsigned int>(time(0));
        else this->seed = seed;
        center_x = cx;
        center_y = cy;
        center_z = cz;
        this->width = width;
        this->height = height;
        this->depth = depth;


        eng = std::default_random_engine {seed};
        width_dist = std::uniform_real_distribution<T> (cx-width/2, cx+width/2);
        height_dist = std::uniform_real_distribution<T> (cy - height/2, cy+height/2);
        depth_dist = std::uniform_real_distribution<T> (cz - depth/2,cz+depth/2);
    }
    PointScatter3d(PointScatter3d& rhs) {
        this->width = rhs.width;
        this->height = rhs.height;
        this->depth = rhs.depth;
        this->eng = rhs.eng;
        this->seed = rhs.seed;
        this->width_dist = rhs.width_dist;
        this->height_dist = rhs.height_dist;
        this->depth_dist = rhs.depth_dist;
    }
    
    vector3d<T> next() {
        T x = width_dist(eng);
        T y = height_dist(eng);
        T z = depth_dist(eng);
        return vector3d<T>{x,y,z};
    }

};