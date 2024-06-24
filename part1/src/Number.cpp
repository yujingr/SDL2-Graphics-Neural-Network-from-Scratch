#include "Number.h"

// Initialize with Position and texture filename
Number::Number(glm::vec3 coord, std::string f): Object(coord), filepath(f) {
    init();
}

// Setup Number Object
void Number::init() {
    // Square from two triangles
    GLfloat verts1[] = {
        -1.0f, -1.0f, 0.0f, // Position
        0.0f, 0.0f,         // texture

        1.0f, -1.0f, 0.0f,  // Position
        1.0f, 0.0f,

        1.0f, 1.0f, 0.0f,   // Position
        1.0f, 1.0f,

        -1.0f, 1.0f, 0.0f,   // Position
        0.0f, 1.0f
    };

    GLuint indices1[] = {0, 1, 2, 2, 3, 0};


    // Create a texture buffer
    myBuffer.CreateBufferTextureLayout(5, sizeof(verts1), 6, verts1, indices1);

    // Load Texture from File
    myTexture.LoadTexture(filepath);

    // Setup Shader
    std::string vertexShader = myShader.LoadShader("./shaders/vert.glsl");
    std::string fragmentShader = myShader.LoadShader("./shaders/frag.glsl");
    myShader.CreateShader(vertexShader, fragmentShader);
}



// Number Object Update
void Number::update(unsigned int screenWidth, unsigned int screenHeight) {
    // Transformations
    getTransform().loadIdentity();
    getTransform().translate(coordinates.x, coordinates.y, coordinates.z);
    getTransform().scale(scale, scale, scale);

    // Bindings
    myTexture.Unbind();
    myBuffer.Bind();
    myShader.Bind();
    myTexture.Bind();

    // Set to use Texture in shader
    myShader.setUniformMatrix1i("texture_usage", 1);
    myShader.setUniformMatrix1i("u_Texture", 0);

    // Projection Model Matrix
    projectionMatrix = glm::perspective(45.0f, (float) screenWidth / (float)screenHeight, 0.1f, 35.0f);
    myShader.setUniformMatrix4fv("modelTransformMatrix", transform.getTransformMatrix());
    myShader.setUniformMatrix4fv("projectionMatrix", &projectionMatrix[0][0]);

}


void Number::render() {
    // Bindings
    myBuffer.Bind();
    myTexture.Bind();
    myShader.Bind();

    //Render data
    glDrawElements(GL_TRIANGLES,
                   6,               // The number of indicies, not triangles.
                   GL_UNSIGNED_INT, // Make sure the data type matches
                   nullptr);       // Offset pointer to the data. nullptr because we are currently bound:

}




