// animbyte is a opensource 1byte character based rendering engine to provide high speed animation with easy api access

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <sys/ioctl.h>
#include <unistd.h>

class AnimByte{
public:
    int Initialise(int width, int height){
        Allocate_Buffer(width, height);
        return 0;
    }

    int Set_Char(int row, int col, char ch){
        row=row-1,col=col-1;
        if(row < 0 || row >= Height || col < 0 || col >= Width){
            return -1;
        }

        if(Buffer[row * Width + col] == ch)
            return -1;

        Buffer[row * Width + col] = ch;
        List.push_back({row, col});
        return 0;
    }

    int Render_Frame(){
        Generate_Frame();
        if(Frame.empty())
            return -1;
        render(Frame);
        return 0;
    }

    int Frame_Clean(){
        for(int i = 0; i < List.size(); i++){
            Set_Char(List[i].row, List[i].col, ' ');
        }
        return 0;
    }

    int GetWidth() const { return Width; }
    int GetHeight() const { return Height; }

private:
    struct Clean_List{
        int row, col;
    };
    char *Buffer;
    int Width, Height;
    std::string Frame;
    std::vector <Clean_List> List;

    int Allocate_Buffer(int width, int height){
        Width = width;
        Height = height;
        Buffer = new char[Width * Height];
        for(int i = 0; i < Width * Height; i++){
            Buffer[i] = ' ';
        }
        return 0;
    }

    int Generate_Frame(){
        Frame.clear();
        
        for(int i = 0; i < Width * Height; i++){
            Frame += Buffer[i];
            if((i + 1) % Width == 0){
                Frame += '\n';
            }
        }
        return 0;
    }

    int render(const std::string &frame){
        const char *home = "\033[H";
        write(1, home, 3);
        write(1, frame.c_str(), frame.size());
        return 0;
    }
};

// Julia Set Morpher - Morphs between different Julia set parameters
class JuliaMorpher {
private:
    AnimByte& engine;
    int width, height;
    
    // Julia set parameters that morph over time
    double cReal, cImag;
    double targetReal, targetImag;
    double morphProgress;
    
    // Julia set configurations (famous Julia sets)
    struct JuliaConfig {
        const char* name;
        double real;
        double imag;
    };
    
    JuliaConfig configs[8] = {
        {"Classic", -0.74543, 0.11301},      // Classic Julia
        {"Dendrite", 0.0, 1.0},              // Dendrite fractal
        {"Cauliflower", -0.75, 0.0},         // Cauliflower
        {"San Marco", -0.75, 0.1},           // San Marco
        {"Siegel Disk", -0.391, -0.587},     // Siegel Disk
        {"Douady Rabbit", -0.123, 0.745},    // Douady's Rabbit
        {"Ship", -0.5, -0.55},               // Burning Ship variant
        {"Spiral", 0.285, 0.01}              // Spiral Julia
    };
    
    int currentConfig, nextConfig;
    double zoom;
    double offsetX, offsetY;
    double time;
    
    // ASCII gradient (1-byte characters from darkest to brightest)
    const char gradient[12] = {' ', '.', ',', '-', '~', '+', '=', '*', '#', '&', '%', '@'};
    
public:
    JuliaMorpher(AnimByte& eng) : engine(eng), width(engine.GetWidth()), height(engine.GetHeight()) {
        currentConfig = 0;
        nextConfig = 1;
        morphProgress = 0;
        cReal = configs[0].real;
        cImag = configs[0].imag;
        targetReal = configs[1].real;
        targetImag = configs[1].imag;
        zoom = 1.0;
        offsetX = 0;
        offsetY = 0;
        time = 0;
    }
    
    void update() {
        time += 0.016; // ~60 FPS
        
        // Morph between Julia configurations
        morphProgress += 0.0025; // Slow morph over ~8 seconds per transition
        if(morphProgress >= 1.0) {
            morphProgress = 0;
            currentConfig = (currentConfig + 1) % 8;
            nextConfig = (currentConfig + 1) % 8;
            
            // Set new targets
            targetReal = configs[nextConfig].real;
            targetImag = configs[nextConfig].imag;
            
            // Gentle zoom and pan for cinematic effect
            zoom = 1.0;
            offsetX = 0;
            offsetY = 0;
        }
        
        // Smooth interpolation between configurations
        double t = morphProgress;
        // Ease in-out cubic for smooth morphing
        t = t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
        
        cReal = configs[currentConfig].real * (1 - t) + targetReal * t;
        cImag = configs[currentConfig].imag * (1 - t) + targetImag * t;
        
        // Dynamic zoom and pan based on time
        zoom = 1.0 + sin(time * 0.1) * 0.2;
        offsetX = sin(time * 0.07) * 0.3;
        offsetY = cos(time * 0.05) * 0.2;
    }
    
