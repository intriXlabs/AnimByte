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
    
    anim.Initialise(WIDTH, HEIGHT);
    
    // 16-level grayscale characters (darkest to lightest)
    const char* grayChars = "@%#*+=-:.`'^\"~_,; ";
    const int GRAY_LEVELS = 16;
    
    // Eye parameters
    float eyeLX = WIDTH/2 - 45;  // Left eye center X
    float eyeRX = WIDTH/2 + 45;  // Right eye center X
    float eyeY = HEIGHT/2 - 2;    // Eye center Y
    float eyeRadiusX = 38;        // Eye width
    float eyeRadiusY = 22;        // Eye height
    float pupilRadius = 7;        // Pupil size
    float irisRadius = 15;        // Iris size
    
    // Movement state with smooth interpolation
    float targetX = 0, targetY = 0;
    float currentX = 0, currentY = 0;
    float velocityX = 0, velocityY = 0;
    float acceleration = 0.15f;
    float damping = 0.85f;
    
    // Blink system
    float blinkPhase = 0.0f;           // 0=open, 1=closed
    float blinkSpeed = 0.0f;
    float blinkTimer = 0.0f;
    float nextBlinkTime = 3.0f + (rand() % 300) / 100.0f;
    
    // Saccade system
    float saccadeTimer = 0.0f;
    float saccadeInterval = 0.3f;
    bool isSaccade = false;
    float saccadeProgress = 0.0f;
    float saccadeStartX = 0, saccadeStartY = 0;
    float saccadeTargetX = 0, saccadeTargetY = 0;
    
    // Smooth pursuit
    float pursuitPhase = 0.0f;
    bool isPursuit = false;
    float pursuitDuration = 0.0f;
    
    // Micro-tremor
    float tremorPhase = 0.0f;
    
    // Eyelid crease
    float eyelidOffset = 0.0f;
    
    auto lastFrameTime = std::chrono::steady_clock::now();
    
    std::cout << "\033[2J\033[H";
    std::cout << "REALISTIC EYE ANIMATION - 16-LEVEL DEPTH | SMOOTH PHYSICS\n";
    std::cout << "Saccades | Smooth Pursuit | Realistic Blinks | Micro-tremors\n";
    std::cout << "Press Ctrl+C to exit\n";
    usleep(2000000);
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        deltaTime = std::min(deltaTime, 0.033f); // Cap at 30fps equivalent
        
        // Clear buffer
        for (int i = 1; i <= HEIGHT; i++)
            for (int j = 1; j <= WIDTH; j++)
                anim.Set_Char(i, j, ' ');
        
        // === UPDATE EYE BEHAVIORS ===
        
        // Blink logic
        blinkTimer += deltaTime;
        if (blinkTimer >= nextBlinkTime && blinkPhase < 0.01f) {
            blinkSpeed = 8.0f;  // Start blink
            nextBlinkTime = 3.0f + (rand() % 400) / 100.0f;
            blinkTimer = 0.0f;
        }
        
        // Update blink phase with smooth acceleration/deceleration
        if (blinkSpeed > 0.01f || blinkPhase > 0.01f) {
            blinkPhase += blinkSpeed * deltaTime;
            
            // Eyelid physics - accelerate then decelerate
            if (blinkPhase >= 0.7f && blinkSpeed > 0) {
                blinkSpeed *= 0.92f;  // Slow down near closed
            }
            if (blinkPhase >= 1.0f) {
                blinkPhase = 1.0f;
                blinkSpeed = -5.0f;  // Start opening
            }
            if (blinkPhase <= 0.3f && blinkSpeed < 0) {
                blinkSpeed *= 0.9f;  // Slow down near open
            }
            if (blinkPhase <= 0.0f) {
                blinkPhase = 0.0f;
                blinkSpeed = 0.0f;
            }
            
            // Add slight eyelid droop after blinks
            eyelidOffset = blinkPhase * 3.0f;
        }
        
        // Saccade generation (only when eyes open)
        if (blinkPhase < 0.3f) {
            saccadeTimer += deltaTime;
            
            if (!isSaccade && saccadeTimer >= saccadeInterval) {
                // Start new saccade
                isSaccade = true;
                saccadeProgress = 0.0f;
                saccadeStartX = currentX;
                saccadeStartY = currentY;
                
                // Choose target (sometimes follow pattern, sometimes random)
                if (rand() % 100 < 30) {
                    // Look at specific regions (edges, center)
                    int region = rand() % 5;
                    switch(region) {
                        case 0: saccadeTargetX = 0; saccadeTargetY = 0; break;
                        case 1: saccadeTargetX = 25; saccadeTargetY = -10; break;
                        case 2: saccadeTargetX = -25; saccadeTargetY = -10; break;
                        case 3: saccadeTargetX = 20; saccadeTargetY = 12; break;
                        case 4: saccadeTargetX = -20; saccadeTargetY = 12; break;
                    }
                } else {
                    // Random target within bounds
                    float angle = (rand() % 360) * PI / 180.0f;
                    float distance = (rand() % 85) / 100.0f;
                    float maxDist = (eyeRadiusX - irisRadius - 3);
                    saccadeTargetX = cos(angle) * maxDist * distance;
                    saccadeTargetY = sin(angle) * (eyeRadiusY - irisRadius - 3) * distance * 0.7f;
                }
                
                saccadeInterval = 0.2f + (rand() % 300) / 1000.0f;
                saccadeTimer = 0.0f;
                isPursuit = false;
            }
            
            // Smooth pursuit trigger
            if (!isSaccade && !isPursuit && rand() % 1000 < 2) {
                isPursuit = true;
                pursuitDuration = 1.5f + (rand() % 200) / 100.0f;
                pursuitPhase += 0.5f;
            }
            
            // Update pursuit target
            if (isPursuit) {
                pursuitDuration -= deltaTime;
                pursuitPhase += deltaTime * 1.5f;
                
                // Smooth sinusoidal tracking
                targetX = sin(pursuitPhase) * 22.0f;
                targetY = cos(pursuitPhase * 1.3f) * 14.0f + sin(pursuitPhase * 0.7f) * 6.0f;
                
                if (pursuitDuration <= 0.0f) {
                    isPursuit = false;
                }
            }
        }
        
        // Update saccade progress
        if (isSaccade) {
            saccadeProgress += deltaTime * 15.0f;  // Very fast movement
            
            if (saccadeProgress >= 1.0f) {
                saccadeProgress = 1.0f;
                isSaccade = false;
                targetX = saccadeTargetX;
                targetY = saccadeTargetY;
            } else {
                // Easing function for natural saccade
                float t = saccadeProgress;
                float easeT = t < 0.5f ? 2.0f * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
                targetX = saccadeStartX + (saccadeTargetX - saccadeStartX) * easeT;
                targetY = saccadeStartY + (saccadeTargetY - saccadeStartY) * easeT;
            }
        }
        
        // Smooth movement toward target
        if (!isSaccade && blinkPhase < 0.5f) {
            float moveSpeed = isPursuit ? 5.0f : 8.0f;
            float dx = targetX - currentX;
            float dy = targetY - currentY;
            
            velocityX += dx * acceleration * deltaTime * moveSpeed;
            velocityY += dy * acceleration * deltaTime * moveSpeed;
            velocityX *= damping;
            velocityY *= damping;
            
            currentX += velocityX;
            currentY += velocityY;
        }
        
        // Micro-tremor (physiological nystagmus)
        tremorPhase += deltaTime * 20.0f;
        float tremorX = sin(tremorPhase) * 0.3f + cos(tremorPhase * 2.3f) * 0.2f;
        float tremorY = cos(tremorPhase * 1.7f) * 0.3f + sin(tremorPhase * 3.1f) * 0.2f;
        currentX += tremorX * (1.0f - blinkPhase);
        currentY += tremorY * (1.0f - blinkPhase);
        
        // Clamp to eye bounds
        float maxX = eyeRadiusX - irisRadius - 2;
        float maxY = eyeRadiusY - irisRadius - 2;
        currentX = std::max(-maxX, std::min(maxX, currentX));
        currentY = std::max(-maxY, std::min(maxY, currentY));
        targetX = std::max(-maxX, std::min(maxX, targetX));
        targetY = std::max(-maxY, std::min(maxY, targetY));
        
        // === RENDER EYES ===
        float eyes[][2] = {{eyeLX, eyeY}, {eyeRX, eyeY}};
        
        for (int e = 0; e < 2; e++) {
            float centerX = eyes[e][0];
            float centerY = eyes[e][1];
            
            // Calculate eyelid coverage based on blink phase
            float upperLidCover = blinkPhase * (eyeRadiusY * 2 + 8);
            float lowerLidCover = blinkPhase * (eyeRadiusY * 0.5f);
            
            // Draw each pixel of the eye
            for (int y = -eyeRadiusY - 4; y <= eyeRadiusY + 4; y++) {
                for (int x = -eyeRadiusX - 4; x <= eyeRadiusX + 4; x++) {
                    int drawX = centerX + x;
                    int drawY = centerY + y;
                    
                    if (drawX < 1 || drawX > WIDTH || drawY < 1 || drawY > HEIGHT) continue;
                    
                    // Check if covered by eyelid
                    float lidY = y + eyeRadiusY;
                    if (lidY < upperLidCover || (eyeRadiusY * 2 - lidY) < lowerLidCover) {
                        // Draw eyelid skin
                        float skinDist = abs(x) / (eyeRadiusX + 6.0f);
                        int skinIdx = 6 + (int)(skinDist * 5);
                        if (skinIdx >= GRAY_LEVELS) skinIdx = GRAY_LEVELS - 1;
                        anim.Set_Char(drawY, drawX, grayChars[skinIdx]);
                        continue;
                    }
                    
                    // Ellipse test for eye shape
                    float ellipseVal = (x*x)/(eyeRadiusX*eyeRadiusX) + 
                                      (y*y)/(eyeRadiusY*eyeRadiusY);
                    
                    if (ellipseVal <= 1.0f) {
                        // Inside eye
                        if (ellipseVal > 0.9f) {
                            // Edge of eye - darker
                            anim.Set_Char(drawY, drawX, grayChars[4]);
                        } else {
                            // Eye white (sclera) with subtle shading
                            float shade = 1.0f - (ellipseVal * 0.3f);
                            int shadeIdx = 10 + (int)(shade * 4);
                            if (shadeIdx >= GRAY_LEVELS) shadeIdx = GRAY_LEVELS - 2;
                            anim.Set_Char(drawY, drawX, grayChars[shadeIdx]);
                            
                            // Blood vessels in sclera (subtle)
                            if (abs(x) > eyeRadiusX * 0.7f && abs(y) < 3 && rand() % 100 < 2) {
                                anim.Set_Char(drawY, drawX, grayChars[8]);
                            }
                            
                            // Draw iris
                            float irisX = x - currentX;
                            float irisY = y - currentY;
                            float irisDist = sqrt(irisX*irisX + (irisY*1.2f)*(irisY*1.2f));
                            
                            if (irisDist <= irisRadius) {
                                // Iris gradient (16 levels)
                                float irisVal = irisDist / irisRadius;
                                int irisIdx;
                                
                                if (irisVal < 0.3f) {
                                    // Inner iris - darker
                                    irisIdx = 1 + (int)(irisVal * 10);
                                } else if (irisVal < 0.7f) {
                                    // Mid iris - colored (simulate hazel/green)
                                    irisIdx = 4 + (int)((irisVal - 0.3f) * 15);
                                } else {
                                    // Outer iris - darker ring
                                    irisIdx = 2 + (int)((1.0f - irisVal) * 8);
                                }
                                
                                if (irisIdx >= GRAY_LEVELS) irisIdx = GRAY_LEVELS - 1;
                                if (irisIdx < 0) irisIdx = 0;
                                
                                // Add radial iris texture
                                float angle = atan2(irisY, irisX);
                                float radialPattern = sin(angle * 12) * 0.5f + 0.5f;
                                irisIdx = irisIdx + (int)(radialPattern * 3) - 1;
                                if (irisIdx >= GRAY_LEVELS) irisIdx = GRAY_LEVELS - 1;
                                if (irisIdx < 0) irisIdx = 0;
                                
                                anim.Set_Char(drawY, drawX, grayChars[irisIdx]);
                                
                                // Pupil
                                if (irisDist <= pupilRadius) {
                                    // Pupil with slight edge softness
                                    float pupilVal = irisDist / pupilRadius;
                                    int pupilIdx = pupilVal > 0.8f ? 1 : 0;
                                    anim.Set_Char(drawY, drawX, grayChars[pupilIdx]);
                                }
                                
                                // Primary catchlight (specular highlight)
                                float hl1X = irisX - currentX * 0.2f + 3;
                                float hl1Y = irisY - currentY * 0.2f - 3;
                                if (sqrt(hl1X*hl1X + hl1Y*hl1Y) <= 2.5f) {
                                    anim.Set_Char(drawY, drawX, grayChars[15]);
                                }
                                
                                // Secondary catchlight (smaller, opposite)
                                float hl2X = irisX - currentX * 0.3f - 4;
                                float hl2Y = irisY - currentY * 0.3f + 2;
                                if (sqrt(hl2X*hl2X + hl2Y*hl2Y) <= 1.5f) {
                                    anim.Set_Char(drawY, drawX, grayChars[14]);
                                }
                            }
                        }
                    } else if (ellipseVal <= 1.15f) {
                        // Eyelid margin (dark line)
                        anim.Set_Char(drawY, drawX, grayChars[2]);
                    }
                }
            }
            
            // Draw eyelashes (upper)
            if (blinkPhase < 0.7f) {
                for (int x = -eyeRadiusX; x <= eyeRadiusX; x += 3) {
                    int lashX = centerX + x;
                    int lashY = centerY - eyeRadiusY - 1;
                    if (lashX >= 1 && lashX <= WIDTH && lashY >= 1 && lashY <= HEIGHT) {
                        anim.Set_Char(lashY, lashX, grayChars[3]);
                    }
                    // Longer lashes at edges
                    if (abs(x) > eyeRadiusX * 0.7f) {
                        anim.Set_Char(lashY-1, lashX, grayChars[2]);
                    }
                }
            }
            
            // Draw eyebrow
            for (int x = -eyeRadiusX - 6; x <= eyeRadiusX + 6; x++) {
                int browX = centerX + x;
                int browY = centerY - eyeRadiusY - 5 + (int)(sin(abs(x) * 0.15f) * 2);
                if (browX >= 1 && browX <= WIDTH && browY >= 1 && browY <= HEIGHT) {
                    float arch = 1.0f - (abs(x) / (eyeRadiusX + 6.0f));
                    int browIdx = 3 + (int)(arch * 4);
                    if (browIdx >= GRAY_LEVELS) browIdx = GRAY_LEVELS - 1;
                    anim.Set_Char(browY, browX, grayChars[browIdx]);
                }
            }
        }
        
        // Draw eye corners (medial canthus)
        for (int e = 0; e < 2; e++) {
            float centerX = eyes[e][0];
            int cornerX = (e == 0) ? centerX - eyeRadiusX - 1 : centerX + eyeRadiusX + 1;
            int cornerY = eyeY;
            if (cornerX >= 1 && cornerX <= WIDTH) {
                anim.Set_Char(cornerY, cornerX, grayChars[5]);
                anim.Set_Char(cornerY-1, cornerX, grayChars[6]);
                anim.Set_Char(cornerY+1, cornerX, grayChars[6]);
            }
        }
        
        // Draw bridge of nose
        for (int y = -4; y <= 4; y++) {
            int noseY = eyeY + y + 2;
            if (noseY >= 1 && noseY <= HEIGHT) {
                float shade = 1.0f - (abs(y) / 8.0f);
                int idx = 8 + (int)(shade * 4);
                anim.Set_Char(noseY, WIDTH/2, grayChars[idx]);
            }
        }
        
        // Status display
        char status[150];
        const char* mode = isSaccade ? "SACCADE" : (isPursuit ? "PURSUIT" : "FIXATION");
        snprintf(status, sizeof(status), 
                 "Mode: %-8s | Gaze: (%+5.1f,%+5.1f) | Blink: %.0f%% | Next blink: %.1fs", 
                 mode, currentX, currentY, blinkPhase * 100, 
                 std::max(0.0f, nextBlinkTime - blinkTimer));
        
        for (int i = 0; status[i]; i++) {
            anim.Set_Char(HEIGHT-2, i+2, (status[i] == ' ') ? ' ' : status[i]);
        }
        
        // Title
        const char* title = "REALISTIC EYE SIMULATION - 16-LEVEL GRAYSCALE DEPTH";
        for (int i = 0; title[i]; i++) {
            anim.Set_Char(2, (WIDTH-strlen(title))/2 + i, title[i]);
        }
        
        anim.Render_Frame();
        usleep(16667);  // ~60 FPS
    }
    
    return 0;
}