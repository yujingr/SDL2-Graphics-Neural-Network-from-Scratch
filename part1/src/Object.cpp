#include "Object.h"

#define GLCall(x) GLClearErrorStates(); x ; GLCheckError(#x,__LINE__);

// New error handling routine
static void GLClearErrorStates() {
    // Return all of the errors that occur.
    while (glGetError() != GL_NO_ERROR) {
        ;
    }
}

// Returns false if an error occurs
static bool GLCheckError(const char* function, int line) {
    while (GLenum error = glGetError()) {
        // __LINE__ is a special preprocessor macro that
        // Can tell us what line any errors occurred on.
        std::cout << "[OpenGL Error]" << error << "|" << function << " Line: " << line << "\n";
        return false;
    }
    return true;
}

// Initialize Object, takes color and position
Object::Object(glm::vec3 color, glm::vec3 coord): rgb(color), coordinates(coord) {
    init();
}

// alternate Initialization for Number Object, only position needed
Object::Object(glm::vec3 coord): coordinates(coord) {
};

// Destructor
Object::~Object() {
}

// Initialize
void Object::init() {

    // Cube Coordinates
    GLfloat verts1[] = {
        .5f, .5f, .5f,  -.5f, .5f, .5f,  -.5f, -.5f, .5f,  .5f, -.5f, .5f, // front
        .5f, .5f, .5f,   .5f, -.5f, .5f,   .5f, -.5f, -.5f,  .5f, .5f, -.5f, // right
        .5f, .5f, .5f,   .5f, .5f, -.5f,  -.5f, .5f, -.5f, -.5f, .5f, .5f, // top
        -.5f, .5f, .5f,  -.5f, .5f, -.5f,  -.5f, -.5f, -.5f, -.5f, -.5f, .5f, // left
        -.5f, -.5f, -.5f,   .5f, -.5f, -.5f,   .5f, -.5f, .5f, -.5f, -.5f, .5f, // bottom
        .5f, -.5f, -.5f,  -.5f, -.5f, -.5f,  -.5f, .5f, -.5f,  .5f, .5f, -.5f // back
    };
    // pushed into a vector for easy processing
    for (int i = 0; i < sizeof(verts1) / sizeof(verts1[0]); ++i)
    {
        verts.push_back(verts1[i]);
    }


    // Cube Indices
    GLuint indices1[] = {
        0, 1, 2,   2, 3, 0,    // (front)
        4, 5, 6,   6, 7, 4,    // (right)
        8, 9, 10,  10, 11, 8,  // (top)
        12, 13, 14,  14, 15, 12, // (left)
        16, 17, 18,  18, 19, 16, // (bottom)
        20, 21, 22,  22, 23, 20 // (back)
    };

    // pushed into a vector for easy processing
    for (int i = 0; i < sizeof(indices1) / sizeof(indices1[0]); ++i)
    {
        indices.push_back(indices1[i]);
    }

    // Create Buffer and Setup Shaders
    myBuffer.CreateBufferLayout(3, verts.size(), indices.size(), verts.data(), indices.data());
    std::string vertexShader = myShader.LoadShader("./shaders/vert.glsl");
    std::string fragmentShader = myShader.LoadShader("./shaders/frag.glsl");
    myShader.CreateShader(vertexShader, fragmentShader);
}



// update Object function
void Object::update(unsigned int screenWidth, unsigned int screenHeight) {
    // scale and transform
    getTransform().loadIdentity();
    getTransform().translate(coordinates.x, coordinates.y, coordinates.z);
    getTransform().scale(1.0f, 1.0f, 0.05f);

    // Bindings for buffer and shader
    myBuffer.Bind();
    myShader.Bind();

    // Projection Model Matrix
    projectionMatrix = glm::perspective(45.0f, (float) screenWidth / (float)screenHeight, 0.1f, 35.0f);
    myShader.setUniformMatrix4fv("modelTransformMatrix", transform.getTransformMatrix());
    myShader.setUniformMatrix4fv("projectionMatrix", &projectionMatrix[0][0]);

    // Set Color of the Object
    myShader.setUniform3f("rgb", rgb.x, rgb.y, rgb.z);

}

void Object::render() {

    // Bindings for buffer and shader
    myBuffer.Bind();
    myShader.Bind();

    //Render data
    GLCall(glDrawElements(GL_TRIANGLES,
                          indices.size(), // The number of indicies, not triangles.
                          GL_UNSIGNED_INT, // Make sure the data type matches
                          nullptr););        // Offset pointer to the data. nullptr because we are currently bound
}

Transform& Object::getTransform() {
    return transform;
}
