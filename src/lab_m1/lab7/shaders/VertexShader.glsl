#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;


void main()
{
    // TODO(student): Compute world space vectors
    vec3 P = vec3(Model * vec4(v_position, 1));
    vec3 N = normalize(mat3(Model) * normalize(v_normal));
    vec3 V = normalize(eye_position - P);
	vec3 L = normalize(light_position - P);

    // TODO(student): Define ambient light component
    float ambient_light = 0.25;

    // TODO(student): Compute diffuse light component
    float diffuse_light = material_kd * max(dot(N, L), 0);

    // TODO(student): Compute specular light component
    vec3 H = normalize(V + L);
    float specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Gouraud (1971) shading method. There is also the Phong (1975) shading
    // method, which we'll use in the future. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(N, L), 0), material_shininess);
    }

    // TODO(student): Compute light
    float d	= distance(light_position, v_position);
	float f	= 1.0 / (1 + 0.09 * d + 0.032 * d * d);
	float light	= ambient_light + f * (diffuse_light + specular_light);

    // TODO(student): Send color light output to fragment shader
    color = object_color * light;

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
