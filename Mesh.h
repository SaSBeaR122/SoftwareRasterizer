#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Math.h"

struct Vertex {
    Vec3 pos;
    Vec2 uv;
    Vec3 normal;
};

struct Mesh {
    std::vector<Vertex> vertices; 

    bool loadOBJ(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) return false;

        std::vector<Vec3> temp_pos;
        std::vector<Vec2> temp_uv;
        std::vector<Vec3> temp_norm;

        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;
            if (prefix == "v") {
                Vec3 p; iss >> p.x >> p.y >> p.z;
                temp_pos.push_back(p);
            }
            else if (prefix == "vt") {
                Vec2 uv; iss >> uv.x >> uv.y;
                temp_uv.push_back(uv);
            }
            else if (prefix == "vn") {
                Vec3 n; iss >> n.x >> n.y >> n.z;
                temp_norm.push_back(n);
            }
            else if (prefix == "f") {
                std::vector<Vertex> face;
                std::string vertex_str;
                while (iss >> vertex_str) {
                    std::istringstream viss(vertex_str);
                    std::string idx_str;
                    int v_idx = 0, vt_idx = 0, vn_idx = 0;

                    if (std::getline(viss, idx_str, '/') && !idx_str.empty()) v_idx = std::stoi(idx_str);
                    if (std::getline(viss, idx_str, '/') && !idx_str.empty()) vt_idx = std::stoi(idx_str);
                    if (std::getline(viss, idx_str, '/') && !idx_str.empty()) vn_idx = std::stoi(idx_str);

                    Vertex vert;
                    if (v_idx > 0) vert.pos = temp_pos[v_idx - 1];
                    if (vt_idx > 0) vert.uv = temp_uv[vt_idx - 1];
                    if (vn_idx > 0) vert.normal = temp_norm[vn_idx - 1];
                    face.push_back(vert);
                }
                for (size_t i = 1; i + 1 < face.size(); ++i) {
                    if (temp_norm.empty()) {
                        Vec3 n = (face[i].pos - face[0].pos).cross(face[i + 1].pos - face[0].pos).normalize();
                        face[0].normal = face[i].normal = face[i + 1].normal = n;
                    }
                    vertices.push_back(face[0]);
                    vertices.push_back(face[i]);
                    vertices.push_back(face[i + 1]);
                }
            }
        }
        return !vertices.empty();
    }
};
