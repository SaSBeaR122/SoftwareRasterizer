#pragma once
#include <vector>
#include <algorithm>
#include "Math.h"
#include "Shader.h"
#include "Image.h"

inline Vec3 computeBarycentric(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& P) {
    Vec3 s0(C.x - A.x, B.x - A.x, A.x - P.x);
    Vec3 s1(C.y - A.y, B.y - A.y, A.y - P.y);
    Vec3 u = s0.cross(s1);
    if (std::abs(u.z) < 1e-2f) return Vec3(-1.0f, 1.0f, 1.0f); 
    return Vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

inline void drawTriangle(const Vec4 clipCoords[3], const PhongShader& shader, Image& framebuffer, std::vector<float>& zbuffer) {
    Vec3 ndc[3];
    Vec2 screen[3];

    for (int i = 0; i < 3; ++i) {
        ndc[i] = clipCoords[i].toVec3() / clipCoords[i].w;
        screen[i].x = (ndc[i].x + 1.0f) * 0.5f * framebuffer.width;
        screen[i].y = (ndc[i].y + 1.0f) * 0.5f * framebuffer.height;
    }

    int minX = std::max(0, static_cast<int>(std::min({ screen[0].x, screen[1].x, screen[2].x })));
    int maxX = std::min(framebuffer.width - 1, static_cast<int>(std::ceil(std::max({ screen[0].x, screen[1].x, screen[2].x }))));
    int minY = std::max(0, static_cast<int>(std::min({ screen[0].y, screen[1].y, screen[2].y })));
    int maxY = std::min(framebuffer.height - 1, static_cast<int>(std::ceil(std::max({ screen[0].y, screen[1].y, screen[2].y }))));

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Vec3 bc_screen = computeBarycentric(screen[0], screen[1], screen[2], Vec2(x + 0.5f, y + 0.5f));
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;

            Vec3 bc_clip(bc_screen.x / clipCoords[0].w, bc_screen.y / clipCoords[1].w, bc_screen.z / clipCoords[2].w);
            bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);

            float z = ndc[0].z * bc_screen.x + ndc[1].z * bc_screen.y + ndc[2].z * bc_screen.z;
            int idx = y * framebuffer.width + x;

            if (z < zbuffer[idx]) {
                zbuffer[idx] = z;
                framebuffer.setPixel(x, y, shader.fragment(bc_clip));
            }
        }
    }
}
