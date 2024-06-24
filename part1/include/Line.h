#include "Object.h"

// Inherits the Object
class Line: public Object {
public:
    // Line Constructor: color, point-to and point-from
    Line(glm::vec3 color, glm::vec3 coord, glm::vec3 coord2);
    
    // Second coordinates
    glm::vec3 coordinates2;

    // Functions that are changed
    void init();
    void render();
    void update(unsigned int screenWidth, unsigned int screenHeight);

};

