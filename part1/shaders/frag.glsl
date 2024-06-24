// ==================================================================
#version 330 core

// Output Color
out vec4 color;

// Texture Coordinates
in vec2 v_texCoord;

// Color
uniform vec3 rgb;

// Determines if using Texture
uniform int texture_usage = 0;

// Samples texture color if using Texture
uniform sampler2D u_Texture;


void main() {

// If not using Texture, then render color from uniform input
  if (texture_usage == 0) {
    color = vec4(rgb, 1.0);
  } else {
    // If using Texture, then samples Texture
    vec4 texColor = texture(u_Texture, v_texCoord);
    color = texColor;
  }


}
// ==================================================================
