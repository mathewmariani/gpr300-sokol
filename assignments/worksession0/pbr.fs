#version 300 es

precision mediump float;

struct PBRMaterial {
  sampler2D albedo;
  sampler2D metallic;
  sampler2D roughness;
  sampler2D occlusion;
  sampler2D specular;
};
struct Light {
  float brightness;
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
vec3 alb;
float mtl;
float rgh;
float ao;
float spec;

// GGX / Throwbridge-Reitz normal distribution function
// alpha: roughness
// N: normal vector
// H: half vector
float D(float alpha)
{
  float numerator = pow(alpha, 4.0);
  float denominator = PI * pow(pow(NdotH, 2.0) * (pow(alpha, 2.0) - 1.0) + 1.0, 2.0);
  denominator = max(denominator, 0.000001);

  return numerator / denominator;
}

// Schlick-Beckmann geometry shadowing function
// alpha: roughness
// X: NdotV | NdotL
float G1(float alpha, float X)
{
  float k = pow((rgh + 1.0), 2.0) / 8.0;
  float denominator = X * (1.0 - k) + k;
  denominator = max(denominator, 0.000001);

  return X / denominator;
}

// Smith model
// alpha: roughness
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
  return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

// Rendering equation
// N: normal
// V: view vector
// V: light vector
// H: half-way vector
vec3 PBR()
{
  // F0: base reflectivity
  // dieletrics: 0.04
  // metals: alb
  vec3 F0 = vec3(0.04);
  F0 = alb;

  // ratio between reflection and refraction
  vec3 Ks = F(F0);
  vec3 Kd = (vec3(1.0) - Ks) * (1.0 - mtl);

  // diffuse BDRF
  vec3 lambert = alb / PI;

  float alpha = pow(rgh, 2.0);

  // specular BDRF 
  vec3 cookTorranceNumerator = D(alpha) * G(alpha) * Ks;
  float cookTorranceDenominator = 4.0 * VdotN * LdotN;
  cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
  vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

  // diffuse + specular
  vec3 BDRF = (Kd * lambert) + cookTorrance;

  // final color
  return BDRF * lightColor * LdotN;
}

void main()
{
  // pre-sample all textures
  alb = texture(material.albedo, vs_texcoord).rgb;
  mtl = texture(material.metallic, vs_texcoord).r;
  rgh = texture(material.roughness, vs_texcoord).r;
  ao = texture(material.occlusion, vs_texcoord).r;
  spec = texture(material.specular, vs_texcoord).r;
  
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


  // added specular for stylization
  vec3 reflectionDir = reflect(-L, N);
  float specAmount = pow(max(dot(V, reflectionDir), 0.0f), 32.0);
  vec3 specular = spec * specAmount * lightColor;

  vec3 finalColor = (light.color * alb * ao) + PBR() + specular;

  // HDR and gamma correction
  // finalColor = finalColor / (finalColor + vec3(1.0));
  // finalColor = pow(finalColor, vec3(1.0/2.2)); 

  FragColor = vec4(finalColor, 1.0);
}