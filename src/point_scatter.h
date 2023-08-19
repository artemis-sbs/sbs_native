#pragma once

#include <cmath>
#include <concepts>
#include <ostream>
#include <random>




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
    //requires std::floating_point<T> or std::integral<T>
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