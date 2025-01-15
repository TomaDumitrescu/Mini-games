#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms
uniform vec3 light_direction2;
uniform vec3 light_position2;
uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // TODO(student): Define ambient, diffuse and specular light components
    vec3 V = normalize(eye_position - world_position);
	vec3 L = normalize(light_position - world_position);
    vec3 L2 = normalize(light_position2 - world_position);

    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(world_normal, L), 0);
    float diffuse_light2 = material_kd * max(dot(world_normal, L2), 0);

    vec3 H = normalize(V + L);
    vec3 H2 = normalize(V + L2);
    float specular_light = 0;
    float specular_light2 = 0;
    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(world_normal, L), 0), material_shininess);
    }

    if (diffuse_light2 > 0) {
        specular_light2 = material_ks * pow(max(dot(world_normal, L2), 0), material_shininess);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.
    float cut_off = radians(30.0f);
    float spot_light = dot(-L, light_direction);
    float spot_light2 = dot(-L2, light_direction2);
    float Kq1 = 0, Kq2 = 0;
    if (spot_light > cos(cut_off))
    {
	    float spot_light_limit = cos(cut_off);
        float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
        Kq1 = pow(linear_att, 2);
    }
    if (spot_light2 > cos(cut_off))
    {
	    float spot_light_limit = cos(cut_off);
        float linear_att = (spot_light2 - spot_light_limit) / (1.0f - spot_light_limit);
        Kq2 = pow(linear_att, 2);
    }
    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
	float light1	= ambient_light + Kq1 * (diffuse_light + specular_light);
    float light2	= ambient_light + Kq2 * (diffuse_light2 + specular_light2);

    // TODO(student): Write pixel out color
    out_color = vec4(object_color * (light1 + light2), 1);
}
