#include "Line.h"

// Constructor: set color and points
Line::Line(glm::vec3 color, glm::vec3 coord, glm::vec3 coord2): Object(color, coord), coordinates2(coord2) {
    init();
}

// Initialize
void Line::init() {

    // Set Coordinates for two ends of the line
    GLfloat verts1[] = {
        coordinates.x, coordinates.y, coordinates.z,
        coordinates2.x, coordinates2.y, coordinates2.z,

    };
    // Indices
    GLuint indices1[] = {
        0, 1
    };

    // Create Buffer and Setup Shader
    myBuffer.CreateBufferLayout(3, sizeof(verts1), 2, verts1, indices1);
    std::string vertexShader = myShader.LoadShader("./shaders/vert.glsl");
    std::string fragmentShader = myShader.LoadShader("./shaders/frag.glsl");
    myShader.CreateShader(vertexShader, fragmentShader);

}


void Line::update(unsigned int screenWidth, unsigned int screenHeight) {
    // Load Identity / No transform
    getTransform().loadIdentity();

    // Buffer and Shader Bindings
    myBuffer.Bind();
    myShader.Bind();

    // Projection Model Matrix
    projectionMatrix = glm::perspective(45.0f, (float) screenWidth / (float)screenHeight, 0.1f, 35.0f);
    myShader.setUniformMatrix4fv("modelTransformMatrix", transform.getTransformMatrix());
    myShader.setUniformMatrix4fv("projectionMatrix", &projectionMatrix[0][0]);

    // Set Line Color
    myShader.setUniform3f("rgb", rgb.x, rgb.y, rgb.z);
}

void Line::render() {
    // Bindings
    myBuffer.Bind();
    myShader.Bind();

    //Draw line
    glDrawArrays(GL_LINES, 0, 2);

}

