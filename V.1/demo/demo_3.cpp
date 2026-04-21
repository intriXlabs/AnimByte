#include "AnimByte.cpp"
#include <cmath>
#include <ctime>
#include <vector>
#include <chrono>
#include <cstring>



int main() {
    AnimByte anim;
    const int WIDTH = 210;
    const int HEIGHT = 54;
    const float PI = 3.14159265358979323846;
    const float PHI = 1.618033988749895f;  // Golden ratio
    
    anim.Initialise(WIDTH, HEIGHT);
    
    // 16-level grayscale characters
    const char* grayChars = "@%#*+=-:.`'^\"~_,; ";
    const int GRAY_LEVELS = 16;
    
    // Animation state
    float time = 0.0f;
    int mode = 0;
    float modeTimer = 0.0f;
    const float MODE_DURATION = 12.0f;
    
    // Fibonacci numbers for scaling
    int fib[20] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765};
    
    auto lastFrameTime = std::chrono::steady_clock::now();
    
    std::cout << "\033[2J\033[H";
    std::cout << "GOLDEN RATIO (φ = 1.618) ANIMATION - 16-LEVEL DEPTH\n";
    std::cout << "Spirals | Rectangles | Fibonacci | Sacred Geometry\n";
    std::cout << "Press Ctrl+C to exit\n";
    usleep(2000000);
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        deltaTime = std::min(deltaTime, 0.033f);
        
        time += deltaTime;
        modeTimer += deltaTime;
        
        if (modeTimer >= MODE_DURATION) {
            modeTimer = 0.0f;
            mode = (mode + 1) % 6;
        }
        
        // Clear buffer
        for (int i = 1; i <= HEIGHT; i++)
            for (int j = 1; j <= WIDTH; j++)
                anim.Set_Char(i, j, ' ');
        
        float centerX = WIDTH / 2.0f;
        float centerY = HEIGHT / 2.0f;
        float transitionProgress = modeTimer / MODE_DURATION;
        
        switch(mode) {
            case 0: { // Golden Spiral (Logarithmic)
                float rotation = time * 0.5f;
                float scale = 1.0f + sin(time * 0.3f) * 0.2f;
                
                for (int i = 1; i <= HEIGHT; i++) {
                    for (int j = 1; j <= WIDTH; j++) {
                        float dx = (j - centerX) / scale;
                        float dy = (i - centerY) / scale * 2.0f;  // Aspect correction
                        
                        float dist = sqrt(dx*dx + dy*dy);
                        float angle = atan2(dy, dx) - rotation;
                        
                        // Golden spiral: r = a * φ^(θ/π)
                        float spiralR = 3.0f * pow(PHI, angle / PI);
                        float spiralDist = fabs(dist - spiralR);
                        
                        if (spiralDist < 2.5f) {
                            // Spiral line with thickness gradient
                            float intensity = 1.0f - (spiralDist / 3.0f);
                            // Add golden ratio angle bands
                            float bandAngle = fmod(angle + rotation * 2.0f, 2.0f * PI);
                            intensity *= 0.7f + 0.3f * cos(bandAngle * 8.0f);
                            
                            int idx = (int)(intensity * (GRAY_LEVELS - 1));
                            if (idx < 0) idx = 0;
                            if (idx >= GRAY_LEVELS) idx = GRAY_LEVELS - 1;
                            anim.Set_Char(i, j, grayChars[idx]);
                        }
                        
                        // Golden angle markers (137.5°)
                        float goldenAngle = 137.5077f * PI / 180.0f;
                        for (int k = 0; k < 12; k++) {
                            float markAngle = k * goldenAngle + rotation * 1.5f;
                            float markX = centerX + cos(markAngle) * spiralR * scale;
                            float markY = centerY + sin(markAngle) * spiralR * scale * 0.5f;
                            if (fabs(j - markX) < 1.5f && fabs(i - markY) < 1.5f) {
                                anim.Set_Char(i, j, grayChars[2]);
                            }
                        }
                    }
                }
                break;
            }
            
            case 1: { // Golden Rectangles (Recursive subdivision)
                float subdividePhase = time * 0.3f;
                int depth = 5 + (int)(sin(time * 0.5f) * 3);
                
                struct Rect {
                    float x, y, w, h;
                    int depth;
                };
                
                std::vector<Rect> rects;
                rects.push_back({centerX - 40, centerY - 20, 80, 50, 0});
                
                for (int d = 0; d < depth; d++) {
                    std::vector<Rect> newRects;
                    for (auto& r : rects) {
                        if (r.depth >= d) {
                            // Subdivide according to golden ratio
                            float split;
                            Rect rect1, rect2;
                            
                            if (r.w > r.h) {
                                split = r.w / PHI;
                                rect1 = {r.x, r.y, split, r.h, r.depth + 1};
                                rect2 = {r.x + split, r.y, r.w - split, r.h, r.depth + 1};
                            } else {
                                split = r.h / PHI;
                                rect1 = {r.x, r.y, r.w, split, r.depth + 1};
                                rect2 = {r.x, r.y + split, r.w, r.h - split, r.depth + 1};
                            }
                            
                            // Animate which side to keep
                            float pulse = sin(time * 2.0f + r.depth) * 0.5f + 0.5f;
                            if (pulse > 0.5f) {
                                newRects.push_back(rect1);
                            } else {
                                newRects.push_back(rect2);
                            }
                        }
                    }
                    rects = newRects;
                    
                    // Draw rectangles
                    for (auto& r : rects) {
                        int x1 = (int)r.x;
                        int y1 = (int)r.y;
                        int x2 = (int)(r.x + r.w);
                        int y2 = (int)(r.y + r.h);
                        
                        if (x1 < 1) x1 = 1;
                        if (y1 < 1) y1 = 1;
                        if (x2 > WIDTH) x2 = WIDTH;
                        if (y2 > HEIGHT) y2 = HEIGHT;
                        
                        int colorIdx = 4 + (r.depth * 2);
                        if (colorIdx >= GRAY_LEVELS) colorIdx = GRAY_LEVELS - 1;
                        
                        // Draw border
                        for (int x = x1; x <= x2; x++) {
                            anim.Set_Char(y1, x, grayChars[colorIdx]);
                            anim.Set_Char(y2, x, grayChars[colorIdx]);
                        }
                        for (int y = y1; y <= y2; y++) {
                            anim.Set_Char(y, x1, grayChars[colorIdx]);
                            anim.Set_Char(y, x2, grayChars[colorIdx]);
                        }
                        
                        // Draw golden spiral inside largest rectangle
                        if (d == 0) {
                            float cx = r.x + r.w / 2.0f;
                            float cy = r.y + r.h / 2.0f;
                            for (float angle = 0; angle < PI * 2; angle += 0.1f) {
                                float rSpiral = 5.0f * pow(PHI, angle / PI);
                                int sx = cx + (int)(cos(angle + time) * rSpiral);
                                int sy = cy + (int)(sin(angle + time) * rSpiral * 0.5f);
                                if (sx >= 1 && sx <= WIDTH && sy >= 1 && sy <= HEIGHT) {
                                    anim.Set_Char(sy, sx, grayChars[0]);
                                }
                            }
                        }
                    }
                }
                break;
            }
            
            case 2: { // Fibonacci Sequence Visualization
                int maxFib = 15;
                float pulseTime = time * 2.0f;
                
                // Draw Fibonacci squares
                int x = centerX - 30;
                int y = centerY - 15;
                int size = 3;
                int prevSize = 2;
                
                for (int i = 0; i < maxFib; i++) {
                    // Fibonacci number as size
                    int fibNum = fib[i + 2];
                    int squareSize = fibNum * 2;
                    
                    // Draw square
                    int colorIdx = 2 + (i % 8);
                    if (colorIdx >= GRAY_LEVELS) colorIdx = GRAY_LEVELS - 1;
                    
                    float pulse = 0.7f + 0.3f * sin(pulseTime + i * 0.5f);
                    int idx = (int)(colorIdx * pulse);
                    
                    for (int sx = 0; sx < squareSize; sx++) {
                        for (int sy = 0; sy < squareSize; sy++) {
                            int drawX = x + sx;
                            int drawY = y + sy;
                            if (drawX >= 1 && drawX <= WIDTH && drawY >= 1 && drawY <= HEIGHT) {
                                if (sx == 0 || sx == squareSize-1 || sy == 0 || sy == squareSize-1) {
                                    anim.Set_Char(drawY, drawX, grayChars[idx]);
                                } else if (i == maxFib - 1) {
                                    // Fill last square with spiral
                                    float cx = x + squareSize / 2.0f;
                                    float cy = y + squareSize / 2.0f;
                                    float dx = (drawX - cx) / 3.0f;
                                    float dy = (drawY - cy) / 1.5f;
                                    float dist = sqrt(dx*dx + dy*dy);
                                    if (dist < squareSize / 3.0f) {
                                        int spiralIdx = (int)(dist / 3.0f) + 6;
                                        if (spiralIdx < GRAY_LEVELS) {
                                            anim.Set_Char(drawY, drawX, grayChars[spiralIdx]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    // Display Fibonacci number
                    char numStr[10];
                    snprintf(numStr, sizeof(numStr), "%d", fibNum);
                    int numX = x + squareSize / 2 - strlen(numStr) / 2;
                    int numY = y + squareSize / 2;
                    if (numX >= 1 && numX + (int)strlen(numStr) <= WIDTH && numY >= 1 && numY <= HEIGHT) {
                        for (int c = 0; numStr[c]; c++) {
                            anim.Set_Char(numY, numX + c, numStr[c]);
                        }
                    }
                    
                    // Move position for next square (spiral pattern)
                    switch(i % 4) {
                        case 0: x += squareSize; break;
                        case 1: y += squareSize; x -= prevSize; break;
                        case 2: x -= squareSize + prevSize; break;
                        case 3: y -= squareSize; break;
                    }
                    prevSize = squareSize;
                }
                break;
            }
            
            case 3: { // Golden Ratio Grid & Pentagram
                float rot = time * 0.2f;
                float scale = 15.0f;
                
                // Draw golden ratio grid lines
                for (int i = 0; i < 2; i++) {
                    float ratio = (i == 0) ? 1.0f : PHI;
                    for (int g = -3; g <= 3; g++) {
                        float gridX = centerX + g * scale * ratio;
                        float gridY = centerY + g * scale * ratio * 0.5f;
                        
                        // Vertical lines
                        if (gridX >= 1 && gridX <= WIDTH) {
                            for (int y = 1; y <= HEIGHT; y++) {
                                int idx = 10 + (int)(fabs(g) * 0.5f);
                                if (idx < GRAY_LEVELS) {
                                    anim.Set_Char(y, (int)gridX, grayChars[idx]);
                                }
                            }
                        }
                        
                        // Horizontal lines
                        if (gridY >= 1 && gridY <= HEIGHT) {
                            for (int x = 1; x <= WIDTH; x++) {
                                int idx = 10 + (int)(fabs(g) * 0.5f);
                                if (idx < GRAY_LEVELS) {
                                    anim.Set_Char((int)gridY, x, grayChars[idx]);
                                }
                            }
                        }
                    }
                }
                
                // Draw pentagram/pentagon (golden ratio in angles)
                float pentagonAngles[5];
                for (int i = 0; i < 5; i++) {
                    pentagonAngles[i] = i * 72.0f * PI / 180.0f + rot;
                }
                
                // Draw star
                for (int i = 0; i < 5; i++) {
                    float angle1 = pentagonAngles[i];
                    float angle2 = pentagonAngles[(i + 2) % 5];
                    
                    float x1 = centerX + cos(angle1) * 20;
                    float y1 = centerY + sin(angle1) * 10;
                    float x2 = centerX + cos(angle2) * 20;
                    float y2 = centerY + sin(angle2) * 10;
                    
                    // Draw line
                    float steps = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
                    for (int s = 0; s <= steps; s++) {
                        float t = s / steps;
                        int lx = x1 + (x2 - x1) * t;
                        int ly = y1 + (y2 - y1) * t;
                        if (lx >= 1 && lx <= WIDTH && ly >= 1 && ly <= HEIGHT) {
                            anim.Set_Char(ly, lx, grayChars[2]);
                        }
                    }
                }
                
                // Draw pentagon
                for (int i = 0; i < 5; i++) {
                    float angle1 = pentagonAngles[i];
                    float angle2 = pentagonAngles[(i + 1) % 5];
                    
                    float x1 = centerX + cos(angle1) * 15;
                    float y1 = centerY + sin(angle1) * 7.5f;
                    float x2 = centerX + cos(angle2) * 15;
                    float y2 = centerY + sin(angle2) * 7.5f;
                    
                    float steps = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
                    for (int s = 0; s <= steps; s++) {
                        float t = s / steps;
                        int lx = x1 + (x2 - x1) * t;
                        int ly = y1 + (y2 - y1) * t;
                        if (lx >= 1 && lx <= WIDTH && ly >= 1 && ly <= HEIGHT) {
                            anim.Set_Char(ly, lx, grayChars[5]);
                        }
                    }
                }
                break;
            }
            
            case 4: { // Golden Ratio Wave Interference
                for (int i = 1; i <= HEIGHT; i++) {
                    for (int j = 1; j <= WIDTH; j++) {
                        float x = (j - centerX) / 30.0f;
                        float y = (i - centerY) / 15.0f;
                        
                        // Multiple waves with golden ratio frequencies
                        float wave1 = sin(x * PHI + time) * cos(y * 1.0f + time * 0.5f);
                        float wave2 = sin(x * 1.0f - time * 0.7f) * cos(y * PHI + time * 0.3f);
                        float wave3 = sin((x + y) * (PHI * 0.5f) + time * 1.2f);
                        float wave4 = cos((x - y) * (PHI * 0.3f) - time * 0.8f);
                        
                        float combined = (wave1 + wave2 + wave3 + wave4) / 4.0f;
                        
                        // Map to grayscale
                        int idx = (int)((combined + 1.0f) * 0.5f * (GRAY_LEVELS - 1));
                        if (idx < 0) idx = 0;
                        if (idx >= GRAY_LEVELS) idx = GRAY_LEVELS - 1;
                        
                        anim.Set_Char(i, j, grayChars[idx]);
                    }
                }
                
                // Draw golden ratio overlay
                float phiSpiral = time * 0.3f;
                for (int i = 0; i < 8; i++) {
                    float angle = i * 137.5f * PI / 180.0f + phiSpiral;
                    float dist = 5.0f * pow(PHI, i * 0.5f);
                    int x = centerX + cos(angle) * dist;
                    int y = centerY + sin(angle) * dist * 0.5f;
                    if (x >= 1 && x <= WIDTH && y >= 1 && y <= HEIGHT) {
                        anim.Set_Char(y, x, '@');
                    }
                }
                break;
            }
            
            case 5: { // Golden Ratio Tree / Fractal
                float treeTime = time * 0.5f;
                int maxDepth = 8;
                
                struct Branch {
                    float x, y, angle, length;
                    int depth;
                };
                
                std::vector<Branch> branches;
                branches.push_back({centerX, (float)HEIGHT - 5, -PI / 2.0f, 20.0f, 0});
                
                for (int iter = 0; iter < maxDepth; iter++) {
                    std::vector<Branch> newBranches;
                    for (auto& b : branches) {
                        // Draw branch
                        float endX = b.x + cos(b.angle) * b.length;
                        float endY = b.y + sin(b.angle) * b.length * 0.5f;
                        
                        int steps = (int)b.length * 2;
                        for (int s = 0; s <= steps; s++) {
                            float t = s / (float)steps;
                            int bx = b.x + (endX - b.x) * t;
                            int by = b.y + (endY - b.y) * t;
                            if (bx >= 1 && bx <= WIDTH && by >= 1 && by <= HEIGHT) {
                                int idx = b.depth * 2;
                                if (idx < GRAY_LEVELS) {
                                    anim.Set_Char(by, bx, grayChars[idx]);
                                }
                            }
                        }
                        
                        if (b.depth < maxDepth) {
                            // Golden ratio branching
                            float newLength = b.length / PHI;
                            
                            // Left branch (golden angle)
                            Branch left = {
                                endX, endY,
                                b.angle - 137.5f * PI / 180.0f + sin(treeTime + b.depth) * 0.1f,
                                newLength,
                                b.depth + 1
                            };
                            newBranches.push_back(left);
                            
                            // Right branch
                            Branch right = {
                                endX, endY,
                                b.angle + 137.5f * PI / 180.0f + cos(treeTime + b.depth) * 0.1f,
                                newLength,
                                b.depth + 1
                            };
                            newBranches.push_back(right);
                        }
                        
                        // Leaves at tips
                        if (b.depth == maxDepth) {
                            for (int l = 0; l < 3; l++) {
                                int leafX = endX + (rand() % 5 - 2);
                                int leafY = endY + (rand() % 3 - 1);
                                if (leafX >= 1 && leafX <= WIDTH && leafY >= 1 && leafY <= HEIGHT) {
                                    anim.Set_Char(leafY, leafX, grayChars[8 + (rand() % 4)]);
                                }
                            }
                        }
                    }
                    branches = newBranches;
                }
                break;
            }
        }
        
        // Draw golden ratio symbol and info
        char info[100];
        const char* modeNames[] = {
            "GOLDEN SPIRAL", "GOLDEN RECTANGLES", "FIBONACCI SEQUENCE",
            "GOLDEN GRID & PENTAGRAM", "WAVE INTERFERENCE", "GOLDEN TREE"
        };
        
        snprintf(info, sizeof(info), "φ = 1.618033988749895 | %s | Time: %.1fs", 
                 modeNames[mode], time);
        
        for (int i = 0; info[i]; i++) {
            anim.Set_Char(HEIGHT-1, (WIDTH - strlen(info)) / 2 + i, info[i]);
        }
        
        // Draw phi symbol large
        const char* phiArt[] = {
            "  φ  ",
            " / \\ ",
            "/   \\",
            "\\   /",
            " \\ / ",
            "  |  "
        };
        
        int phiY = 3;
        for (int i = 0; i < 6; i++) {
            int phiX = 3;
            for (int j = 0; phiArt[i][j]; j++) {
                anim.Set_Char(phiY + i, phiX + j, phiArt[i][j]);
            }
        }
        
        // Draw transition progress bar
        int barWidth = 40;
        int barX = WIDTH - barWidth - 5;
        int barY = HEIGHT - 3;
        int fillWidth = (int)(barWidth * transitionProgress);
        
        anim.Set_Char(barY, barX - 1, '[');
        anim.Set_Char(barY, barX + barWidth, ']');
        for (int i = 0; i < barWidth; i++) {
            char ch = (i < fillWidth) ? '=' : ' ';
            anim.Set_Char(barY, barX + i, ch);
        }
        
        anim.Render_Frame();
        usleep(16667);  // ~60 FPS
    }
    
    return 0;
}