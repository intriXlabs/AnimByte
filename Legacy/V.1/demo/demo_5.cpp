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
    const float PI = 3.14159265358979323846f;
    const float PHI = 1.618033988749895f;
    
    anim.Initialise(WIDTH, HEIGHT);
    
    // 16-level grayscale
    const char* grayChars = "@%#*+=-:.`'^\"~_,; ";
    const int GRAY_LEVELS = 16;
    
    // Audio simulation
    const int BANDS = 24;
    float frequencyBands[BANDS] = {0};
    float frequencyBandsSmoothed[BANDS] = {0};
    float waveform[210] = {0};
    
    // Beat detection
    float beatEnergy = 0;
    float beatFlash = 0;
    bool isBeat = false;
    int beatCount = 0;
    float bpm = 128.0f;
    float beatTimer = 0;
    float songTime = 0;
    
    // THE ORB - Main attraction
    float orbX = WIDTH / 2.0f;
    float orbY = HEIGHT / 2.0f;
    float orbRadius = 12.0f;
    float orbTargetRadius = 12.0f;
    float orbPulse = 1.0f;
    float orbRotation = 0.0f;
    float orbDeform = 0.0f;
    float orbIntensity = 0.5f;
    
    // Rings around the orb
    const int NUM_RINGS = 8;
    float ringRadii[NUM_RINGS] = {16, 20, 24, 28, 32, 36, 40, 44};
    float ringRotations[NUM_RINGS] = {0};
    float ringSpeeds[NUM_RINGS] = {0.5f, -0.3f, 0.7f, -0.4f, 0.6f, -0.5f, 0.8f, -0.6f};
    float ringThickness[NUM_RINGS] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    int ringPoints[NUM_RINGS] = {32, 40, 48, 56, 64, 72, 80, 88};
    
    // Orbiting particles
    const int ORBITERS = 40;
    float orbiterAngles[ORBITERS];
    float orbiterRadii[ORBITERS];
    float orbiterSpeeds[ORBITERS];
    float orbiterSizes[ORBITERS];
    
    // Energy waves
    const int MAX_WAVES = 10;
    float waveRadii[MAX_WAVES];
    float waveAlphas[MAX_WAVES];
    float waveThickness[MAX_WAVES];
    int waveCount = 0;
    
    // Bass drop shockwaves
    const int MAX_SHOCKWAVES = 5;
    float shockwaveRadii[MAX_SHOCKWAVES];
    float shockwaveAlphas[MAX_SHOCKWAVES];
    float shockwavePower[MAX_SHOCKWAVES];
    int shockwaveCount = 0;
    
    // Frequency-driven tentacles/rays
    const int RAYS = 36;
    float rayLengths[RAYS] = {0};
    float rayTargetLengths[RAYS] = {0};
    float rayAngles[RAYS];
    
    // Initialize
    for (int i = 0; i < ORBITERS; i++) {
        orbiterAngles[i] = (rand() % 360) * PI / 180.0f;
        orbiterRadii[i] = 18.0f + (rand() % 20);
        orbiterSpeeds[i] = 0.5f + (rand() % 100) / 100.0f;
        orbiterSizes[i] = 1.0f + (rand() % 3);
    }
    
    for (int i = 0; i < RAYS; i++) {
        rayAngles[i] = i * 10.0f * PI / 180.0f;
    }
    
    for (int i = 0; i < MAX_WAVES; i++) {
        waveAlphas[i] = 0;
    }
    
    for (int i = 0; i < MAX_SHOCKWAVES; i++) {
        shockwaveAlphas[i] = 0;
    }
    
    auto lastFrameTime = std::chrono::steady_clock::now();
    float globalTime = 0;
    int frameCount = 0;
    float bassAccumulator = 0;
    
    std::cout << "\033[2J\033[H";
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                  ORB VISUALIZER - 100 FPS                                                                                              ║\n";
    std::cout << "║                                              Central Pulsating Orb • Reactive Rings • Energy Waves • Particle Orbiters • Shockwaves                                                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n                                Press Ctrl+C to exit | Watch the orb pulse and react to the music | 128 BPM Electronic\n";
    usleep(2000000);
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        
        // 100 FPS cap
        if (deltaTime < 0.01f) {
            usleep((0.01f - deltaTime) * 1000000);
            deltaTime = 0.01f;
        }
        
        globalTime += deltaTime;
        songTime += deltaTime;
        frameCount++;
        
        // Clear buffer
        for (int i = 1; i <= HEIGHT; i++)
            for (int j = 1; j <= WIDTH; j++)
                anim.Set_Char(i, j, ' ');
        
        // === SIMULATE AUDIO ===
        
        // Beat timing
        beatTimer += deltaTime;
        float beatInterval = 60.0f / bpm;
        isBeat = false;
        
        if (beatTimer >= beatInterval) {
            beatTimer -= beatInterval;
            isBeat = true;
            beatCount++;
            beatFlash = 1.0f;
            beatEnergy = 1.0f;
            
            // Trigger energy wave on beat
            waveRadii[waveCount % MAX_WAVES] = orbRadius + 5;
            waveAlphas[waveCount % MAX_WAVES] = 1.0f;
            waveThickness[waveCount % MAX_WAVES] = 1.5f;
            waveCount++;
            
            // Accumulate bass for shockwave
            bassAccumulator += 0.25f;
        } else {
            beatEnergy *= 0.9f;
        }
        
        // Generate frequency bands
        float totalBass = 0;
        for (int i = 0; i < BANDS; i++) {
            float t = songTime;
            float bandVal = 0;
            
            // Different frequency characteristics
            if (i < 6) {  // Bass
                bandVal = sin(t * 2.0f * PI * 1.0f) * 0.5f + 0.5f;
                if (isBeat) bandVal = 1.0f;
                bandVal += sin(t * 4.0f * PI) * 0.3f;
                totalBass += bandVal;
            } else if (i < 12) {  // Low mids
                bandVal = sin(t * 2.0f * PI * 2.0f + i) * 0.4f + 0.3f;
                bandVal += cos(t * 3.0f * PI) * 0.2f;
            } else if (i < 18) {  // High mids
                bandVal = sin(t * 2.0f * PI * 4.0f + i * 2.0f) * 0.3f + 0.2f;
                bandVal += sin(t * 8.0f * PI) * 0.15f;
            } else {  // Highs
                bandVal = (rand() % 100) / 200.0f;
                bandVal += sin(t * 16.0f * PI) * 0.1f;
            }
            
            // Add noise and variation
            bandVal += (rand() % 100) / 400.0f;
            if (bandVal > 1.0f) bandVal = 1.0f;
            
            frequencyBands[i] = bandVal;
            frequencyBandsSmoothed[i] = frequencyBandsSmoothed[i] * 0.7f + bandVal * 0.3f;
        }
        
        // Normalize total bass
        totalBass /= 6.0f;
        
        // Trigger shockwave on bass accumulation
        if (bassAccumulator >= 1.0f) {
            shockwaveRadii[shockwaveCount % MAX_SHOCKWAVES] = orbRadius;
            shockwaveAlphas[shockwaveCount % MAX_SHOCKWAVES] = 1.0f;
            shockwavePower[shockwaveCount % MAX_SHOCKWAVES] = bassAccumulator;
            shockwaveCount++;
            bassAccumulator = 0;
            orbTargetRadius = 18.0f;  // Expand orb
        }
        
        beatFlash *= 0.85f;
        
        // === UPDATE ORB ===
        
        // Orb pulse based on bass
        orbPulse = 0.8f + totalBass * 0.6f;
        orbTargetRadius = 10.0f + totalBass * 12.0f + beatFlash * 5.0f;
        orbRadius += (orbTargetRadius - orbRadius) * 0.1f;
        
        // Orb rotation from highs
        float highFreqAvg = 0;
        for (int i = 12; i < BANDS; i++) highFreqAvg += frequencyBandsSmoothed[i];
        highFreqAvg /= (BANDS - 12);
        orbRotation += deltaTime * (0.5f + highFreqAvg * 2.0f);
        
        // Orb deformation from frequency spread
        orbDeform = 0;
        for (int i = 0; i < BANDS; i++) {
            orbDeform += frequencyBandsSmoothed[i] * sin(i * 0.5f);
        }
        orbDeform = fabs(orbDeform) / BANDS * 3.0f;
        
        // Orb intensity
        orbIntensity = 0.4f + totalBass * 0.6f;
        
        // Update ray lengths based on frequencies
        for (int i = 0; i < RAYS; i++) {
            int bandIndex = (i * BANDS / RAYS) % BANDS;
            rayTargetLengths[i] = 5.0f + frequencyBandsSmoothed[bandIndex] * 25.0f;
            rayLengths[i] += (rayTargetLengths[i] - rayLengths[i]) * 0.15f;
        }
        
        // Update rings
        for (int i = 0; i < NUM_RINGS; i++) {
            ringRotations[i] += deltaTime * ringSpeeds[i] * (1.0f + highFreqAvg);
            
            // Ring radius responds to specific frequencies
            int bandIdx = (i * 3) % BANDS;
            float freqResponse = frequencyBandsSmoothed[bandIdx];
            float targetRadius = 16.0f + i * 4.0f + freqResponse * 8.0f;
            ringRadii[i] += (targetRadius - ringRadii[i]) * 0.05f;
            
            // Ring thickness pulses with beat
            ringThickness[i] = 1.0f + beatFlash * (1.0f + i * 0.2f);
        }
        
        // Update orbiters
        for (int i = 0; i < ORBITERS; i++) {
            orbiterAngles[i] += deltaTime * orbiterSpeeds[i] * (0.5f + totalBass);
            
            // Orbiter distance affected by music
            int bandIdx = i % BANDS;
            float distanceMod = 1.0f + frequencyBandsSmoothed[bandIdx] * 0.5f;
            orbiterRadii[i] = 15.0f + (i % 5) * 5.0f * distanceMod;
        }
        
        // Update energy waves
        for (int i = 0; i < MAX_WAVES; i++) {
            if (waveAlphas[i] > 0) {
                waveRadii[i] += deltaTime * 30.0f;
                waveAlphas[i] -= deltaTime * 0.8f;
                if (waveAlphas[i] < 0) waveAlphas[i] = 0;
            }
        }
        
        // Update shockwaves
        for (int i = 0; i < MAX_SHOCKWAVES; i++) {
            if (shockwaveAlphas[i] > 0) {
                shockwaveRadii[i] += deltaTime * 50.0f * shockwavePower[i];
                shockwaveAlphas[i] -= deltaTime * 0.5f;
                if (shockwaveAlphas[i] < 0) shockwaveAlphas[i] = 0;
            }
        }
        
        // === RENDER ===
        
        // 1. Draw frequency-driven rays (behind orb)
        for (int r = 0; r < RAYS; r++) {
            float angle = rayAngles[r] + orbRotation * 0.3f;
            float length = rayLengths[r];
            
            for (float l = orbRadius; l < orbRadius + length; l += 1.0f) {
                float t = (l - orbRadius) / length;
                float intensity = (1.0f - t) * 0.5f;
                
                int px = orbX + (int)(cos(angle) * l);
                int py = orbY + (int)(sin(angle) * l * 0.5f);
                
                if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                    int idx = 5 + (int)(intensity * 8);
                    if (idx < GRAY_LEVELS) {
                        anim.Set_Char(py, px, grayChars[idx]);
                    }
                }
            }
        }
        
        // 2. Draw shockwaves (behind orb, large impact)
        for (int i = 0; i < MAX_SHOCKWAVES; i++) {
            if (shockwaveAlphas[i] > 0.01f) {
                float radius = shockwaveRadii[i];
                float alpha = shockwaveAlphas[i];
                
                for (int a = 0; a < 360; a += 3) {
                    float angle = a * PI / 180.0f;
                    
                    for (int t = -2; t <= 2; t++) {
                        float r = radius + t;
                        int px = orbX + (int)(cos(angle) * r);
                        int py = orbY + (int)(sin(angle) * r * 0.5f);
                        
                        if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                            int idx = (int)(alpha * 10) + 2;
                            if (idx < GRAY_LEVELS) {
                                anim.Set_Char(py, px, grayChars[idx]);
                            }
                        }
                    }
                }
            }
        }
        
        // 3. Draw rings
        for (int ring = 0; ring < NUM_RINGS; ring++) {
            float radius = ringRadii[ring];
            float rotation = ringRotations[ring];
            int points = ringPoints[ring];
            
            for (int p = 0; p < points; p++) {
                float angle = p * 2.0f * PI / points + rotation;
                
                // Ring wobble from audio
                int wobbleBand = (ring * 2 + p) % BANDS;
                float wobble = frequencyBandsSmoothed[wobbleBand] * 2.0f;
                float r = radius + sin(angle * 3.0f + globalTime) * wobble;
                
                int px = orbX + (int)(cos(angle) * r);
                int py = orbY + (int)(sin(angle) * r * 0.5f);
                
                if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                    // Ring brightness based on frequency
                    int bandIdx = (ring * 3) % BANDS;
                    float bright = frequencyBandsSmoothed[bandIdx];
                    int idx = 4 + (int)(bright * 8);
                    if (idx >= GRAY_LEVELS) idx = GRAY_LEVELS - 1;
                    
                    anim.Set_Char(py, px, grayChars[idx]);
                    
                    // Thicker rings on beat
                    if (ringThickness[ring] > 1.2f) {
                        anim.Set_Char(py-1, px, grayChars[idx-1]);
                        anim.Set_Char(py+1, px, grayChars[idx-1]);
                    }
                }
            }
        }
        
        // 4. Draw orbiters (particles circling the orb)
        for (int i = 0; i < ORBITERS; i++) {
            float angle = orbiterAngles[i];
            float radius = orbiterRadii[i];
            
            int px = orbX + (int)(cos(angle) * radius);
            int py = orbY + (int)(sin(angle) * radius * 0.5f);
            
            if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                char orbiterChar = (i % 3 == 0) ? 'o' : ((i % 3 == 1) ? '*' : '+');
                anim.Set_Char(py, px, orbiterChar);
                
                // Trail
                int trailX = orbX + (int)(cos(angle - 0.2f) * radius);
                int trailY = orbY + (int)(sin(angle - 0.2f) * radius * 0.5f);
                if (trailX >= 1 && trailX <= WIDTH && trailY >= 1 && trailY <= HEIGHT) {
                    anim.Set_Char(trailY, trailX, grayChars[12]);
                }
            }
        }
        
        // 5. Draw energy waves
        for (int i = 0; i < MAX_WAVES; i++) {
            if (waveAlphas[i] > 0.01f) {
                float radius = waveRadii[i];
                float alpha = waveAlphas[i];
                
                for (int a = 0; a < 360; a += 4) {
                    float angle = a * PI / 180.0f + globalTime;
                    int px = orbX + (int)(cos(angle) * radius);
                    int py = orbY + (int)(sin(angle) * radius * 0.5f);
                    
                    if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                        int idx = 8 + (int)(alpha * 6);
                        if (idx < GRAY_LEVELS) {
                            anim.Set_Char(py, px, grayChars[idx]);
                        }
                    }
                }
            }
        }
        
        // 6. THE MAIN ORB - the centerpiece
        for (int dy = -orbRadius - 3; dy <= orbRadius + 3; dy++) {
            for (int dx = -orbRadius - 3; dx <= orbRadius + 3; dx++) {
                int px = orbX + dx;
                int py = orbY + dy;
                
                if (px < 1 || px > WIDTH || py < 1 || py > HEIGHT) continue;
                
                float dist = sqrt(dx*dx + dy*dy);
                
                // Deformed orb shape (reacts to audio)
                float angle = atan2(dy, dx);
                float deformOffset = orbDeform * sin(angle * 5.0f + orbRotation) * cos(angle * 3.0f);
                float effectiveRadius = orbRadius * orbPulse + deformOffset;
                
                if (dist <= effectiveRadius) {
                    // Inner orb with gradient
                    float normDist = dist / effectiveRadius;
                    
                    // Texture based on frequency content
                    float texture = 0;
                    for (int b = 0; b < 4; b++) {
                        texture += sin(angle * (b + 1) * PHI + globalTime * 2.0f) * frequencyBandsSmoothed[b * 6];
                    }
                    texture = fabs(texture) / 4.0f;
                    
                    // Core brightness
                    float brightness = 1.0f - normDist * 0.7f;
                    brightness += texture * 0.3f;
                    brightness *= (0.6f + orbIntensity * 0.5f);
                    
                    // Beat flash adds brightness
                    brightness += beatFlash * (1.0f - normDist) * 0.5f;
                    
                    if (brightness > 1.0f) brightness = 1.0f;
                    
                    int idx = (int)(brightness * (GRAY_LEVELS - 1));
                    if (idx < 0) idx = 0;
                    if (idx >= GRAY_LEVELS) idx = GRAY_LEVELS - 1;
                    
                    // Special center highlight
                    if (dist < 3.0f) {
                        idx = GRAY_LEVELS - 1 - (int)(dist / 3.0f * 4);
                        if (idx < 0) idx = 0;
                    }
                    
                    anim.Set_Char(py, px, grayChars[idx]);
                    
                    // Add specular highlight
                    if (dist > effectiveRadius * 0.7f && dist < effectiveRadius * 0.9f) {
                        float highlightAngle = fmod(angle - orbRotation * 2.0f, 2.0f * PI);
                        if (highlightAngle > 5.0f && highlightAngle < 5.5f) {
                            anim.Set_Char(py, px, grayChars[14]);
                        }
                    }
                }
            }
        }
        
        // 7. Orb corona/glow
        for (int a = 0; a < 360; a += 5) {
            float angle = a * PI / 180.0f + orbRotation;
            float coronaRadius = orbRadius + 3.0f + beatFlash * 5.0f;
            
            for (int g = 0; g < 3; g++) {
                int px = orbX + (int)(cos(angle + g * 0.3f) * coronaRadius);
                int py = orbY + (int)(sin(angle + g * 0.3f) * coronaRadius * 0.5f);
                
                if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                    int idx = 10 + g * 2;
                    if (idx < GRAY_LEVELS) {
                        anim.Set_Char(py, px, grayChars[idx]);
                    }
                }
            }
        }
        
        // 8. Bottom frequency bars (subtle)
        for (int i = 0; i < BANDS; i++) {
            int barHeight = (int)(frequencyBandsSmoothed[i] * 8);
            int barX = i * (WIDTH / BANDS) + 2;
            
            for (int h = 0; h < barHeight; h++) {
                int barY = HEIGHT - 2 - h;
                if (barY >= 1) {
                    int idx = 8 + h;
                    if (idx < GRAY_LEVELS) {
                        anim.Set_Char(barY, barX, grayChars[idx]);
                    }
                }
            }
        }
        
        // 9. HUD Information
        char hud[100];
        snprintf(hud, sizeof(hud), 
                 "BPM: %.0f | Beat: %d | Bass: %.2f | Orb Radius: %.1f | Deform: %.2f | Energy: %.2f", 
                 bpm, beatCount, totalBass, orbRadius, orbDeform, beatEnergy);
        
        for (int i = 0; hud[i]; i++) {
            anim.Set_Char(1, i + 2, hud[i]);
        }
        
        // Title
        const char* title = "◈ THE ORB ◈ Frequency-Reactive • Pulsating Core • Energy Rings • Shockwaves • Particle Orbiters ◈";
        for (int i = 0; title[i]; i++) {
            anim.Set_Char(HEIGHT, i + 2, title[i]);
        }
        
        // Beat indicator in corner
        if (isBeat) {
            anim.Set_Char(2, WIDTH - 10, 'B');
            anim.Set_Char(2, WIDTH - 9, 'E');
            anim.Set_Char(2, WIDTH - 8, 'A');
            anim.Set_Char(2, WIDTH - 7, 'T');
            anim.Set_Char(2, WIDTH - 6, '!');
        }
        
        // Ring count indicator
        char ringInfo[30];
        snprintf(ringInfo, sizeof(ringInfo), "Rings: %d | Orbiters: %d", NUM_RINGS, ORBITERS);
        for (int i = 0; ringInfo[i]; i++) {
            anim.Set_Char(2, WIDTH - 30 + i, ringInfo[i]);
        }
        
        anim.Render_Frame();
        
        // 100 FPS timing
        auto frameEnd = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float>(frameEnd - currentTime).count();
        if (frameTime < 0.01f) {
            usleep((0.01f - frameTime) * 1000000);
        }
    }
    
    return 0;
}