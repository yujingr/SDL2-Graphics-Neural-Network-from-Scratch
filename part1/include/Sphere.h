#include "Object.h"
#include <cmath>

// Inherits the Object
class Sphere: public Object {
public:
    // Sphere Constructor - Color and Position
    Sphere(glm::vec3 color, glm::vec3 coord);

    // functions to change
    void init();
    void update(unsigned int screenWidth, unsigned int screenHeight);

    // Set Sphere Size scaling
    float scale = 1.0f;
};

