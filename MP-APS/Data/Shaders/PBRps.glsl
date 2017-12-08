#version 440 core

in FragData {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    noperspective vec3 wireframeDist;
} fragData;

// Directional Light
struct DirectionalLight {
	vec3 color;
	vec3 direction;
};
uniform DirectionalLight directionalLights[3];

// Point Light
struct PointLight {
    vec3 color;
    vec3 position;
};
uniform PointLight pointLights[2];

// material parameters
uniform vec3  albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// IBL
uniform samplerCube irradianceMap;

// Camera
uniform vec3 viewPos;

uniform bool wireframe;

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
vec3 calcBRDF(const vec3 N, const vec3 H, const vec3 L, const vec3 V, const vec3 F0) {
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
    return (kD * albedo / PI + specular) * NdotL;
}

// ----------------------------------------------------------------------------
vec3 pointRadiance(const vec3 lightPosition, const vec3 lightColor, const vec3 V, const vec3 N, const vec3 F0) {
    const vec3 L = normalize(lightPosition - fragData.FragPos);
    const vec3 H = normalize(V + L);

    const float distance = length(lightPosition - fragData.FragPos);
    const float attenuation = 1.0 / (distance * distance); // Inverse-square fall-off
    const vec3 radiance = lightColor * attenuation;

    return calcBRDF(N, H, L, V, F0) * radiance;
}

// ----------------------------------------------------------------------------
vec3 directionalRadiance(const vec3 N, const vec3 V, const vec3 F0, const vec3 color, const vec3 direction) {
    const vec3 L = -direction;
    const vec3 H = normalize(V + L);
    const vec3 radiance = color;

    return calcBRDF(N, H, L, V, F0) * radiance;
}

// ----------------------------------------------------------------------------
void main() {		
    const vec3 N = normalize(fragData.Normal);
    const vec3 V = normalize(viewPos - fragData.FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    // Calculate reflectance for the directional lights
    for (int i = 0; i < 3; ++i) {
        Lo += directionalRadiance(N, V, F0, directionalLights[i].color, directionalLights[i].direction);
    }

    // Calculate reflectance for point lights
    for (int i = 0; i < 2; ++i) {
        Lo += pointRadiance(pointLights[i].position, pointLights[i].color, V, N, F0);
    }

    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;     
    const vec3 irradiance = texture(irradianceMap, N).rgb;
    const vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    // Wireframe
    const vec3 d = fwidth(fragData.wireframeDist);
    const vec3 a3 = smoothstep(vec3(0.0), d * 1.5, fragData.wireframeDist);
    const float edgeFactor = min(min(a3.x, a3.y), a3.z);

    color = wireframe ? mix(vec3(1.0), color.rgb, edgeFactor) : color;

    FragColor = vec4(color, 1.0);
}