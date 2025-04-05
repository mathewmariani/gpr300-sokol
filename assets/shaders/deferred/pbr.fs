#version 300 es

precision mediump float;
precision mediump int;

struct Light {
    vec3 color;
    vec3 position;
    float radius;
};

struct Material
{
    float roughness;
    float metallic;
    float reflectivity;
};

// attachments
layout(location = 0) out vec4 frag_lighting;

uniform vec2 textureSize; 
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;
uniform vec3 camera_position;
uniform Light light;
uniform Material material;

// constants
const float PI = 3.14159265359;

bool is_metal = false;

// cached variables
float NdotH;
float NdotV;
float NdotL;
float VdotH;
float VdotN;
float LdotN;

float calculateAttentuation(float dist, float radius)
{
	float i = clamp(1.0 - pow(dist/radius,4.0), 0.0, 1.0);
	return i * i;
}

// GGX / Throwbridge-Reitz normal distribution function
// N: normal vector
// H: half vector
float D(float alpha)
{
    float alpha2 = pow(alpha, 4.0);
    float denominator = pow(pow(NdotH, 2.0) * (alpha2 - 1.0) + 1.0, 2.0);
    return alpha2 / (PI * denominator);
}

// Schlick-Beckmann geometry shadowing function
// X: NdotV | NdotL
float G1(float alpha, float X)
{
    float k = pow((alpha + 1.0), 2.0) / 8.0;
    float denominator = X * (1.0 - k) + k;
    return X / denominator;
}

// Smith model
// N: normal vector
// V: view vector
// L: light vector
float G(float alpha)
{
    return G1(alpha, NdotV) * G1(alpha, NdotL);
}

// Fresnel-Schlick function
// F0: base reflectivity
// V: view vector
// H: half-way vector
vec3 F(vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

// V: view vector
// H: half-way vector
// L: light vector
// N: normal
// F0: base reflectivity
// alpha: roughness
vec3 cookTorrence(vec3 fresnel, float alpha)
{
    // D: GGX / Throwbridge-Reitz Normal Distribution Function
    // G: Schlick-Beckmann Geometry Shadowing Function
    // F: Fresnel-Schlick Function
    vec3 cookTorranceNumerator = D(alpha) * G(alpha) * fresnel;
    float cookTorranceDenominator = max(4.0 * VdotN * LdotN, 0.0001);
    return cookTorranceNumerator / cookTorranceDenominator;
}

vec3 BDRF(vec3 position, vec3 normal, vec3 albedo)
{
    // if it's a metal, use the albedo color as F0 (metallic workflow).
    // if dia-electric (like plastic) use F0 of `material.reflectivity`.
    vec3 F0 = vec3(material.reflectivity);
    if (is_metal)
    {
        F0 = albedo;
    }
    else
    {
        F0 = mix(F0, albedo, material.metallic);
    }

    vec3 fresnel = F(F0);
    vec3 cook = cookTorrence(fresnel, material.roughness);

    // ratio between reflection and refraction
    // Ks + Kd = 1
    vec3 Ks = fresnel;
    vec3 Kd = (1.0 - Ks) * (1.0 - material.metallic);

    // lambertian distribution
    vec3 lambert = albedo / PI;

    // diffuse + specular
    vec3 diffuse = (Kd * lambert);
    vec3 specular = (Ks * cook);
    return diffuse + specular;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / textureSize;

    vec3 position = texture(g_position, uv).xyz;
    vec3 normal = texture(g_normal, uv).xyz;
    vec3 albedo = texture(g_albedo, uv).xyz;

    vec3 N = normalize(normal);
    vec3 V = normalize(camera_position);
    vec3 L = normalize(light.position);
    vec3 H = normalize(V + L);

    // pre-compute all dot products
    NdotH = max(dot(N, H), 0.0);
    NdotV = max(dot(N, V), 0.0);
    NdotL = max(dot(N, L), 0.0);
    VdotH = max(dot(V, H), 0.0);
    VdotN = max(dot(V, N), 0.0);
    LdotN = max(dot(L, N), 0.0);

    // rendering equation
    float attentuation = calculateAttentuation(length(light.position - position), light.radius);
    vec3 emitted = vec3(0.0);
    vec3 brdf = BDRF(position, normal, albedo);
    vec3 incoming = vec3(1.0) * attentuation;
    vec3 pbr = emitted + (brdf * incoming * LdotN);
    vec3 color = pbr;

    // HDR & Gamma correction
    color = color / (color + vec3(1.0));
    frag_lighting = vec4(pow(color, vec3(1.0/2.2)), 1.0);
}