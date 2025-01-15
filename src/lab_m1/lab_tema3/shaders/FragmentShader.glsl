#version 330

in vec2 texcoord;

uniform sampler2D texture_1;     // Heightmap texture
uniform sampler2D color_texture; // Color texture

out vec4 frag_color;

void main()
{
    // Sample the heightmap to get the height value (for reference)
    float height_value = texture(texture_1, texcoord).r;

    // Sample the color texture for surface color
    vec3 color = texture(color_texture, texcoord).rgb;

    // Output the final color
    frag_color = vec4(color * (0.5 + height_value), 1.0);  // Blend based on height
}
