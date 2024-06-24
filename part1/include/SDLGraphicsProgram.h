#ifndef SDLGRAPHICSPROGRAM
#define SDLGRAPHICSPROGRAM

// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
#if defined(LINUX) || defined(MINGW)
#include <SDL2/SDL.h>
#else // This works for Mac
#include <SDL.h>
#endif

// The glad library helps setup OpenGL extensions.
#include <glad/glad.h>

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include "Object.h"

#include "Line.h"
#include "Sphere.h"
#include "Number.h"

// This class sets up a full graphics program
class SDLGraphicsProgram {
public:

    // Constructor
    SDLGraphicsProgram(int w, int h);
    // Desctructor
    ~SDLGraphicsProgram();
    // Setup OpenGL
    bool initGL();
    // Per frame update
    void update();
    // Renders shapes to the screen
    void render();

    // Update each layer
    void layer1Update();
    void layer2Update();
    void layer3Update();
    void layer4Update();

    // loop that runs forever
    void loop();
    
    // Get Pointer to Window
    SDL_Window* getSDLWindow();

    // Helper Function to QueryOpenGL information.
    void getOpenGLVersionInfo();

private:
    // Screen dimension constants
    int screenHeight;
    int screenWidth;

    // Select specific cube/pixel
    int selectrol = -1;
    int selectcol = -1;

    // Specify Drawing Board size
    int rows = 10;
    int cols = 10;
    // Store the state of drawing Board
    std::array<int, 100> objState;

    // Store Drawing board objects in vector
    std::vector<Object*> allObjects;

    // Store spheres/nodes in Vectors
    std::vector<Sphere*> layer1Spheres;
    std::vector<Sphere*> layer2Spheres;
    std::vector<Sphere*> layer3Spheres;
    std::vector<Sphere*> outputSpheres;

    // Store Number Representation objects in vector
    std::vector<Number*> numbersObjects;

    // Store initial Sphere Sizes
    std::vector<float> b0out;
    std::vector<float> b1out;
    std::vector<float> b2out;
    std::vector<float> b3out;

    // Store results for each layer after calculations
    std::array<float, 10> layer1result;
    std::array<float, 10> layer2result;
    std::array<float, 10> layer3result;
    std::array<float, 10> layer4result;

    // Line Objects between all the layers
    std::vector<Line*> inputTOlayer1;
    std::vector<Line*> layer1TOlayer2;
    std::vector<Line*> layer2TOlayer3;
    std::vector<Line*> layer3TOlayerout;

    // The window we'll be rendering to
    SDL_Window* gWindow ;
    // OpenGL context
    SDL_GLContext gContext;
    // For now, we can have one shader.

};

#endif
