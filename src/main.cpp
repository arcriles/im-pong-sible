#include "../engine/AsciiFramework.h"

// Game Constants
const float COURT_W = 800;
const float COURT_H = 500;
const float COURT_D = 1000;
const float PADDLE_W = 100;
const float PADDLE_H = 60;
const float BALL_RADIUS = 15;

// Global State
struct {
    Vector3 ball = {0, 0, 20};
    Vector3 ballVel = {0, 0, 0};
    Vector2 player = {0, 0};
    Vector2 cpu = {0, 0};
    int playerScore = 0;
    int cpuScore = 0;
    
    bool waitingForServe = true;
    bool serverIsPlayer = true;
    int cpuServeTimer = 0;
    
    // Mouse momentum tracking
    Vector2 mouseVel = {0, 0}; 
} state;

int main() {
    AsciiFramework engine;
    engine.Init("3D ASCII Pong - Windows Edition");

    // --- Load Assets & Set Colors ---
    Sprite3D paddleArt;
    paddleArt.Load("assets/paddle.txt", WHITE); // Classic White

    Sprite3D ballArt;
    ballArt.Load("assets/ball.txt", RED);       // Classic Red

    Sprite3D cpuArt;
    cpuArt.Load("assets/paddle.txt", WHITE);    // CPU is also White

    while (engine.Running()) {
        float dt = GetFrameTime();
        float timeScale = dt * 60.0f; // Normalize to 60 FPS speed

        // --- 1. INPUT & PLAYER MOVEMENT ---
        Vector2 mouseDelta = engine.GetMouseDelta();
        
        // Track momentum for serve
        state.mouseVel.x = mouseDelta.x;
        state.mouseVel.y = mouseDelta.y;

        state.player.x += mouseDelta.x;
        state.player.y += mouseDelta.y;

        // Clamp Player
        state.player.x = std::clamp(state.player.x, -COURT_W/2 + PADDLE_W/2, COURT_W/2 - PADDLE_W/2);
        state.player.y = std::clamp(state.player.y, -COURT_H/2 + PADDLE_H/2, COURT_H/2 - PADDLE_H/2);

        // --- 2. GAME LOGIC ---
        if (state.waitingForServe) {
            // Ball Attached to Server
            if (state.serverIsPlayer) {
                state.ball = {state.player.x, state.player.y, 20};
                
                // CLICK TO SERVE
                if (engine.IsClick()) {
                    state.waitingForServe = false;
                    state.ballVel.z = 14.0f; // Medium Speed
                    // Add Mouse Momentum to Serve
                    state.ballVel.x = state.mouseVel.x * 0.5f;
                    state.ballVel.y = state.mouseVel.y * 0.5f;
                }
            } else {
                // CPU Serve
                state.ball = {state.cpu.x, state.cpu.y, COURT_D - 20};
                state.cpuServeTimer++;
                
                // CPU "Wind up" shake
                if (state.cpuServeTimer > 30) {
                    state.cpu.x += (float)GetRandomValue(-5, 5);
                    state.cpu.y += (float)GetRandomValue(-5, 5);
                }

                if (state.cpuServeTimer > 60) {
                    state.waitingForServe = false;
                    state.ballVel.z = -14.0f;
                    // Random Trick Serve
                    state.ballVel.x = (float)GetRandomValue(-10, 10);
                    state.ballVel.y = (float)GetRandomValue(-5, 5);
                    state.cpuServeTimer = 0;
                }
            }
        } else {
            // Ball Physics
            state.ball.x += state.ballVel.x * timeScale;
            state.ball.y += state.ballVel.y * timeScale;
            state.ball.z += state.ballVel.z * timeScale;

            // Wall Bounces
            if (state.ball.x > COURT_W/2 || state.ball.x < -COURT_W/2) state.ballVel.x *= -1;
            if (state.ball.y > COURT_H/2 || state.ball.y < -COURT_H/2) state.ballVel.y *= -1;

            // Player Collision (Z < 20)
            if (state.ball.z < 20 && state.ball.z > -20 && state.ballVel.z < 0) {
                if (fabs(state.ball.x - state.player.x) < PADDLE_W/2 + BALL_RADIUS && 
                    fabs(state.ball.y - state.player.y) < PADDLE_H/2 + BALL_RADIUS) {
                    
                    state.ballVel.z *= -1.05f; // Speed up
                    // English (Spin)
                    state.ballVel.x += (state.ball.x - state.player.x) * 0.1f;
                    state.ballVel.y += (state.ball.y - state.player.y) * 0.1f;
                }
            }

            // CPU Collision (Z > Depth)
            if (state.ball.z > COURT_D - 20 && state.ball.z < COURT_D + 20 && state.ballVel.z > 0) {
                if (fabs(state.ball.x - state.cpu.x) < PADDLE_W/2 + BALL_RADIUS && 
                    fabs(state.ball.y - state.cpu.y) < PADDLE_H/2 + BALL_RADIUS) {
                    state.ballVel.z *= -1.05f;
                }
            }

            // Scoring
            if (state.ball.z < -100) {
                state.cpuScore++;
                state.serverIsPlayer = false; // Loser receives
                state.waitingForServe = true;
            } else if (state.ball.z > COURT_D + 100) {
                state.playerScore++;
                state.serverIsPlayer = true;
                state.waitingForServe = true;
            }
        }

        // --- 3. CPU AI ---
        float cpuSpeed = 10.0f * timeScale;
        float reactionZone = 30.0f; // Reaction delay

        if (state.ballVel.z > 0) { // Only move if ball is coming
            if (state.cpu.x < state.ball.x - reactionZone) state.cpu.x += cpuSpeed;
            else if (state.cpu.x > state.ball.x + reactionZone) state.cpu.x -= cpuSpeed;
            
            if (state.cpu.y < state.ball.y - reactionZone) state.cpu.y += cpuSpeed;
            else if (state.cpu.y > state.ball.y + reactionZone) state.cpu.y -= cpuSpeed;
        }

        // --- 4. RENDER ---
        engine.BeginFrame();

            // Draw Court
            Color gridColor = {50, 50, 50, 255}; // #333333
            float w = COURT_W/2, h = COURT_H/2, d = COURT_D;
            
            engine.DrawRetroLine(-w, -h, 0, -w, -h, d, WHITE); 
            engine.DrawRetroLine(w, -h, 0, w, -h, d, WHITE);
            engine.DrawRetroLine(-w, h, 0, -w, h, d, WHITE);
            engine.DrawRetroLine(w, h, 0, w, h, d, WHITE);
            engine.DrawRetroLine(-w, -h, d, w, -h, d, WHITE); // Back Wall
            engine.DrawRetroLine(w, -h, d, w, h, d, WHITE);
            engine.DrawRetroLine(w, h, d, -w, h, d, WHITE);
            engine.DrawRetroLine(-w, h, d, -w, -h, d, WHITE);

            // Draw Grid (Floor)
            for(int z=0; z<=d; z+=200) engine.DrawRetroLine(-w, h, (float)z, w, h, (float)z, gridColor);
            
            // Draw Entities (Sort by Z is automatic if we draw back-to-front manually)
            // 1. CPU
            engine.DrawSprite3D(cpuArt, state.cpu.x, state.cpu.y, COURT_D, 4.0f);
            
            // 2. Ball
            engine.DrawSprite3D(ballArt, state.ball.x, state.ball.y, state.ball.z, 3.0f);
            
            // 3. Player
            engine.DrawSprite3D(paddleArt, state.player.x, state.player.y, 0, 4.0f);

            // UI
            DrawText(TextFormat("%i  -  %i", state.playerScore, state.cpuScore), WINDOW_WIDTH/2 - 60, 50, 50, WHITE);
            
            if (state.waitingForServe && state.serverIsPlayer) {
                DrawText("CLICK TO SERVE", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 100, 30, GRAY);
            }

        engine.EndFrame();
    }

    engine.Close();
    return 0;
}