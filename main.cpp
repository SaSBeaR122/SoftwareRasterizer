#include <clocale>
#include <iostream>
#include <limits>

#include "Camera.h"
#include "Image.h"
#include "Math.h"
#include "Mesh.h"
#include "Rasterizer.h"
#include "Shader.h"

int main(int argc, char** argv) {
  setlocale(LC_ALL, "Russian");

  const int WIDTH = 800;
  const int HEIGHT = 800;

  Image framebuffer(WIDTH, HEIGHT);
  std::vector<float> zbuffer(WIDTH * HEIGHT, std::numeric_limits<float>::max());

  std::string objPath = (argc > 1) ? argv[1] : "african_head.obj";
  Mesh mesh;
  if (!mesh.loadOBJ(objPath)) {
    std::cout << "Файл " << objPath
              << " не найден. Генерируем тестовую пирамиду...\n";
    Vec3 top(0, 0.8f, 0), fl(-0.7f, -0.6f, 0.7f), fr(0.7f, -0.6f, 0.7f),
        b(0, -0.6f, -0.7f);
    auto addFace = [&](Vec3 p0, Vec3 p1, Vec3 p2) {
      Vec3 n = (p1 - p0).cross(p2 - p0).normalize();
      mesh.vertices.push_back({p0, {0, 0}, n});
      mesh.vertices.push_back({p1, {1, 0}, n});
      mesh.vertices.push_back({p2, {0.5f, 1}, n});
    };
    addFace(top, fl, fr);
    addFace(top, fr, b);
    addFace(top, b, fl);
  } else {
    std::cout << "Модель успешно загружена: " << mesh.vertices.size() / 3
              << " треугольников.\n";
  }

  Image texture;
  std::string texPath = (argc > 2) ? argv[2] : "african_head_diffuse.tga";
  if (texture.loadTGA(texPath)) {
    std::cout << "Текстура загружена: " << texPath << "\n";
  } else {
    std::cout << "Текстура " << texPath
              << " не найдена, рендерим с базовым цветом по Фонгу.\n";
  }

  Camera camera(Vec3(0.0f, 0.0f, 3.0f), Vec3(0.0f, 0.0f, 0.0f),
                Vec3(0.0f, 1.0f, 0.0f), 45.0f,
                static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);

  PhongShader shader;
  shader.modelMatrix = Mat4::identity();
  shader.viewProjMatrix = camera.getProjectionMatrix() * camera.getViewMatrix();
  shader.lightPos = Vec3(2.0f, 2.0f, 3.0f);
  shader.cameraPos = camera.position;
  shader.diffuseTexture = texture.data.empty() ? nullptr : &texture;

  for (size_t i = 0; i + 2 < mesh.vertices.size(); i += 3) {
    Vec4 clipCoords[3];
    for (int j = 0; j < 3; ++j) {
      clipCoords[j] = shader.vertex(mesh.vertices[i + j], j);
    }
    drawTriangle(clipCoords, shader, framebuffer, zbuffer);
  }

  framebuffer.saveTGA("output.tga");
  std::cout << "Рендер завершен! Результат сохранен в output.tga\n";
  return 0;
}