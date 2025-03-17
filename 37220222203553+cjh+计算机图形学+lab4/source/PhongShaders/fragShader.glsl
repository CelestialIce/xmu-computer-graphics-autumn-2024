#version 430


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


in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingLightDi1;
in vec3 varyingVertPos;

out vec4 fragColor;

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform PositionalLight ligh1; // Second light source
uniform Material material;
uniform mat4 mv_matrix;	 
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

void main(void)
{
    // Normalize the light, normal, and view vectors:
    vec3 L = normalize(varyingLightDir);
    vec3 L1 = normalize(varyingLightDi1); // Normalize direction to second light
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(-varyingVertPos);
    
    // Compute light reflection vectors for both lights:
    vec3 R = normalize(reflect(-L, N));
    vec3 R1 = normalize(reflect(-L1, N));
    
    // Get the angle between the light and surface normal for both lights:
    float cosTheta = max(dot(L, N), 0.0);
    float cosTheta1 = max(dot(L1, N), 0.0);
    
    // Angle between the view vector and reflected light vectors:
    float cosPhi = max(dot(V, R), 0.0);
    float cosPhi1 = max(dot(V, R1), 0.0);

    // Compute ADS contributions (per pixel) for both lights:
    vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient) + (ligh1.ambient * material.ambient)).xyz;
    vec3 diffuse = (light.diffuse.xyz * material.diffuse.xyz * cosTheta) + (ligh1.diffuse.xyz * material.diffuse.xyz * cosTheta1);
    vec3 specular = (light.specular.xyz * material.specular.xyz * pow(cosPhi, material.shininess)) +
                    (ligh1.specular.xyz * material.specular.xyz * pow(cosPhi1, material.shininess));
    
    // Combine contributions:
    fragColor = vec4((ambient + diffuse + specular), 1.0);
}
