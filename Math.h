#pragma once
#include <cmath>

struct Vec2 {
    float x = 0, y = 0;
    Vec2() = default;
    Vec2(float _x, float _y) : x(_x), y(_y) {}
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
};

struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    Vec3 normalize() const {
        float len = std::sqrt(x * x + y * y + z * z);
        return (len > 1e-6f) ? (*this) / len : Vec3(0, 0, 0);
    }
    Vec3 cwiseProduct(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
};

struct Vec4 {
    float x = 0, y = 0, z = 0, w = 1;
    Vec4() = default;
    Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4(const Vec3& v, float _w = 1.0f) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Vec3 toVec3() const { return Vec3(x, y, z); }
};

struct Mat4 {
    float m[4][4] = { {0} };
    static Mat4 identity() {
        Mat4 res;
        for (int i = 0; i < 4; ++i) res.m[i][i] = 1.0f;
        return res;
    }
    Mat4 operator*(const Mat4& o) const {
        Mat4 res;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k)
                    res.m[i][j] += m[i][k] * o.m[k][j];
        return res;
    }
    Vec4 operator*(const Vec4& v) const {
        return Vec4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }
};
