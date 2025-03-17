#version 430

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;

// Outputs to the fragment shader
out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingLightDi1; // Direction to the second light
out vec3 varyingVertPos;

struct PositionalLight
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform PositionalLight ligh1; // Second light source
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

void main(void)
{
    vec4 worldPos = mv_matrix * vec4(vertPos, 1.0);
    varyingVertPos = worldPos.xyz;
    varyingLightDir = light.position - varyingVertPos;
    varyingLightDi1 = ligh1.position - varyingVertPos; // Calculate the direction to ligh1
    varyingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;

    gl_Position = proj_matrix * worldPos;
}
