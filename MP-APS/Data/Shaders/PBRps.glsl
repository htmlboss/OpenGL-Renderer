#version 440 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// material parameters
uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 sunDirection;
uniform vec3 sunColor;

uniform vec3 viewPos;

out vec4 FragColor;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(const vec3 N, const vec3 H, const float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(const float NdotV, const float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(const vec3 N, const vec3 V, const vec3 L, const float roughness) {
    const float NdotV = max(dot(N, V), 0.0);
    const float NdotL = max(dot(N, L), 0.0);
    const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    const float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(const float cosTheta, const vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 directionalReflectance(const vec3 N, const vec3 V, const vec3 F0) {
    // calculate per-light radiance
    const vec3 L = -sunDirection;
    const vec3 H = normalize(V + L);
    const vec3 radiance = sunColor;

    // Cook-Torrance BRDF
    const float NDF = DistributionGGX(N, H, roughness);   
    const float G   = GeometrySmith(N, V, L, roughness);      
    const vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
    const vec3 nominator    = NDF * G * F; 
    const float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    const vec3 specular = nominator / denominator;
        
    // kS is equal to Fresnel
    const vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;     

    // scale light by NdotL
    const float NdotL = max(dot(N, L), 0.0);        

    // add to outgoing radiance Lo
    // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    return (kD * albedo / PI + specular) * radiance * NdotL;
    
}
// ----------------------------------------------------------------------------
void main() {		
    const vec3 N = normalize(Normal);
    const vec3 V = normalize(viewPos - FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // Calculate reflectance for each light
    vec3 Lo = directionalReflectance(N, V, F0);

    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    const vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}