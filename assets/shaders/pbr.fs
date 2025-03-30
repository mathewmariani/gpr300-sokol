#version 300 es

precision mediump float;

struct PBRMaterial {
  sampler2D albedo;
  sampler2D metallic;
  sampler2D roughness;
  sampler2D occlusion;
  sampler2D specular;
  sampler2D combined;
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
uniform PBRMaterial material;
uniform Light light;
uniform vec3 camera_position;

bool use_nintendo = true;
uniform bool is_metal;
uniform float base_reflectivity;
uniform bool use_custom;
uniform vec3 custom_alb;
uniform float custom_mtl;
uniform float custom_rgh;

// constants
const vec3 lightColor = vec3(1.0);
const float PI = 3.14159265359;

// cached variables
float NdotH;
float NdotV;
float NdotL;
float VdotH;
float VdotN;
float LdotN;
vec3 mat_alb;
vec3 mat_pbr;
float mat_mtl;
float mat_rgh;
float mat_ao;
float mat_spec;

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

vec3 BDRF()
{
  vec3 _alb = vec3(0.0);
  float _rgh = 0.0;
  float _mtl = 0.0;
  if (use_custom)
  {
    _alb = custom_alb;
    _rgh = custom_rgh;
    _mtl = custom_mtl;
  }
  else
  {
    _alb = mat_alb;
    _rgh = mat_rgh;
    _mtl = mat_mtl;
  }

  // if it's a metal, use the albedo color as F0 (metallic workflow).
  // if dia-electric (like plastic) use F0 of `base_reflectivity`.
  vec3 F0 = vec3(base_reflectivity);
  if (is_metal)
  {
    F0 = _alb;
  }
  else
  {
    F0 = mix(F0, _alb, _mtl);
  }

  vec3 fresnel = F(F0);
  vec3 cook = cookTorrence(fresnel, _rgh);

  // ratio between reflection and refraction
  // Ks + Kd = 1
  vec3 Ks = fresnel;
  vec3 Kd = (1.0 - Ks) * (1.0 - _mtl);

  // lambertian distribution
  vec3 lambert = _alb / PI;

  // diffuse + specular
  vec3 diffuse = (Kd * lambert);
  vec3 specular = (Ks * cook);
  return diffuse + specular;
}

// Rendering equation
// N: normal
// V: view vector
// L: light vector
// H: halfway vector
void main()
{
  // pre-sample all textures
  mat_alb = pow(texture(material.albedo, vs_texcoord).rgb, vec3(2.2));
  mat_pbr = texture(material.combined, vs_texcoord).rgb;
  mat_mtl = texture(material.metallic, vs_texcoord).r;
  mat_rgh = texture(material.roughness, vs_texcoord).r;
  mat_ao = texture(material.occlusion, vs_texcoord).r;
  mat_spec = texture(material.specular, vs_texcoord).r;
  
  vec3 N = normalize(vs_normal);
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

  vec3 ambient = vec3(0.03) * mat_alb * mat_ao;
  if (use_custom)
  {
    ambient = vec3(0.03) * custom_alb * mat_ao;
  }
  vec3 emitted = vec3(0.0);
  vec3 brdf = BDRF();
  vec3 incoming = vec3(1.0) * mat_ao;
  vec3 pbr = emitted + (brdf * incoming * LdotN);
  vec3 color = pbr + ambient;

  // HDR & Gamma correction
  color = color / (color + vec3(1.0));
  FragColor = vec4(pow(color, vec3(1.0/2.2)), 1.0);
  // FragColor = vec4(mat_pbr, 1.0);
  // FragColor = texture(material.roughness, vs_texcoord);
}