    // Calculate Julia set iteration count for a point
    int juliaPoint(double x, double y, int maxIter) {
        double zx = x;
        double zy = y;
        
        for(int i = 0; i < maxIter; i++) {
            double zx2 = zx * zx;
            double zy2 = zy * zy;
            
            if(zx2 + zy2 > 4.0) {
                return i;
            }
            
            double temp = zx2 - zy2 + cReal;
            zy = 2.0 * zx * zy + cImag;
            zx = temp;
        }
        return maxIter;
    }
    
    void draw() {
        engine.Frame_Clean();
        
        int maxIter = 256;
        
        // Julia set rendering bounds
        double xmin = -2.0 / zoom + offsetX;
        double xmax = 2.0 / zoom + offsetX;
        double ymin = -1.5 / zoom + offsetY;
        double ymax = 1.5 / zoom + offsetY;
        
        double xstep = (xmax - xmin) / width;
        double ystep = (ymax - ymin) / height;
        
        // Multi-threaded rendering simulation (sequential but optimized)
        for(int y = 0; y < height; y++) {
            double cy = ymin + y * ystep;
            
            for(int x = 0; x < width; x++) {
                double cx = xmin + x * xstep;
                
                int iter = juliaPoint(cx, cy, maxIter);
                
                if(iter < maxIter) {
                    // Map iteration count to gradient character
                    int gradientIndex = (iter * 12) / maxIter;
                    gradientIndex = std::min(11, std::max(0, gradientIndex));
                    
                    // Add subtle animation - iterations vary with time
                    int timeShift = (int)(time * 2) % 12;
                    gradientIndex = (gradientIndex + timeShift / 3) % 12;
                    
                    engine.Set_Char(y + 1, x + 1, gradient[gradientIndex]);
                } else {
                    // Inside the set - draw as '@'
                    engine.Set_Char(y + 1, x + 1, '@');
                }
            }
        }
        
        // Draw border
        for(int i = 1; i <= width; i++) {
            engine.Set_Char(1, i, '#');
            engine.Set_Char(height, i, '#');
        }
        for(int i = 1; i <= height; i++) {
            engine.Set_Char(i, 1, '#');
            engine.Set_Char(i, width, '#');
        }
        
        // Draw title and info
        char title[200];
        sprintf(title, "JULIA SET MORPHER | %s -> %s | c = %.4f + %.4fi | Zoom: %.2f", 
                configs[currentConfig].name, 
                configs[nextConfig].name,
                cReal, cImag, zoom);
        
        for(int i = 0; title[i] && i < width - 2; i++) {
            engine.Set_Char(2, i + 2, title[i]);
        }
        
        // Draw progress bar
        int barWidth = std::min(50, width - 10);
        int filled = (int)(morphProgress * barWidth);
        
        for(int i = 0; i < barWidth; i++) {
            char barChar = (i < filled) ? '=' : '-';
            engine.Set_Char(4, i + (width - barWidth) / 2, barChar);
        }
        
        // Draw controls legend
        char legend[] = "Morphing between 8 Julia configurations | 192x54 | 60 FPS";
        int legendX = (width - strlen(legend)) / 2;
        for(int i = 0; legend[i] && legendX + i < width - 1; i++) {
            engine.Set_Char(height - 2, legendX + i, legend[i]);
        }
    }
};

// Enhanced Julia Morpher with multiple effects
class AdvancedJuliaMorpher {
private:
    AnimByte& engine;
    int width, height;
    double cReal, cImag;
    double time;
    int effectMode; // 0=none, 1=spiral, 2=wave, 3=colorcycle
    double morphTarget[8][2];
    int currentTarget;
    double morphT;
    
