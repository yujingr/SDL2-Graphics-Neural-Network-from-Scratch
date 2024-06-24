#include "Object.h"
#include <cmath>
#include "Texture.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
// Inherits the Object
class Number: public Object{
public:
    // Object Constructor
    Number(glm::vec3 coord, std::string f);
    void init();
    void update(unsigned int screenWidth, unsigned int screenHeight);
    void render();
    std::string filepath;
    float scale =1.0f;
    Texture myTexture;
};

