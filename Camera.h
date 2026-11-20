#pragma once
#include "Math.h"
#include <cmath>

class Camera {
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float fovY;
    float aspect;
    float zNear, zFar;

    Camera(Vec3 pos, Vec3 tgt, Vec3 _up, float fov_deg, float asp, float zn, float zf)
        : position(pos), target(tgt), up(_up), fovY(fov_deg * 3.14159265f / 180.0f),
        aspect(asp), zNear(zn), zFar(zf) {
    }

    Mat4 getViewMatrix() const {
        Vec3 zaxis = (position - target).normalize();
        Vec3 xaxis = up.cross(zaxis).normalize();
        Vec3 yaxis = zaxis.cross(xaxis);

        Mat4 view = Mat4::identity();
        view.m[0][0] = xaxis.x; view.m[0][1] = xaxis.y; view.m[0][2] = xaxis.z; view.m[0][3] = -xaxis.dot(position);
        view.m[1][0] = yaxis.x; view.m[1][1] = yaxis.y; view.m[1][2] = yaxis.z; view.m[1][3] = -yaxis.dot(position);
        view.m[2][0] = zaxis.x; view.m[2][1] = zaxis.y; view.m[2][2] = zaxis.z; view.m[2][3] = -zaxis.dot(position);
        return view;
    }

    Mat4 getProjectionMatrix() const {
        Mat4 proj;
        float tanHalfFov = std::tan(fovY / 2.0f);
        proj.m[0][0] = 1.0f / (aspect * tanHalfFov);
        proj.m[1][1] = 1.0f / tanHalfFov;
        proj.m[2][2] = -(zFar + zNear) / (zFar - zNear);
        proj.m[2][3] = -(2.0f * zFar * zNear) / (zFar - zNear);
        proj.m[3][2] = -1.0f;
        return proj;
    }
};
