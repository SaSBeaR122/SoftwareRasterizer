#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include "Math.h"

struct Color {
    uint8_t r = 0, g = 0, b = 0;
    Color() = default;
    Color(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}
};

class Image {
public:
    int width = 0, height = 0;
    std::vector<Color> data;

    Image() = default;
    Image(int w, int h) : width(w), height(h), data(w* h) {}

    void setPixel(int x, int y, const Color& c) {
        if (x >= 0 && x < width && y >= 0 && y < height)
            data[y * width + x] = c;
    }

    Vec3 sample(float u, float v) const {
        if (data.empty()) return Vec3(0.8f, 0.8f, 0.8f); 
        u = std::clamp(u, 0.0f, 1.0f);
        v = std::clamp(v, 0.0f, 1.0f);
        int x = std::min(static_cast<int>(u * width), width - 1);
        int y = std::min(static_cast<int>(v * height), height - 1);
        Color c = data[y * width + x];
        return Vec3(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f);
    }

    bool saveTGA(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) return false;
        uint8_t header[18] = { 0 };
        header[2] = 2; 
        header[12] = width & 0xFF; header[13] = (width >> 8) & 0xFF;
        header[14] = height & 0xFF; header[15] = (height >> 8) & 0xFF;
        header[16] = 24;
        out.write(reinterpret_cast<char*>(header), 18);
        for (const auto& c : data) {
            uint8_t pixel[3] = { c.b, c.g, c.r };
            out.write(reinterpret_cast<char*>(pixel), 3);
        }
        return true;
    }

    bool saveBMP(const std::string& filename) const {
      std::ofstream out(filename, std::ios::binary);
      if (!out) return false;

      int padding = (4 - (width * 3) % 4) % 4;
      uint32_t dataSize = (width * 3 + padding) * height;
      uint32_t fileSize = 54 + dataSize;

      uint8_t fileHeader[14] = {'B', 'M'};
      fileHeader[2] = fileSize & 0xFF;
      fileHeader[3] = (fileSize >> 8) & 0xFF;
      fileHeader[4] = (fileSize >> 16) & 0xFF;
      fileHeader[5] = (fileSize >> 24) & 0xFF;
      fileHeader[10] = 54;  

      uint8_t infoHeader[40] = {0};
      infoHeader[0] = 40;  
      infoHeader[4] = width & 0xFF;
      infoHeader[5] = (width >> 8) & 0xFF;
      infoHeader[6] = (width >> 16) & 0xFF;
      infoHeader[7] = (width >> 24) & 0xFF;
      infoHeader[8] = height & 0xFF;
      infoHeader[9] = (height >> 8) & 0xFF;
      infoHeader[10] = (height >> 16) & 0xFF;
      infoHeader[11] = (height >> 24) & 0xFF;
      infoHeader[12] = 1;   
      infoHeader[14] = 24;  

      out.write(reinterpret_cast<char*>(fileHeader), 14);
      out.write(reinterpret_cast<char*>(infoHeader), 40);

      uint8_t pad[3] = {0, 0, 0};
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          const Color& c = data[y * width + x];
          uint8_t pixel[3] = {c.b, c.g, c.r};
          out.write(reinterpret_cast<char*>(pixel), 3);
        }
        if (padding > 0) out.write(reinterpret_cast<char*>(pad), padding);
      }
      return true;
    }

    bool loadTGA(const std::string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) return false;
        uint8_t header[18];
        in.read(reinterpret_cast<char*>(header), 18);
        width = header[12] | (header[13] << 8);
        height = header[14] | (header[15] << 8);
        int bpp = header[16];
        int imgType = header[2];

        if ((imgType != 2 && imgType != 10) || (bpp != 24 && bpp != 32)) {
            return false;
        }

        data.resize(width * height);
        int bytesPerPixel = bpp / 8;
        int totalPixels = width * height;
        int currentPixel = 0;

        if (imgType == 2) { 
            for (int i = 0; i < totalPixels; ++i) {
                uint8_t p[4];
                in.read(reinterpret_cast<char*>(p), bytesPerPixel);
                data[i] = Color(p[2], p[1], p[0]);
            }
        }
        else if (imgType == 10) { 
            while (currentPixel < totalPixels && in.good()) {
                uint8_t chunkHeader = 0;
                in.read(reinterpret_cast<char*>(&chunkHeader), 1);
                if (chunkHeader < 128) {
                    int count = chunkHeader + 1;
                    for (int i = 0; i < count && currentPixel < totalPixels; ++i) {
                        uint8_t p[4];
                        in.read(reinterpret_cast<char*>(p), bytesPerPixel);
                        data[currentPixel++] = Color(p[2], p[1], p[0]);
                    }
                }
                else {
                    int count = chunkHeader - 127;
                    uint8_t p[4];
                    in.read(reinterpret_cast<char*>(p), bytesPerPixel);
                    Color c(p[2], p[1], p[0]);
                    for (int i = 0; i < count && currentPixel < totalPixels; ++i) {
                        data[currentPixel++] = c;
                    }
                }
            }
        }

        if (header[17] & 0x20) {
            for (int y = 0; y < height / 2; ++y)
                for (int x = 0; x < width; ++x)
                    std::swap(data[y * width + x], data[(height - 1 - y) * width + x]);
        }
        return true;
    }
};
