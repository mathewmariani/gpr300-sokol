#version 300 es

precision mediump float;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Ambient {
    float intensity;
    vec3 color;
};
struct Light {
    vec3 color;
    vec3 position;
};

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform sampler2D texture0;
uniform Material material;
uniform Ambient ambient;
uniform Light light;
uniform vec3 camera_position;

vec3 blinnPhong(float NdotL, float NdotH)
{
    vec3 diffuse = NdotL * material.diffuse;
    vec3 specular = pow(NdotH, material.shininess * 128.0) * material.specular;
    return (diffuse + specular);
}

void main()
{
    // normalize inputs
    vec3 normal = normalize(vs_normal);
    vec3 view_dir = normalize(camera_position - vs_position);
    vec3 light_dir = normalize(light.position - vs_position);
    vec3 halfway_dir = normalize(light_dir + view_dir);

    // dot products
    float NdotL = max(dot(normal, light_dir), 0.0);
    float NdotH = max(dot(normal, halfway_dir), 0.0);

    // calculate blinnPhong reflectance model
    vec3 lighting = blinnPhong(NdotL, NdotH);
    lighting *= light.color;
    lighting += light.color * material.ambient;

    vec3 object_color = (normal * 0.5 + 0.5);

    FragColor = vec4(object_color * lighting, 1.0);
}