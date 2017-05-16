#version 440 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 camPos;

out vec4 FragColor;

const float PI = 3.14159265359f;

vec3 GetNormal() {
    const vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0f - 1.0f;
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(const vec3 N, const vec3 H, const float roughness) {
    const float a      = roughness*roughness;
    const float a2     = a*a;
    const float NdotH  = max(dot(N, H), 0.0f);
    const float NdotH2 = NdotH*NdotH;
	
    const float numerator = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
	
    return numerator / denom;
}

float GeometrySchlickGGX(const float NdotV, const float roughness) {
    const float r = (roughness + 1.0f);
    const float k = (r*r) / 8.0f;

    const float nom = NdotV;
    const float denom = NdotV * (1.0f - k) + k;
	
    return nom / denom;
}

float GeometrySmith(const vec3 N, const vec3 V, const vec3 L, const float roughness) {
    const float NdotV = max(dot(N, V), 0.0f);
    const float NdotL = max(dot(N, L), 0.0f);
    const float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    const float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlickRoughness(const float cosTheta, const vec3 F0, const float roughness) {
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

void main() {

    const vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, 2.2f);
    const vec3 normal     = GetNormal();
    const float metallic  = texture(metallicMap, TexCoords).r;
    const float roughness = texture(roughnessMap, TexCoords).r;
    const float ao        = texture(aoMap, TexCoords).r;

    const vec3 N = normalize(Normal);
    const vec3 V = normalize(camPos - FragPos);

    vec3 F0 = vec3(0.04f); 
    F0 = mix(F0, albedo, metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0f);

    const vec3 ambient = vec3(0.03f) * albedo * ao;
    const vec3 color = ambient + Lo;

    FragColor = vec4(color, 1.0f);
}