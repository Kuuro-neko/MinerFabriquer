#version 330 core

uniform sampler2D TextureSampler;
uniform sampler2D NormalsSampler;
uniform sampler2D RoughnessSampler;
uniform sampler2D MetalnessSampler;

in vec2 UV;
out vec4 color;
in vec3 vNormal;
in vec3 pos;
in vec3 camPos;

uniform int displayNormals;

float PI = 3.14159265358979323846;

vec3 lightOffset = vec3(30.0, 50.0, 30.0);
vec3 lightpos = pos + lightOffset;

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(vec3 w0, vec3 wh)
{
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, texture(TextureSampler, UV).xyz, texture(MetalnessSampler, UV).x);
    
    return F0 + (1.0 - F0) * pow(1.0 - dot(w0, wh), 5.0);
}

float F_lambert() {
        return 1.0 / PI;
}

vec3 F_cooktorrance(vec3 w0, vec3 wi, vec3 n, float a) {
        vec3 wh = normalize(w0 + wi);
        float D = DistributionGGX(n, wh, a);
        float G = GeometrySmith(n, w0, wi, a);
        vec3 F = FresnelSchlick(w0, wh);
        // return (D * G * F) / (4.0 * max(dot(n, w0), 0.0) * max(dot(n, wi), 0.0));
        //return 0.0;
        return (D * G * F) / (4.0 * (dot(n, w0)) * (dot(n, wi)));
}

vec3 F_r(vec3 pos, vec3 n, vec3 w0, vec3 wi, vec3 albedo, float kd) {
        vec3 ret = vec3(0.0);
        float a = texture(RoughnessSampler, UV).x;
        ret += albedo * F_lambert() * kd;
        ret += F_cooktorrance(w0, wi, n, a);
        return ret;
}

float L(vec3 pos, vec3 wi) {
        
        // float angle = dot(normalize(lightpos - pos), wi);
        // return max(angle, 0.0);
       //return 1.0;
        vec3 lightdir = normalize(lightpos - pos);
        float angle = dot(lightdir, wi);
        return max(angle, 0.0);
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


void main(){
        vec3 normal = normalize(vNormal);
        vec3 normalMapValue = texture(NormalsSampler, UV).xyz;
        mat3 TBN = computeTBN(normal);
        normal = normalize(TBN * (normalMapValue * 2.0 - 1.0));

        color = vec4(normal * 0.5 + 0.5, 1.0);
        if (displayNormals == 1) {
                color = vec4(normal * 0.5 + 0.5, 1.0);
        } else {
                vec3 lightdir = normalize(lightpos - pos);

                vec3 albedo = texture(TextureSampler, UV).xyz;
                vec3 roughness = texture(RoughnessSampler, UV).xyz;
                vec3 metalness = texture(MetalnessSampler, UV).xyz;
                
                vec3 w0 = normalize(camPos - pos);
                vec3 wl = normalize(lightpos - pos);
                
                int steps = 100;
                vec3 l = vec3(0.0); // + emitted light if we add it
                float kd = 0.6;
                

                float dw = 1.0 / float(steps);
                for (int i = 0; i < steps; i++) {
                        vec3 wi = normalize(lightdir + vec3(float(i) * dw, float(i) * dw, float(i) * dw));
                        vec3 f = vec3(1.0); 
                        vec3 Fr = F_r(pos, normal, w0, wi, albedo, kd); //* L(pos, wi) * dot(wi, normal) * dw;
                        f.x = Fr.x;
                        f.y = Fr.y;
                        f.z = Fr.z;
                        f *= L(pos, wi);
                        f *= dot(wi, normal) * dw;
                        l += f;
                }
                vec3 ambient = vec3(0.01);
                l += ambient;
                //color = texture(TextureSampler, UV);
                color = vec4(l, 1.0);
        }
}
