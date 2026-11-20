#pragma once
#include "Math.h"
#include "Mesh.h"
#include "Image.h"
#include "Camera.h"
#include <algorithm>

struct PhongShader {
    Mat4 modelMatrix;
    Mat4 viewProjMatrix;
    Vec3 lightPos;
    Vec3 cameraPos;
    const Image* diffuseTexture = nullptr;

    Vec3 varying_worldPos[3];
    Vec3 varying_normal[3];
    Vec2 varying_uv[3];

    Vec4 vertex(const Vertex& inVert, int vertIndex) {
        Vec4 worldPos = modelMatrix * Vec4(inVert.pos, 1.0f);
        varying_worldPos[vertIndex] = worldPos.toVec3();
        varying_normal[vertIndex] = (modelMatrix * Vec4(inVert.normal, 0.0f)).toVec3().normalize();
        varying_uv[vertIndex] = inVert.uv;
        return viewProjMatrix * worldPos;
    }

    Color fragment(const Vec3& bar) const {
        Vec3 worldPos = varying_worldPos[0] * bar.x + varying_worldPos[1] * bar.y + varying_worldPos[2] * bar.z;
        Vec3 N = (varying_normal[0] * bar.x + varying_normal[1] * bar.y + varying_normal[2] * bar.z).normalize();
        Vec2 uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;

        Vec3 L = (lightPos - worldPos).normalize();
        Vec3 V = (cameraPos - worldPos).normalize();
        Vec3 R = (N * (2.0f * N.dot(L)) - L).normalize();

        Vec3 baseColor = diffuseTexture ? diffuseTexture->sample(uv.x, uv.y) : Vec3(0.8f, 0.7f, 0.6f);

        Vec3 ambient = baseColor * 0.15f;

        float diff = std::max(0.0f, N.dot(L));
        Vec3 diffuse = baseColor * diff * 0.85f;

        float spec = std::pow(std::max(0.0f, R.dot(V)), 32.0f);
        Vec3 specular = Vec3(1.0f, 1.0f, 1.0f) * spec * 0.3f;

        Vec3 finalColor = ambient + diffuse + specular;
        return Color(
            static_cast<uint8_t>(std::clamp(finalColor.x, 0.0f, 1.0f) * 255),
            static_cast<uint8_t>(std::clamp(finalColor.y, 0.0f, 1.0f) * 255),
            static_cast<uint8_t>(std::clamp(finalColor.z, 0.0f, 1.0f) * 255)
        );
    }
};
