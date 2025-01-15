#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform sampler2D texture_1; 

// Output
out vec2 texcoord;


void main()
{
    // Pass v_texture_coord as output to fragment shader
    texcoord = v_texture_coord;

    vec3 deformed_terrain = v_position;
    const float Y_OFFSET = 35; 
    deformed_terrain[1] += Y_OFFSET * texture(texture_1, v_texture_coord).r;

    gl_Position = Projection * View * Model * vec4(deformed_terrain, 1.0);
}