    const char gradient[12] = {' ', '.', ',', '-', '~', '+', '=', '*', '#', '&', '%', '@'};
    
public:
    AdvancedJuliaMorpher(AnimByte& eng) : engine(eng), width(engine.GetWidth()), height(engine.GetHeight()) {
        // Predefined Julia targets
        morphTarget[0][0] = -0.74543;  morphTarget[0][1] = 0.11301;   // Classic
        morphTarget[1][0] = 0.0;       morphTarget[1][1] = 1.0;       // Dendrite
        morphTarget[2][0] = -0.75;     morphTarget[2][1] = 0.0;       // Cauliflower
        morphTarget[3][0] = -0.75;     morphTarget[3][1] = 0.1;       // San Marco
        morphTarget[4][0] = -0.391;    morphTarget[4][1] = -0.587;    // Siegel
        morphTarget[5][0] = -0.123;    morphTarget[5][1] = 0.745;     // Rabbit
        morphTarget[6][0] = 0.285;     morphTarget[6][1] = 0.01;      // Spiral
        morphTarget[7][0] = -0.8;      morphTarget[7][1] = 0.156;     // Fire
        
        cReal = morphTarget[0][0];
        cImag = morphTarget[0][1];
        time = 0;
        currentTarget = 0;
        morphT = 0;
        effectMode = 0;
    }
    
    void update() {
        time += 0.016;
        
        // Morph between targets
        morphT += 0.0015;
        if(morphT >= 1.0) {
            morphT = 0;
            currentTarget = (currentTarget + 1) % 8;
            
            // Occasionally change effect mode
            if(rand() % 300 == 0) {
                effectMode = (effectMode + 1) % 4;
            }
        }
        
        // Smooth cubic interpolation
        double t = morphT;
        t = t * t * (3 - 2 * t);
        
        int nextTarget = (currentTarget + 1) % 8;
        cReal = morphTarget[currentTarget][0] * (1 - t) + morphTarget[nextTarget][0] * t;
        cImag = morphTarget[currentTarget][1] * (1 - t) + morphTarget[nextTarget][1] * t;
        
        // Add effect-based parameter modulation
        switch(effectMode) {
            case 1: // Spiral effect
                cReal += sin(time * 0.3) * 0.05;
                cImag += cos(time * 0.5) * 0.05;
                break;
            case 2: // Wave effect
                cReal += sin(time * 0.7) * 0.03;
                cImag += sin(time * 0.9) * 0.03;
                break;
            case 3: // Chaotic effect
                cReal += (sin(time * 1.2) * cos(time * 0.8)) * 0.04;
                cImag += (cos(time * 1.1) * sin(time * 0.6)) * 0.04;
                break;
        }
    }
    
    int calculateJulia(double x, double y, int maxIter) {
        double zx = x;
        double zy = y;
        
        for(int i = 0; i < maxIter; i++) {
            double zx2 = zx * zx;
            double zy2 = zy * zy;
            
            if(zx2 + zy2 > 4.0) {
                // Smooth coloring
                return i + 1 - log(log(sqrt(zx2 + zy2))) / log(2);
            }
            
            double temp = zx2 - zy2 + cReal;
            zy = 2.0 * zx * zy + cImag;
            zx = temp;
        }
        return maxIter;
    }
    
    void draw() {
        engine.Frame_Clean();
        
        int maxIter = 512;
        double zoom = 1.2 + sin(time * 0.05) * 0.1;
        double offsetX = sin(time * 0.03) * 0.2;
        double offsetY = cos(time * 0.04) * 0.15;
        
        double xmin = -2.0 / zoom + offsetX;
        double xmax = 2.0 / zoom + offsetX;
        double ymin = -1.5 / zoom + offsetY;
        double ymax = 1.5 / zoom + offsetY;
        
        double xstep = (xmax - xmin) / width;
        double ystep = (ymax - ymin) / height;
        
        // Optimized rendering - process scanlines
        for(int y = 0; y < height; y++) {
            double cy = ymin + y * ystep;
            
            for(int x = 0; x < width; x++) {
                double cx = xmin + x * xstep;
                
                int iter = calculateJulia(cx, cy, maxIter);
                
                char ch;
                if(iter >= maxIter) {
                    ch = '@';
                } else {
                    // Map to gradient with smooth transitions
                    float normalized = (float)iter / maxIter;
                    int idx = (int)(normalized * 11);
                    idx = std::min(11, std::max(0, idx));
                    
                    // Add time-based variation to gradient
                    if(effectMode == 3) {
                        idx = (idx + (int)(time * 5)) % 12;
                    }
                    
                    ch = gradient[idx];
                }
                
                engine.Set_Char(y + 1, x + 1, ch);
            }
        }
        
        // Draw decorative border
        for(int i = 1; i <= width; i++) {
            if(i % 2 == 0) {
                engine.Set_Char(1, i, '=');
                engine.Set_Char(height, i, '=');
            } else {
                engine.Set_Char(1, i, '-');
                engine.Set_Char(height, i, '-');
            }
        }
        
        // Information display
        char info[256];
        const char* effectNames[] = {"NORMAL", "SPIRAL", "WAVE", "CHAOS"};
        sprintf(info, "JULIA SET MORPHER | Mode: %s | c = %.5f + %.5fi | Morph: %.1f%%", 
                effectNames[effectMode], cReal, cImag, morphT * 100);
        
        int infoLen = strlen(info);
        for(int i = 0; i < infoLen && i < width - 2; i++) {
            engine.Set_Char(3, i + 2, info[i]);
        }
        
        // Dynamic parameter display
        char params[100];
        sprintf(params, "Zoom: %.2f | Offset: (%.2f, %.2f)", zoom, offsetX, offsetY);
        for(int i = 0; params[i] && i < width - 2; i++) {
            engine.Set_Char(4, i + 2, params[i]);
        }
        
        // Progress bar for morphing
        int barWidth = 60;
        int filled = (int)(morphT * barWidth);
        for(int i = 0; i < barWidth; i++) {
            char barChar;
            if(i < filled) barChar = '>';
            else if(i == filled) barChar = '|';
            else barChar = '.';
            engine.Set_Char(height - 3, i + (width - barWidth) / 2, barChar);
        }
        
        // Frame counter and FPS
        static int frameCount = 0;
        static auto lastTime = std::chrono::steady_clock::now();
        frameCount++;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
        
        if(elapsed >= 1000) {
            char fps[50];
            sprintf(fps, "FPS: %d | Resolution: %dx%d", frameCount, width, height);
            for(int i = 0; fps[i] && i < width - 2; i++) {
                engine.Set_Char(height - 1, i + 2, fps[i]);
            }
            frameCount = 0;
            lastTime = now;
        }
    }
};

