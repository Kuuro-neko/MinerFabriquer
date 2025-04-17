#version 330 core

uniform sampler2D TextureSampler;
uniform sampler2D NormalsSampler;
uniform sampler2D RoughnessSampler;
uniform sampler2D MetalnessSampler;

in vec2 UV;
out vec4 frag_color;
in vec3 vNormal;
in vec3 pos;
in vec3 camPos;

uniform int displayNormals;

float PI = 3.14159265358979323846;

vec3 lightOffset = vec3(30.0, 50.0, 30.0);
vec3 light_pos = pos + lightOffset;

// https://learnopengl.com/PBR/Lighting

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return max(F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0), 0.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r*r)/8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 getTangent(vec3 n) {
    // Choose an arbitrary vector that is not parallel to n
    vec3 up = abs(n.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    return normalize(cross(up, n));
}

mat3 computeTBN(vec3 n) {
    vec3 t = getTangent(n);
    vec3 b = normalize(cross(n, t));
    return mat3(t, b, n);
}

vec3 randomHemisphereDirection(vec3 normal, vec2 randomSeed) {
    float phi = 2.0 * PI * randomSeed.x;
    float cosTheta = sqrt(1.0 - randomSeed.y);
    float sinTheta = sqrt(randomSeed.y);

    // Tangent space direction
    vec3 tangentDir = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    // Transform to world space using TBN
    mat3 TBN = computeTBN(normal);
    return normalize(TBN * tangentDir);
}

void main(){
        vec3 N = normalize(vNormal);
        vec3 normalMapValue = texture(NormalsSampler, UV).xyz;
        mat3 TBN = computeTBN(N);
        N = normalize(TBN * (normalMapValue * 2.0 - 1.0));
        
        frag_color = vec4(N * 0.5 + 0.5, 1.0);
        if (displayNormals == 1) {
                frag_color = vec4(N * 0.5 + 0.5, 1.0);
        } else {
                vec3 lightdir = normalize(light_pos - pos);
                vec3 light_color = vec3(1.0, 1.0, 1.0);

                vec4 albedoTex = texture(TextureSampler, UV);
                vec3 albedo = pow(albedoTex.rgb, vec3(2.2));
                if (albedoTex.a < 0.1) {
                    frag_color = vec4(0.0, 0.0, 0.0, 0.0);
                    return;
                }
                float roughness = texture(RoughnessSampler, UV).r;
                float metalness = texture(MetalnessSampler, UV).r;
                
                vec3 V = normalize(camPos - pos);
                vec3 L = normalize(light_pos - pos);
                vec3 H = normalize(V + L);

                float distance = length(light_pos - pos);
                float attenuation = 1.0 / (distance * distance);
                vec3 radiance = light_color * attenuation;

                vec3 F0 = vec3(0.04);
                F0 = mix(F0, albedo, metalness);
                
                int steps = 100;
                vec3 Lo = vec3(0.0);
                float kd = 0.6;

                // vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
                vec3 F = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0, roughness);
                float NDF = DistributionGGX(N, H, roughness);
                float G = GeometrySmith(N, V, L, roughness);

                vec3 numerator = NDF * G * F;
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
                vec3 specular = numerator/denominator; 

                vec3 kS = F;
                vec3 kD = vec3(1.0) - kS;

                kD *= 1.0 - metalness;

                float NdotL = max(dot(N, L), 0.0);

                Lo += (kD * albedo / PI + specular) * radiance * NdotL;

                vec3 ambient = vec3(0.4) * albedo * kD;
                vec3 color = ambient + Lo;


                //hdr
                color = color / (color + vec3(1.0));
                //gamma
                color = pow(color, vec3(1.0/2.2));

                frag_color = vec4(color, 1.0);
           
        }
}
