// ==================================================================
#version 330 core

// Position Coordinates
layout(location = 0)in vec3 position;
// Take in the texture coordinates
layout(location = 1)in vec2 texCoord;

// Texture Coordinates
out vec2 v_texCoord;

// Uniforms for passing in Projection and Model Matrices
uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;



void main()
{
  // old position
  vec4 oldPosition = vec4(position.x, position.y, position.z, 1.0f);

  // final position as modified by projection and model matrices
  gl_Position  =  projectionMatrix * modelTransformMatrix * oldPosition;

  // Texture
  v_texCoord = texCoord;
}
// ==================================================================