int main() {
    // Force 192x54 terminal size
    std::cout << "\033[8;54;192t";  // Resize terminal to 192x54
    
    AnimByte engine;
    
    if(engine.Initialise(192, 54) != 0) {
        std::cerr << "Failed to initialise AnimByte engine" << std::endl;
        return 1;
    }
    
    // Hide cursor and clear screen
    std::cout << "\033[2J\033[?25l";
    
    // Splash screen
    std::cout << "\033[H\033[1;36m";
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                         JULIA SET MORPHER - Advanced Fractal Animation Engine                            ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Features:                                                                                               ║\n";
    std::cout << "║  • Real-time Julia set rendering at 192x54 resolution                                                    ║\n";
    std::cout << "║  • Smooth morphing between 8 different Julia configurations                                              ║\n";
    std::cout << "║  • Dynamic zoom and pan with cinematic camera movement                                                   ║\n";
    std::cout << "║  • 4 visual effect modes: Normal, Spiral, Wave, Chaos                                                    ║\n";
    std::cout << "║  • 1-byte ASCII gradient with 12 levels of detail                                                       ║\n";
    std::cout << "║  • 512 iterations per pixel for deep fractal detail                                                     ║\n";
    std::cout << "║  • Smooth coloring algorithm for beautiful gradients                                                    ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n\033[1;33mStarting Julia Morpher in 3 seconds...\033[0m\n";
    sleep(3);
    
    AdvancedJuliaMorpher morpher(engine);
    
    // Target 60 FPS for smooth animation
    const int targetFPS = 60;
    const std::chrono::microseconds frameDuration(1000000 / targetFPS);
    
    int frameCount = 0;
    auto fpsTimer = std::chrono::steady_clock::now();
    
    std::cout << "\033[2J";
    
    // Run animation
    for(int frame = 0; frame < 3600; frame++) { // 60 seconds at 60 FPS
        auto frameStart = std::chrono::steady_clock::now();
        
        morpher.update();
        morpher.draw();
        engine.Render_Frame();
        
        // Precise frame timing
        auto frameEnd = std::chrono::steady_clock::now();
        auto elapsed = frameEnd - frameStart;
        
        if(elapsed < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsed);
        }
        
        // Display FPS in real-time
        frameCount++;
        auto now = std::chrono::steady_clock::now();
        if(now - fpsTimer >= std::chrono::seconds(1)) {
            std::cout << "\033[1;32m\r\033[54;1HFPS: " << frameCount << "     \033[0m";
            std::cout.flush();
            frameCount = 0;
            fpsTimer = now;
        }
    }
    
    // Restore cursor
    std::cout << "\033[?25h";
    std::cout << "\n\n\033[1;32mJulia Morpher Complete! Thank you for watching!\033[0m\n";
    
    return 0;
}