#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>

// Configuration for Windows
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

struct Sprite3D {
    std::vector<std::string> lines;
    float width = 0;
    float height = 0;
    Color baseColor = WHITE; // Default color

    void Load(const std::string& filepath, Color color) {
        lines.clear();
        baseColor = color;
        std::ifstream file(filepath);
        std::string line;
        width = 0;
        while (std::getline(file, line)) {
            lines.push_back(line);
            if (line.length() > width) width = (float)line.length();
        }
        height = (float)lines.size();
    }
};

class AsciiFramework {
private:
    // Projection Constants
    const float FOCAL_LENGTH = 600.0f;
    RenderTexture2D target; // For CRT Effect

    struct Point2D { float x, y, scale; };

    Point2D Project(float x, float y, float z) {
        float scale = FOCAL_LENGTH / (FOCAL_LENGTH + z);
        float cx = WINDOW_WIDTH / 2.0f;
        float cy = WINDOW_HEIGHT / 2.0f;
        return { cx + (x * scale), cy + (y * scale), scale };
    }

public:
    void Init(const char* title) {
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title);
        SetTargetFPS(60);
        target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
        // Hide Cursor for game feel
        HideCursor();
    }

    void Close() {
        UnloadRenderTexture(target);
        CloseWindow();
    }

    bool Running() { return !WindowShouldClose(); }

    void BeginFrame() {
        BeginTextureMode(target);
        ClearBackground({5, 5, 5, 255}); // #050505 Background
    }

    void EndFrame() {
        EndTextureMode();
        
        BeginDrawing();
            // Draw the game to screen
            DrawTexturePro(target.texture, 
                {0, 0, (float)target.texture.width, (float)-target.texture.height},
                {0, 0, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT}, 
                {0, 0}, 0.0f, WHITE);

            // --- CRT SCANLINE EFFECT ---
            // Draw faint black lines every 2 pixels
            for (int y = 0; y < WINDOW_HEIGHT; y += 2) {
                DrawRectangle(0, y, WINDOW_WIDTH, 1, {0, 0, 0, 50}); 
            }
            
            // Subtle Flicker
            if (GetRandomValue(0, 100) > 95) {
                DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {0, 0, 0, 10});
            }
        EndDrawing();
    }

    // --- Drawing Tools ---

    void DrawSprite3D(const Sprite3D& sprite, float x, float y, float z, float scaleMult = 1.0f) {
        Point2D p = Project(x, y, z);
        if (p.scale <= 0) return;

        // Dynamic Font Size based on depth
        float fontSize = 20.0f * p.scale * scaleMult;
        if (fontSize < 2) return;

        float charW = fontSize * 0.6f; 
        float charH = fontSize * 0.8f;
        float totalW = sprite.width * charW;
        float totalH = sprite.height * charH;

        // Centered Draw
        float startX = p.x - (totalW / 2);
        float startY = p.y - (totalH / 2);

        for (size_t i = 0; i < sprite.lines.size(); i++) {
            DrawText(sprite.lines[i].c_str(), 
                (int)startX, 
                (int)(startY + (i * charH)), 
                (int)fontSize, 
                sprite.baseColor);
        }
    }

    void DrawRetroLine(float x1, float y1, float z1, float x2, float y2, float z2, Color color) {
        Point2D p1 = Project(x1, y1, z1);
        Point2D p2 = Project(x2, y2, z2);

        if (p1.scale <= 0 || p2.scale <= 0) return;

        DrawLineEx({p1.x, p1.y}, {p2.x, p2.y}, 2.0f * p1.scale, color);
    }
    
    // Input Helpers
    Vector2 GetMouseDelta() { return ::GetMouseDelta(); }
    bool IsClick() { return IsMouseButtonPressed(MOUSE_LEFT_BUTTON); }
};