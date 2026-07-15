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
    
    const char* grayChars = "@%#*+=-:.`'^\"~_,; ";
    const int GRAY_LEVELS = 16;
    
    // 3D vertices
    const int MAX_VERTICES = 600;
    float vertices[MAX_VERTICES][3];
    float transformed[MAX_VERTICES][3];
    float projected[MAX_VERTICES][2];
    float depthBuffer[MAX_VERTICES];
    int vertexCount = 0;
    
    // Edges
    const int MAX_EDGES = 1000;
    int edges[MAX_EDGES][2];
    int edgeCount = 0;
    
    // Generate torus knot
    float R = 10.0f;
    float r = 3.5f;
    int uSteps = 24;
    int vSteps = 12;
    
    for (int i = 0; i < uSteps; i++) {
        float u = i * 2.0f * PI / uSteps;
        for (int j = 0; j < vSteps; j++) {
            float v = j * 2.0f * PI / vSteps;
            
            float knotR = R + r * cos(2.0f * u);
            float x = knotR * cos(3.0f * u);
            float y = knotR * sin(3.0f * u);
            float z = r * sin(2.0f * u) + 2.0f * sin(v);
            
            vertices[vertexCount][0] = x;
            vertices[vertexCount][1] = y;
            vertices[vertexCount][2] = z;
            vertexCount++;
        }
    }
    
    // Generate edges
    for (int i = 0; i < uSteps; i++) {
        for (int j = 0; j < vSteps; j++) {
            int current = i * vSteps + j;
            int nextU = ((i + 1) % uSteps) * vSteps + j;
            int nextV = i * vSteps + ((j + 1) % vSteps);
            
            edges[edgeCount][0] = current;
            edges[edgeCount][1] = nextU;
            edgeCount++;
            
            edges[edgeCount][0] = current;
            edges[edgeCount][1] = nextV;
            edgeCount++;
        }
    }
    
    // Add stellated core
    float phi = (1.0f + sqrt(5.0f)) / 2.0f;
    float coreVerts[12][3] = {
        {0, 1, phi}, {0, -1, phi}, {0, 1, -phi}, {0, -1, -phi},
        {1, phi, 0}, {-1, phi, 0}, {1, -phi, 0}, {-1, -phi, 0},
        {phi, 0, 1}, {phi, 0, -1}, {-phi, 0, 1}, {-phi, 0, -1}
    };
    
    int coreOffset = vertexCount;
    for (int i = 0; i < 12; i++) {
        vertices[vertexCount][0] = coreVerts[i][0] * 5.0f;
        vertices[vertexCount][1] = coreVerts[i][1] * 5.0f;
        vertices[vertexCount][2] = coreVerts[i][2] * 5.0f;
        vertexCount++;
    }
    
    // Core edges
    int coreEdgeList[30][2] = {
        {0,1}, {0,4}, {0,5}, {0,8}, {0,10},
        {1,6}, {1,7}, {1,8}, {1,10},
        {2,3}, {2,4}, {2,5}, {2,9}, {2,11},
        {3,6}, {3,7}, {3,9}, {3,11},
        {4,5}, {4,8}, {4,9}, {5,10}, {5,11},
        {6,7}, {6,8}, {6,9}, {7,10}, {7,11},
        {8,9}, {10,11}
    };
    
    for (int i = 0; i < 30; i++) {
        edges[edgeCount][0] = coreOffset + coreEdgeList[i][0];
        edges[edgeCount][1] = coreOffset + coreEdgeList[i][1];
        edgeCount++;
    }
    
    // Camera settings
    float camDistance = 30.0f;
    float fov = 80.0f;
    float aspectRatio = (float)WIDTH / HEIGHT;
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
    
    // Depth buffer for rendering
    float screenDepth[HEIGHT][WIDTH];
    
    auto lastFrameTime = std::chrono::steady_clock::now();
    float globalTime = 0;
    int frameCount = 0;
    
    std::cout << "\033[2J\033[H";
    std::cout << "COMPLEX 3D OBJECT - Torus Knot + Stellated Core | 210x54 | 100 FPS\n";
    usleep(2000000);
    
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;
        
        if (deltaTime < 0.01f) {
            usleep((0.01f - deltaTime) * 1000000);
            deltaTime = 0.01f;
        }
        
        globalTime += deltaTime;
        frameCount++;
        
        // Clear buffer and depth buffer
        for (int i = 1; i <= HEIGHT; i++) {
            for (int j = 1; j <= WIDTH; j++) {
                anim.Set_Char(i, j, ' ');
                screenDepth[i-1][j-1] = 999999.0f;
            }
        }
        
        // Update rotation
        rotX += deltaTime * 0.4f;
        rotY += deltaTime * 0.6f;
        rotZ += deltaTime * 0.2f;
        
        float cosX = cos(rotX), sinX = sin(rotX);
        float cosY = cos(rotY), sinY = sin(rotY);
        float cosZ = cos(rotZ), sinZ = sin(rotZ);
        
        // Transform vertices
        for (int i = 0; i < vertexCount; i++) {
            float x = vertices[i][0];
            float y = vertices[i][1];
            float z = vertices[i][2];
            
            // Rotate X
            float y1 = y * cosX - z * sinX;
            float z1 = y * sinX + z * cosX;
            
            // Rotate Y
            float x2 = x * cosY + z1 * sinY;
            float z2 = -x * sinY + z1 * cosY;
            
            // Rotate Z
            float x3 = x2 * cosZ - y1 * sinZ;
            float y3 = x2 * sinZ + y1 * cosZ;
            float z3 = z2;
            
            transformed[i][0] = x3;
            transformed[i][1] = y3;
            transformed[i][2] = z3;
            
            float zWithCam = z3 + camDistance;
            depthBuffer[i] = zWithCam;
            
            projected[i][0] = (x3 * fov / zWithCam * aspectRatio) + WIDTH / 2.0f;
            projected[i][1] = (-y3 * fov / zWithCam) + HEIGHT / 2.0f;
        }
        
        // Draw filled faces using scanline
        for (int i = 0; i < uSteps - 1; i++) {
            for (int j = 0; j < vSteps - 1; j++) {
                int v0 = i * vSteps + j;
                int v1 = (i + 1) * vSteps + j;
                int v2 = i * vSteps + j + 1;
                int v3 = (i + 1) * vSteps + j + 1;
                
                // Two triangles per quad
                int faces[2][3] = {{v0, v1, v2}, {v1, v3, v2}};
                
                for (int f = 0; f < 2; f++) {
                    int f0 = faces[f][0];
                    int f1 = faces[f][1];
                    int f2 = faces[f][2];
                    
                    // Back-face culling
                    float x1 = transformed[f1][0] - transformed[f0][0];
                    float y1 = transformed[f1][1] - transformed[f0][1];
                    float x2 = transformed[f2][0] - transformed[f0][0];
                    float y2 = transformed[f2][1] - transformed[f0][1];
                    float cross = x1 * y2 - y1 * x2;
                    
                    if (cross < 0) continue;
                    
                    // Get projected points
                    int px0 = (int)projected[f0][0];
                    int py0 = (int)projected[f0][1];
                    int px1 = (int)projected[f1][0];
                    int py1 = (int)projected[f1][1];
                    int px2 = (int)projected[f2][0];
                    int py2 = (int)projected[f2][1];
                    
                    // Bounding box
                    int minX = std::min(std::min(px0, px1), px2);
                    int maxX = std::max(std::max(px0, px1), px2);
                    int minY = std::min(std::min(py0, py1), py2);
                    int maxY = std::max(std::max(py0, py1), py2);
                    
                    if (minX < 0) minX = 0;
                    if (maxX >= WIDTH) maxX = WIDTH - 1;
                    if (minY < 0) minY = 0;
                    if (maxY >= HEIGHT) maxY = HEIGHT - 1;
                    
                    float avgDepth = (depthBuffer[f0] + depthBuffer[f1] + depthBuffer[f2]) / 3.0f;
                    
                    // Light calculation
                    float nx = y1 * (transformed[f2][2] - transformed[f0][2]) - (transformed[f1][2] - transformed[f0][2]) * y2;
                    float ny = (transformed[f1][2] - transformed[f0][2]) * x2 - x1 * (transformed[f2][2] - transformed[f0][2]);
                    float nz = x1 * y2 - y1 * x2;
                    float light = (nx * 0.5f + ny * 0.3f + nz * 0.8f) / sqrt(nx*nx + ny*ny + nz*nz);
                    if (light < 0.2f) light = 0.2f;
                    if (light > 1.0f) light = 1.0f;
                    
                    int charIdx = 4 + (int)(light * 10);
                    if (charIdx >= GRAY_LEVELS) charIdx = GRAY_LEVELS - 1;
                    
                    // Scanline fill
                    for (int y = minY; y <= maxY; y++) {
                        for (int x = minX; x <= maxX; x++) {
                            // Barycentric test
                            float denom = (py1 - py2) * (px0 - px2) + (px2 - px1) * (py0 - py2);
                            if (fabs(denom) < 0.001f) continue;
                            
                            float w0 = ((py1 - py2) * (x - px2) + (px2 - px1) * (y - py2)) / denom;
                            float w1 = ((py2 - py0) * (x - px2) + (px0 - px2) * (y - py2)) / denom;
                            float w2 = 1.0f - w0 - w1;
                            
                            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                                float depth = w0 * depthBuffer[f0] + w1 * depthBuffer[f1] + w2 * depthBuffer[f2];
                                if (depth < screenDepth[y][x]) {
                                    screenDepth[y][x] = depth;
                                    anim.Set_Char(y + 1, x + 1, grayChars[charIdx]);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Draw wireframe edges
        for (int e = 0; e < edgeCount; e++) {
            int v0 = edges[e][0];
            int v1 = edges[e][1];
            
            int x0 = (int)projected[v0][0];
            int y0 = (int)projected[v0][1];
            int x1 = (int)projected[v1][0];
            int y1 = (int)projected[v1][1];
            
            // Bresenham line
            int dx = abs(x1 - x0);
            int dy = abs(y1 - y0);
            int sx = x0 < x1 ? 1 : -1;
            int sy = y0 < y1 ? 1 : -1;
            int err = dx - dy;
            
            float avgDepth = (depthBuffer[v0] + depthBuffer[v1]) / 2.0f;
            float depthFactor = 1.0f - (avgDepth - camDistance) / 40.0f;
            if (depthFactor < 0.3f) depthFactor = 0.3f;
            if (depthFactor > 1.0f) depthFactor = 1.0f;
            
            int edgeIdx = 1 + (int)(depthFactor * 4);
            if (edgeIdx >= GRAY_LEVELS) edgeIdx = GRAY_LEVELS - 1;
            
            while (true) {
                if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT) {
                    if (avgDepth <= screenDepth[y0][x0] + 2.0f) {
                        anim.Set_Char(y0 + 1, x0 + 1, grayChars[edgeIdx]);
                    }
                }
                if (x0 == x1 && y0 == y1) break;
                int e2 = 2 * err;
                if (e2 > -dy) { err -= dy; x0 += sx; }
                if (e2 < dx) { err += dx; y0 += sy; }
            }
        }
        
        // Draw vertices as stars
        for (int v = 0; v < vertexCount; v++) {
            int x = (int)projected[v][0];
            int y = (int)projected[v][1];
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                anim.Set_Char(y + 1, x + 1, '@');
            }
        }
        
        // HUD
        char hud[100];
        snprintf(hud, sizeof(hud), "3D OBJECT | Verts:%d Edges:%d | Rot:(%.0f,%.0f,%.0f) | FPS:100", 
                 vertexCount, edgeCount, rotX*180/PI, rotY*180/PI, rotZ*180/PI);
        for (int i = 0; hud[i]; i++) anim.Set_Char(1, i + 2, hud[i]);
        
        const char* title = "COMPLEX 3D - Torus Knot + Stellated Core - Real-time Rotation - Depth Shading";
        for (int i = 0; title[i]; i++) anim.Set_Char(HEIGHT, i + 2, title[i]);
        
        anim.Render_Frame();
        
        auto frameEnd = std::chrono::steady_clock::now();
        float frameTime = std::chrono::duration<float>(frameEnd - currentTime).count();
        if (frameTime < 0.01f) usleep((0.01f - frameTime) * 1000000);
    }
    
    return 0;
}