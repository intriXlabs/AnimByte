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
    
    anim.Initialise(WIDTH, HEIGHT);
    
    // 16-level grayscale
    const char* grayChars = "@%#*+=-:.`'^\"~_,; ";
    const int GRAY_LEVELS = 16;
    
    // Audio simulation parameters
    const int BANDS = 32;  // Frequency bands
    const int WAVEFORM_POINTS = 210;
    
    float frequencyBands[BANDS] = {0};
    float frequencyBandsSmoothed[BANDS] = {0};
    float waveform[WAVEFORM_POINTS] = {0};
    float waveformHistory[20][WAVEFORM_POINTS] = {0};
    int waveformHistoryIndex = 0;
    
    // Beat detection
    float beatEnergy = 0;
    float beatThreshold = 0.3f;
    bool isBeat = false;
    float beatFlash = 0;
    int beatCount = 0;
    float bpm = 128.0f;
    float beatTimer = 0;
    
    // Music simulation (virtual track progress)
    float songTime = 0;
    float songDuration = 240.0f;  // 4 minutes
    int sectionType = 0;  // 0=intro, 1=verse, 2=chorus, 3=bridge, 4=outro
    
    // Visual effects
    float rotationAngle = 0;
    float particlePhase = 0;
    float bassDropIntensity = 0;
    float stereoSpread = 0;
    
    // Equalizer bands (32 bands covering full spectrum)
    float bandFrequencies[BANDS];
    for (int i = 0; i < BANDS; i++) {
        bandFrequencies[i] = 20.0f * pow(2.0f, i / 4.0f);  // 20Hz to 20kHz
    }
    
    auto lastFrameTime = std::chrono::steady_clock::now();
    float globalTime = 0;
    int frameCount = 0;
    
    std::cout << "\033[2J\033[H";
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                   AUDIO VISUALIZER - 100 FPS                                                                                            ║\n";
    std::cout << "║                                                  32-Band EQ • Waveform • Beat Detection • Spectrum Analysis • Particle Effects                                                          ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n                                  Press Ctrl+C to exit | Simulating: Electronic Track - 128 BPM | 32-Band Spectrum Analyzer\n";
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
        if (songTime >= songDuration) songTime = 0;
        frameCount++;
        
        // Clear buffer
        for (int i = 1; i <= HEIGHT; i++)
            for (int j = 1; j <= WIDTH; j++)
                anim.Set_Char(i, j, ' ');
        
        // === SIMULATE AUDIO DATA ===
        
        // Determine song section
        float sectionProgress = fmod(songTime, 30.0f) / 30.0f;
        if (songTime < 30.0f) sectionType = 0;      // Intro
        else if (songTime < 90.0f) sectionType = 1;  // Verse
        else if (songTime < 150.0f) sectionType = 2; // Chorus
        else if (songTime < 180.0f) sectionType = 3; // Bridge
        else sectionType = 4;                         // Outro
        
        // Base amplitude by section
        float baseAmp = 0.3f;
        if (sectionType == 2) baseAmp = 0.8f;  // Chorus loud
        else if (sectionType == 0) baseAmp = 0.2f;  // Intro quiet
        else if (sectionType == 4) baseAmp = 0.4f;  // Outro fading
        
        // Beat timing (128 BPM = 0.46875 seconds per beat)
        beatTimer += deltaTime;
        float beatInterval = 60.0f / bpm;
        if (beatTimer >= beatInterval) {
            beatTimer -= beatInterval;
            isBeat = true;
            beatCount++;
            beatFlash = 1.0f;
            
            // Occasional bass drop
            if (beatCount % 32 == 0 && sectionType == 2) {
                bassDropIntensity = 1.0f;
            }
        } else {
            isBeat = false;
        }
        
        // Generate frequency bands (simulating FFT output)
        float totalEnergy = 0;
        for (int i = 0; i < BANDS; i++) {
            // Base frequency response (pink noise style - more energy in bass)
            float freqNormalized = i / (float)BANDS;
            float baseResponse = 1.0f / (freqNormalized * 3.0f + 0.5f);
            
            // Musical content simulation
            float musicalContent = 0;
            
            // Sub bass (20-60Hz)
            if (i < 4) {
                musicalContent = sin(songTime * 2.0f * PI * 0.5f) * 0.5f + 0.5f;
                if (isBeat) musicalContent = 1.0f;
            }
            // Bass (60-250Hz)
            else if (i < 8) {
                musicalContent = sin(songTime * 2.0f * PI * 1.0f) * 0.7f;
                if (isBeat) musicalContent += 0.5f;
            }
            // Low mids (250-500Hz)
            else if (i < 12) {
                musicalContent = sin(songTime * 2.0f * PI * 2.0f) * 0.5f;
                musicalContent += sin(songTime * 2.0f * PI * 4.0f) * 0.3f;
            }
            // Mids (500Hz-2kHz)
            else if (i < 20) {
                musicalContent = sin(songTime * 2.0f * PI * 8.0f + i) * 0.6f;
                musicalContent += cos(songTime * 3.0f * PI * 5.0f) * 0.3f;
            }
            // High mids (2kHz-8kHz)
            else if (i < 28) {
                musicalContent = sin(songTime * 2.0f * PI * 16.0f + i * 2.0f) * 0.4f;
                musicalContent += (rand() % 100) / 200.0f;  // Noise in highs
            }
            // Highs (8kHz+)
            else {
                musicalContent = (rand() % 100) / 150.0f;
                musicalContent += sin(songTime * 20.0f * PI) * 0.2f;
            }
            
            // Apply section envelope
            musicalContent *= baseAmp;
            
            // Bass drop effect
            if (bassDropIntensity > 0) {
                if (i < 6) musicalContent += bassDropIntensity;
                bassDropIntensity *= 0.95f;
            }
            
            // Random variation
            musicalContent += (rand() % 100) / 500.0f;
            
            frequencyBands[i] = musicalContent * baseResponse;
            if (frequencyBands[i] > 1.0f) frequencyBands[i] = 1.0f;
            
            // Smoothing
            frequencyBandsSmoothed[i] = frequencyBandsSmoothed[i] * 0.7f + frequencyBands[i] * 0.3f;
            
            totalEnergy += frequencyBands[i];
        }
        
        // Generate waveform
        for (int i = 0; i < WAVEFORM_POINTS; i++) {
            float t = i / (float)WAVEFORM_POINTS;
            float waveVal = 0;
            
            // Mix multiple frequencies
            waveVal += sin(t * 2.0f * PI * 2.0f + songTime * 10.0f) * 0.5f;
            waveVal += sin(t * 2.0f * PI * 4.0f + songTime * 15.0f) * 0.3f;
            waveVal += sin(t * 2.0f * PI * 8.0f + songTime * 20.0f) * 0.2f;
            waveVal += cos(t * 2.0f * PI * 16.0f) * 0.1f;
            
            // Add beat spikes
            if (isBeat) {
                waveVal += sin(t * 50.0f) * 0.3f * beatFlash;
            }
            
            waveform[i] = waveVal * baseAmp;
        }
        
        // Store waveform history
        for (int i = 0; i < WAVEFORM_POINTS; i++) {
            waveformHistory[waveformHistoryIndex][i] = waveform[i];
        }
        waveformHistoryIndex = (waveformHistoryIndex + 1) % 20;
        
        // Update beat energy
        beatEnergy = totalEnergy / BANDS;
        beatFlash *= 0.85f;
        
        // === RENDER VISUALIZER ===
        
        // 1. WAVEFORM DISPLAY (top section)
        int waveformY = 10;
        for (int h = 0; h < 20; h++) {
            int historyIdx = (waveformHistoryIndex - h - 1 + 20) % 20;
            float alpha = 1.0f - h / 20.0f;
            
            for (int x = 0; x < WAVEFORM_POINTS && x < WIDTH; x++) {
                float val = waveformHistory[historyIdx][x];
                int y = waveformY + (int)(val * 5);
                
                if (y >= 1 && y <= HEIGHT) {
                    int idx = (int)(alpha * (GRAY_LEVELS - 6)) + 5;
                    if (idx >= GRAY_LEVELS) idx = GRAY_LEVELS - 1;
                    anim.Set_Char(y, x + 1, grayChars[idx]);
                }
            }
        }
        
        // Draw center line for waveform
        for (int x = 1; x <= WIDTH; x++) {
            anim.Set_Char(waveformY, x, grayChars[10]);
        }
        
        // 2. FREQUENCY SPECTRUM (main equalizer)
        int spectrumStartY = 25;
        int spectrumHeight = 25;
        int barWidth = WIDTH / BANDS;
        
        for (int band = 0; band < BANDS; band++) {
            float value = frequencyBandsSmoothed[band];
            float rawValue = frequencyBands[band];
            int barHeight = (int)(value * spectrumHeight);
            
            // Bar position
            int barX = band * barWidth;
            
            // Draw bar with gradient
            for (int h = 0; h < barHeight; h++) {
                int y = spectrumStartY + spectrumHeight - h;
                if (y >= 1 && y <= HEIGHT) {
                    // Gradient based on height and frequency
                    float gradientPos = h / (float)spectrumHeight;
                    float freqPos = band / (float)BANDS;
                    
                    int charIdx;
                    if (freqPos < 0.2f) {
                        // Bass - dark and solid
                        charIdx = (int)(gradientPos * 6);
                    } else if (freqPos < 0.5f) {
                        // Mids - medium
                        charIdx = 3 + (int)(gradientPos * 8);
                    } else {
                        // Highs - lighter
                        charIdx = 8 + (int)(gradientPos * 6);
                    }
                    
                    if (charIdx >= GRAY_LEVELS) charIdx = GRAY_LEVELS - 1;
                    if (charIdx < 0) charIdx = 0;
                    
                    for (int bx = 0; bx < barWidth - 1; bx++) {
                        anim.Set_Char(y, barX + bx + 1, grayChars[charIdx]);
                    }
                }
            }
            
            // Draw peak hold
            int peakY = spectrumStartY + spectrumHeight - barHeight;
            if (peakY >= 1 && peakY <= HEIGHT) {
                for (int bx = 0; bx < barWidth - 1; bx++) {
                    anim.Set_Char(peakY, barX + bx + 1, '@');
                }
            }
            
            // Draw raw value as brighter overlay
            int rawHeight = (int)(rawValue * spectrumHeight);
            int rawY = spectrumStartY + spectrumHeight - rawHeight;
            if (rawY >= 1 && rawY <= HEIGHT && rawHeight > 0) {
                for (int bx = 0; bx < barWidth - 2; bx++) {
                    anim.Set_Char(rawY, barX + bx + 1, '#');
                }
            }
        }
        
        // Spectrum baseline
        for (int x = 1; x <= WIDTH; x++) {
            anim.Set_Char(spectrumStartY + spectrumHeight, x, grayChars[2]);
        }
        
        // 3. CIRCULAR SPECTRUM (polar coordinates)
        float centerX = WIDTH / 2.0f;
        float centerY = HEIGHT / 2.0f + 5;
        rotationAngle += deltaTime * 0.5f;
        
        for (int band = 0; band < BANDS; band++) {
            float value = frequencyBandsSmoothed[band];
            float angle = band * 2.0f * PI / BANDS + rotationAngle;
            float radius = 8.0f + value * 15.0f;
            
            int x = centerX + (int)(cos(angle) * radius);
            int y = centerY + (int)(sin(angle) * radius * 0.5f);
            
            if (x >= 1 && x <= WIDTH && y >= 1 && y <= HEIGHT) {
                int idx = (int)(value * 10) + 4;
                if (idx >= GRAY_LEVELS) idx = GRAY_LEVELS - 1;
                anim.Set_Char(y, x, grayChars[idx]);
            }
            
            // Draw connecting lines
            float nextAngle = ((band + 1) % BANDS) * 2.0f * PI / BANDS + rotationAngle;
            float nextRadius = 8.0f + frequencyBandsSmoothed[(band + 1) % BANDS] * 15.0f;
            int x2 = centerX + (int)(cos(nextAngle) * nextRadius);
            int y2 = centerY + (int)(sin(nextAngle) * nextRadius * 0.5f);
            
            int steps = (int)sqrt(pow(x2 - x, 2) + pow(y2 - y, 2));
            for (int s = 0; s <= steps; s++) {
                float t = s / (float)steps;
                int lx = x + (int)((x2 - x) * t);
                int ly = y + (int)((y2 - y) * t);
                if (lx >= 1 && lx <= WIDTH && ly >= 1 && ly <= HEIGHT) {
                    anim.Set_Char(ly, lx, grayChars[8]);
                }
            }
        }
        
        // 4. BEAT REACTIVE PARTICLES
        particlePhase += deltaTime * 5.0f;
        
        if (isBeat) {
            // Spawn particles on beat
            for (int p = 0; p < 20; p++) {
                float angle = (rand() % 360) * PI / 180.0f;
                float speed = 2.0f + (rand() % 100) / 50.0f;
                int px = centerX + (int)(cos(angle) * speed * (1.0f + beatFlash));
                int py = centerY + (int)(sin(angle) * speed * 0.5f * (1.0f + beatFlash));
                
                if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                    anim.Set_Char(py, px, '*');
                }
            }
        }
        
        // Ambient particles
        for (int p = 0; p < 30; p++) {
            float t = p / 30.0f + particlePhase;
            float radius = 12.0f + sin(t * 3.0f) * 3.0f;
            int px = centerX + (int)(cos(t * 2.0f) * radius);
            int py = centerY + (int)(sin(t * 3.0f) * radius * 0.5f);
            
            if (px >= 1 && px <= WIDTH && py >= 1 && py <= HEIGHT) {
                anim.Set_Char(py, px, grayChars[12]);
            }
        }
        
        // 5. STEREO VU METERS
        int vuY = 40;
        float leftChannel = 0, rightChannel = 0;
        for (int i = 0; i < BANDS/2; i++) {
            leftChannel += frequencyBandsSmoothed[i];
            rightChannel += frequencyBandsSmoothed[i + BANDS/2];
        }
        leftChannel /= (BANDS/2);
        rightChannel /= (BANDS/2);
        
        int leftWidth = (int)(leftChannel * 30);
        int rightWidth = (int)(rightChannel * 30);
        
        // Left channel
        for (int i = 0; i < leftWidth; i++) {
            anim.Set_Char(vuY, 5 + i, '>');
        }
        anim.Set_Char(vuY, 4, 'L');
        
        // Right channel
        for (int i = 0; i < rightWidth; i++) {
            anim.Set_Char(vuY, WIDTH - 5 - i, '<');
        }
        anim.Set_Char(vuY, WIDTH - 4, 'R');
        
        // 6. BEAT INDICATOR
        if (isBeat || beatFlash > 0.1f) {
            int flashSize = (int)(beatFlash * 10);
            for (int dy = -flashSize; dy <= flashSize; dy++) {
                for (int dx = -flashSize; dx <= flashSize; dx++) {
                    int fx = centerX + dx;
                    int fy = centerY + dy;
                    if (fx >= 1 && fx <= WIDTH && fy >= 1 && fy <= HEIGHT) {
                        float dist = sqrt(dx*dx + dy*dy);
                        if (dist < flashSize) {
                            anim.Set_Char(fy, fx, '@');
                        }
                    }
                }
            }
        }
        
        // 7. PROGRESS BAR
        float progress = songTime / songDuration;
        int progressWidth = (int)(progress * (WIDTH - 10));
        for (int i = 0; i < WIDTH - 10; i++) {
            char ch = (i < progressWidth) ? '=' : '-';
            anim.Set_Char(HEIGHT - 2, i + 6, ch);
        }
        anim.Set_Char(HEIGHT - 2, 4, '[');
        anim.Set_Char(HEIGHT - 2, WIDTH - 4, ']');
        
        // 8. HUD INFORMATION
        char hud1[100];
        const char* sections[] = {"INTRO", "VERSE", "CHORUS", "BRIDGE", "OUTRO"};
        snprintf(hud1, sizeof(hud1), 
                 "BPM: %.0f | Beat: %d | Section: %s | Energy: %.2f | Bass: %.2f", 
                 bpm, beatCount, sections[sectionType], totalEnergy, frequencyBandsSmoothed[2]);
        
        for (int i = 0; hud1[i]; i++) {
            anim.Set_Char(1, i + 2, hud1[i]);
        }
        
        // Frequency labels
        const char* freqLabels[] = {"20Hz", "100Hz", "500Hz", "2kHz", "8kHz", "20kHz"};
        for (int i = 0; i < 6; i++) {
            int labelX = i * (WIDTH / 6) + 2;
            for (int j = 0; freqLabels[i][j]; j++) {
                anim.Set_Char(spectrumStartY + spectrumHeight + 1, labelX + j, freqLabels[i][j]);
            }
        }
        
        // Title
        const char* title = "◈ AUDIO VISUALIZER ◈ 32-Band EQ • Waveform • Beat Detection • Spectrum Analyzer ◈";
        for (int i = 0; title[i]; i++) {
            anim.Set_Char(HEIGHT, i + 2, title[i]);
        }
        
        // Decibel scale
        for (int i = 0; i <= spectrumHeight; i++) {
            char dbChar = (i % 5 == 0) ? '-' : ' ';
            anim.Set_Char(spectrumStartY + spectrumHeight - i, WIDTH - 2, dbChar);
        }
        anim.Set_Char(spectrumStartY, WIDTH - 1, '0');
        anim.Set_Char(spectrumStartY + spectrumHeight, WIDTH - 1, '-');
        anim.Set_Char(spectrumStartY + spectrumHeight - 5, WIDTH - 1, '6');
        anim.Set_Char(spectrumStartY + spectrumHeight - 10, WIDTH - 1, '1');
        anim.Set_Char(spectrumStartY + spectrumHeight - 15, WIDTH - 1, '2');
        anim.Set_Char(spectrumStartY + spectrumHeight - 20, WIDTH - 1, '3');
        
        anim.Render_Frame();
        
        // 100 FPS timing enforcement
        auto frameEnd = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float>(frameEnd - currentTime).count();
        if (frameTime < 0.01f) {
            usleep((0.01f - frameTime) * 1000000);
        }
    }
    
    return 0;
}