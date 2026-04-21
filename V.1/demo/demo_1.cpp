#include "AnimByte.cpp"
#include <cmath>
#include <ctime>
#include <vector>

int main() {
    AnimByte anim;
    const int WIDTH = 210;
    const int HEIGHT = 54;
    const float PI = 3.14159265358979323846;
    
    anim.Initialise(WIDTH, HEIGHT);
    
    while (true) {
        system("clear");
        std::cout << "\033[H\033[2J";
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                              ANIMBYTE ANIMATION TEST SUITE - 54x210                                                       ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
        std::cout << "║  1. Spiral Wave Pattern          - Hypnotic spiral animation                                                                       30+ seconds           ║\n";
        std::cout << "║  2. Matrix Digital Rain          - Falling characters with trail effects                                                           30+ seconds           ║\n";
        std::cout << "║  3. Bouncing Ball Physics        - Multiple balls with collision physics                                                           30+ seconds           ║\n";
        std::cout << "║  4. Fire Effect Simulation       - Procedural fire animation                                                                       30+ seconds           ║\n";
        std::cout << "║  5. Starfield Warp Speed         - 3D perspective starfield                                                                        30+ seconds           ║\n";
        std::cout << "║  6. Conway's Game of Life        - Cellular automata                                                                               30+ seconds           ║\n";
        std::cout << "║  7. Plasma Cloud Effect          - Plasma fractal animation                                                                        30+ seconds           ║\n";
        std::cout << "║  8. Clock with Sweeping Hands    - Analog clock face                                                                               30+ seconds           ║\n";
        std::cout << "║  9. Sine Wave Oscilloscope       - Multiple overlapping sine waves                                                                 30+ seconds           ║\n";
        std::cout << "║ 10. Particle Explosion System    - Fireworks and particle effects                                                                  30+ seconds           ║\n";
        std::cout << "║  0. Exit                                                                                                                                                      ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "\nSelect animation (0-10): ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 0) break;
        
        for (int i = 1; i <= HEIGHT; i++)
            for (int j = 1; j <= WIDTH; j++)
                anim.Set_Char(i, j, ' ');
        
        time_t startTime = ::time(NULL);
        int frameCount = 0;
        
        switch(choice) {
            case 1: { // Spiral Wave
                float angle = 0;
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++) {
                        for (int j = 1; j <= WIDTH; j++) {
                            float dx = j - WIDTH/2.0f;
                            float dy = i - HEIGHT/2.0f;
                            float dist = sqrt(dx*dx + dy*dy);
                            float val = sin(dist * 0.15f - angle) * cos(atan2(dy, dx) * 3 + angle * 0.5f);
                            const char* chars = " .:-=+*#%@";
                            int idx = (int)((val + 1.0f) * 4.5f);
                            if (idx < 0) idx = 0;
                            if (idx > 9) idx = 9;
                            anim.Set_Char(i, j, chars[idx]);
                        }
                    }
                    anim.Render_Frame();
                    angle += 0.15f;
                    usleep(50000);
                    frameCount++;
                }
                break;
            }
            
            case 2: { // Matrix Rain
                std::vector<int> drops(WIDTH, 0);
                for (int j = 0; j < WIDTH; j++) drops[j] = rand() % HEIGHT;
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++)
                            anim.Set_Char(i, j, ' ');
                    
                    for (int j = 0; j < WIDTH; j += 2) {
                        for (int k = 0; k < 8; k++) {
                            int y = drops[j] - k;
                            if (y >= 1 && y <= HEIGHT) {
                                anim.Set_Char(y, j+1, (rand() % 94) + 33);
                            }
                        }
                        drops[j] += 1 + rand() % 3;
                        if (drops[j] > HEIGHT + 8) drops[j] = 0;
                    }
                    anim.Render_Frame();
                    usleep(80000);
                    frameCount++;
                }
                break;
            }
            
            case 3: { // Bouncing Balls
                struct Ball { float x, y, vx, vy; char sym; };
                std::vector<Ball> balls;
                const char* symbols = "oO@*#";
                for (int b = 0; b < 15; b++) {
                    balls.push_back({
                        (float)(rand() % (WIDTH-4) + 3),
                        (float)(rand() % (HEIGHT-4) + 3),
                        (rand() % 200 - 100) / 50.0f,
                        (rand() % 200 - 100) / 50.0f,
                        symbols[rand() % 5]
                    });
                }
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++)
                            anim.Set_Char(i, j, ' ');
                    
                    for (auto& b : balls) {
                        b.x += b.vx;
                        b.y += b.vy;
                        if (b.x <= 1 || b.x >= WIDTH) b.vx *= -1;
                        if (b.y <= 1 || b.y >= HEIGHT) b.vy *= -1;
                        if (b.x < 1) b.x = 2;
                        if (b.x > WIDTH) b.x = WIDTH-1;
                        if (b.y < 1) b.y = 2;
                        if (b.y > HEIGHT) b.y = HEIGHT-1;
                        anim.Set_Char((int)b.y, (int)b.x, b.sym);
                    }
                    anim.Render_Frame();
                    usleep(30000);
                    frameCount++;
                }
                break;
            }
            
            case 4: { // Fire Effect
                std::vector<std::vector<int>> fire(HEIGHT, std::vector<int>(WIDTH, 0));
                const char* fireChars = " .:;+=xX$&#";
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int j = 0; j < WIDTH; j++)
                        fire[HEIGHT-1][j] = 150 + rand() % 106;
                    
                    for (int i = 0; i < HEIGHT-1; i++)
                        for (int j = 0; j < WIDTH; j++) {
                            int left = (j-1+WIDTH) % WIDTH;
                            int right = (j+1) % WIDTH;
                            int avg = (fire[i+1][j] * 2 + fire[i+1][left] + fire[i+1][right]) / 4;
                            fire[i][j] = std::max(0, avg - (rand() % 10));
                        }
                    
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++) {
                            int val = fire[i-1][j-1];
                            int idx = val / 25;
                            if (idx > 10) idx = 10;
                            anim.Set_Char(i, j, fireChars[idx]);
                        }
                    
                    anim.Render_Frame();
                    usleep(40000);
                    frameCount++;
                }
                break;
            }
            
            case 5: { // Starfield
                struct Star { float x, y, z; };
                std::vector<Star> stars(500);
                for (auto& s : stars) {
                    s.x = (rand() % 2000 - 1000) / 100.0f;
                    s.y = (rand() % 2000 - 1000) / 100.0f;
                    s.z = rand() % 100 + 1.0f;
                }
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++)
                            anim.Set_Char(i, j, ' ');
                    
                    for (auto& s : stars) {
                        s.z -= 2.0f;
                        if (s.z <= 0) {
                            s.x = (rand() % 2000 - 1000) / 100.0f;
                            s.y = (rand() % 2000 - 1000) / 100.0f;
                            s.z = 100.0f;
                        }
                        
                        int px = WIDTH/2 + (int)(s.x / s.z * 50);
                        int py = HEIGHT/2 + (int)(s.y / s.z * 25);
                        if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                            char ch = s.z < 30 ? '#' : (s.z < 60 ? '*' : '.');
                            anim.Set_Char(py, px, ch);
                        }
                    }
                    anim.Render_Frame();
                    usleep(50000);
                    frameCount++;
                }
                break;
            }
            
            case 6: { // Game of Life
                std::vector<std::vector<bool>> grid(HEIGHT, std::vector<bool>(WIDTH, false));
                for (int i = 0; i < HEIGHT; i++)
                    for (int j = 0; j < WIDTH; j++)
                        grid[i][j] = (rand() % 100) < 30;
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    auto next = grid;
                    
                    for (int i = 0; i < HEIGHT; i++)
                        for (int j = 0; j < WIDTH; j++) {
                            int neighbors = 0;
                            for (int di = -1; di <= 1; di++)
                                for (int dj = -1; dj <= 1; dj++) {
                                    if (di == 0 && dj == 0) continue;
                                    int ni = (i + di + HEIGHT) % HEIGHT;
                                    int nj = (j + dj + WIDTH) % WIDTH;
                                    if (grid[ni][nj]) neighbors++;
                                }
                            
                            if (grid[i][j])
                                next[i][j] = (neighbors == 2 || neighbors == 3);
                            else
                                next[i][j] = (neighbors == 3);
                            
                            anim.Set_Char(i+1, j+1, next[i][j] ? '#' : ' ');
                        }
                    
                    grid = next;
                    anim.Render_Frame();
                    usleep(100000);
                    frameCount++;
                    
                    if (frameCount % 100 == 0)
                        for (int i = 0; i < HEIGHT; i++)
                            for (int j = 0; j < WIDTH; j++)
                                if (rand() % 100 < 10) grid[i][j] = true;
                }
                break;
            }
            
            case 7: { // Plasma - FIXED variable name
                float phase = 0;
                const char* chars = " .:-=+*#%@";
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++) {
                        for (int j = 1; j <= WIDTH; j++) {
                            float v1 = sin(j * 0.05f + phase);
                            float v2 = sin(i * 0.03f + phase * 1.3f);
                            float v3 = sin((j * 0.02f + i * 0.01f) + phase * 0.7f);
                            float val = (v1 + v2 + v3) / 3.0f;
                            int idx = (int)((val + 1.0f) * 4.5f);
                            if (idx < 0) idx = 0;
                            if (idx > 9) idx = 9;
                            anim.Set_Char(i, j, chars[idx]);
                        }
                    }
                    anim.Render_Frame();
                    phase += 0.1f;
                    usleep(40000);
                    frameCount++;
                }
                break;
            }
            
            case 8: { // Clock
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    ::time_t now = ::time(NULL);
                    struct tm* tm_info = localtime(&now);
                    
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++)
                            anim.Set_Char(i, j, ' ');
                    
                    int cx = WIDTH/2;
                    int cy = HEIGHT/2;
                    int radius = std::min(WIDTH, HEIGHT)/2 - 5;
                    
                    for (int ang = 0; ang < 360; ang += 30) {
                        int x = cx + (int)(radius * cos(ang * PI / 180.0));
                        int y = cy + (int)(radius * sin(ang * PI / 180.0) * 0.5);
                        if (x >= 1 && x <= WIDTH && y >= 1 && y <= HEIGHT)
                            anim.Set_Char(y, x, 'o');
                    }
                    
                    float hAng = ((tm_info->tm_hour % 12) * 30 + tm_info->tm_min * 0.5f) * PI / 180.0f;
                    for (int r = 0; r < radius * 0.5f; r++) {
                        int x = cx + (int)(r * sin(hAng));
                        int y = cy - (int)(r * cos(hAng) * 0.5f);
                        if (x >= 1 && x <= WIDTH && y >= 1 && y <= HEIGHT)
                            anim.Set_Char(y, x, 'H');
                    }
                    
                    float mAng = (tm_info->tm_min * 6) * PI / 180.0f;
                    for (int r = 0; r < radius * 0.7f; r++) {
                        int x = cx + (int)(r * sin(mAng));
                        int y = cy - (int)(r * cos(mAng) * 0.5f);
                        if (x >= 1 && x <= WIDTH && y >= 1 && y <= HEIGHT)
                            anim.Set_Char(y, x, 'M');
                    }
                    
                    float sAng = (tm_info->tm_sec * 6) * PI / 180.0f;
                    for (int r = 0; r < radius * 0.9f; r++) {
                        int x = cx + (int)(r * sin(sAng));
                        int y = cy - (int)(r * cos(sAng) * 0.5f);
                        if (x >= 1 && x <= WIDTH && y >= 1 && y <= HEIGHT)
                            anim.Set_Char(y, x, '.');
                    }
                    
                    anim.Render_Frame();
                    usleep(100000);
                    frameCount++;
                }
                break;
            }
            
            case 9: { // Sine Waves
                float phaseShift = 0;
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++)
                            anim.Set_Char(i, j, ' ');
                    
                    for (int wave = 0; wave < 5; wave++) {
                        float freq = 0.05f + wave * 0.01f;
                        float amp = 8.0f - wave;
                        int offset = HEIGHT/2 + wave * 8 - 16;
                        
                        for (int x = 1; x <= WIDTH; x++) {
                            int y1 = offset + (int)(amp * sin(x * freq + phaseShift));
                            int y2 = offset + (int)(amp * cos(x * freq * 1.5f + phaseShift * 0.7f));
                            
                            if (y1 >= 1 && y1 <= HEIGHT) anim.Set_Char(y1, x, '#');
                            if (y2 >= 1 && y2 <= HEIGHT) anim.Set_Char(y2, x, '*');
                        }
                    }
                    
                    anim.Render_Frame();
                    phaseShift += 0.1f;
                    usleep(40000);
                    frameCount++;
                }
                break;
            }
            
            case 10: { // Particle Explosions
                struct Particle { float x, y, vx, vy; int life; char sym; };
                std::vector<Particle> particles;
                const char* partChars = "*+#@.";
                
                while (::difftime(::time(NULL), startTime) < 30.0) {
                    for (int i = 1; i <= HEIGHT; i++)
                        for (int j = 1; j <= WIDTH; j++)
                            anim.Set_Char(i, j, ' ');
                    
                    if (frameCount % 50 == 0) {
                        int cx = rand() % (WIDTH-20) + 10;
                        int cy = rand() % (HEIGHT-10) + 5;
                        for (int p = 0; p < 30; p++) {
                            float ang = (rand() % 360) * PI / 180.0f;
                            float spd = (rand() % 50) / 20.0f;
                            particles.push_back({
                                (float)cx, (float)cy,
                                cos(ang) * spd,
                                sin(ang) * spd * 0.5f,
                                100,
                                partChars[rand() % 5]
                            });
                        }
                    }
                    
                    for (auto it = particles.begin(); it != particles.end();) {
                        it->x += it->vx;
                        it->y += it->vy;
                        it->vy += 0.1f;
                        it->life -= 2;
                        
                        if (it->life <= 0 || it->x < 1 || it->x > WIDTH || it->y < 1 || it->y > HEIGHT)
                            it = particles.erase(it);
                        else {
                            anim.Set_Char((int)it->y, (int)it->x, it->sym);
                            ++it;
                        }
                    }
                    
                    anim.Render_Frame();
                    usleep(40000);
                    frameCount++;
                }
                break;
            }
        }
        
        std::cout << "\n\nAnimation completed! Frames: " << frameCount << "\nPress Enter...";
        std::cin.get();
    }
    
    return 0;
}