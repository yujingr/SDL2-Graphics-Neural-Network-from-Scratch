#include "Sphere.h"

// Constructor: color and position
Sphere::Sphere(glm::vec3 color, glm::vec3 coord):Object(color,coord){
    init(); 
}

// Set up indices and vertices for the sphere
void Sphere::init(){
    unsigned int latitudeBands = 30;
    unsigned int longitudeBands = 30;
    float radius = 1.0f;
    double PI = 3.14159265359;

        for(unsigned int latNumber = 0; latNumber <= latitudeBands; latNumber++){
            float theta = latNumber * PI / latitudeBands;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for(unsigned int longNumber = 0; longNumber <= longitudeBands; longNumber++){
                float phi = longNumber * 2 * PI / longitudeBands;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;
                // Why is this "1-" Think about the range of texture coordinates
                float u = 1 - ((float)longNumber / (float)longitudeBands);
                float v = 1 - ((float)latNumber / (float)latitudeBands);

                // Setup geometry
                verts.push_back(radius*x);   // Position
                verts.push_back(radius*y);   // Position
                verts.push_back(radius*z);   // Position

            }
        }

        // Now that we have all of our vertices
        // generated, we need to generate our indices for our
        // index element buffer.
        // This diagram shows it nicely visually
        // http://learningwebgl.com/lessons/lesson11/sphere-triangles.png
        for (unsigned int latNumber1 = 0; latNumber1 < latitudeBands; latNumber1++){
            for (unsigned int longNumber1 = 0; longNumber1 < longitudeBands; longNumber1++){
                unsigned int first = (latNumber1 * (longitudeBands + 1)) + longNumber1;
                unsigned int second = first + longitudeBands + 1;
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first+1);

                indices.push_back(second);
                indices.push_back(second+1);
                indices.push_back(first+1);
            }
        }


        // Create Buffer and Set up Shaders
        myBuffer.CreateBufferLayout(3,verts.size(),indices.size(),verts.data(),indices.data());
        std::string vertexShader = myShader.LoadShader("./shaders/vert.glsl");
        std::string fragmentShader = myShader.LoadShader("./shaders/frag.glsl");
        myShader.CreateShader(vertexShader,fragmentShader);
}




void Sphere::update(unsigned int screenWidth, unsigned int screenHeight){
    // Transformations
    getTransform().loadIdentity();
    getTransform().translate(coordinates.x,coordinates.y,coordinates.z);
    getTransform().scale(scale,scale,scale);

    // Bindings
    myBuffer.Bind();
    myShader.Bind();

    // Projection and Model Matrices
    projectionMatrix= glm::perspective(45.0f, (float) screenWidth / (float)screenHeight, 0.1f, 35.0f);
    myShader.setUniformMatrix4fv("modelTransformMatrix", transform.getTransformMatrix());
    myShader.setUniformMatrix4fv("projectionMatrix", &projectionMatrix[0][0]);
    
    // Set Color
    myShader.setUniform3f("rgb", rgb.x, rgb.y, rgb.z);

}





