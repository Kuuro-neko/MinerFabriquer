#version 330 core

uniform sampler2D TextureSampler;
uniform sampler2D NormalsSampler;
uniform sampler2D RoughnessSampler;
uniform sampler2D MetalnessSampler;

uniform sampler2D LightmapSampler;


in vec2 UV;
out vec4 frag_color;
in vec3 vNormal;
in vec3 pos;

uniform int displayNormals;
uniform vec3 camPos;
uniform float time;

float PI = 3.14159265358979323846;
float halfDayDuration = 12.0;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return max(F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0), 0.0);
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

vec3 getNormalFromNormalMap()
{
    vec3 tangentNormal = texture(NormalsSampler, UV).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(pos);
    vec3 Q2  = dFdy(pos);

    vec2 st1 = dFdx(UV);
    vec2 st2 = dFdy(UV);

    vec3 N   = normalize(vNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));

    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float dayNightCycle(float time, float halfDayTime) {
    float cycle = mod(time, 2.0 * halfDayTime) / (2.0 * halfDayTime);
    return 0.5 * (1.0 - cos(2.0 * PI * cycle));
}

float shadeBasedOnFace(vec3 normal, vec3 lightDir) {
    float dotProduct = dot(normal, lightDir);
    return max(dotProduct, 0.0);
}

void main(){
        vec3 N = getNormalFromNormalMap();
        if (displayNormals == 1) {
                frag_color = vec4(N * 0.5 + 0.5, 1.0);   
        } else {
                vec3 lightdir = normalize(vec3(2.0, 1.0, 2.0));
                float lightX = dayNightCycle(time, halfDayDuration);
                vec2 lightUV = vec2(lightX*14.5/16.0, 15.0/16.0);
                vec3 light_color = texture(LightmapSampler, lightUV).xyz;

                float intensity = 5.0;

                vec4 albedoTex = texture(TextureSampler, UV);
                vec3 albedo = pow(albedoTex.rgb, vec3(2.2));
                float roughness = 1.0-texture(RoughnessSampler, UV).r;
                float metalness = texture(MetalnessSampler, UV).r;
                
                vec3 V = normalize(camPos - pos);
                vec3 L = normalize(lightdir);
                vec3 H = normalize(V + L);

                // float distance = length(light_pos - pos);
                // float attenuation = 1.0 / (distance * distance);
                vec3 radiance = light_color * intensity;

                vec3 F0 = vec3(0.04);
                F0 = mix(F0, albedo, metalness);
                
                int steps = 100;
                vec3 Lo = vec3(0.0);

                vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
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

                vec3 ambient = texture(LightmapSampler, lightUV).xyz * albedo;
                vec3 color = ambient + Lo;


                //hdr
                color = color / (color + vec3(1.0));
                //gamma
                color = pow(color, vec3(1.0/2.2));

                float shade = 1-shadeBasedOnFace(N, lightdir);

                color = mix(color, vec3(0.0), shade*0.4);

                frag_color = vec4(color, albedoTex.a);
           
        }
}
