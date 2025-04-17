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

float Fs()

float Fr(vec3 pos, vec3 w0, vec3 wi) {
        vec3 wh = normalize(w0 + wi);
        
        return 0;
}


void main(){
        vec3 normal = normalize(vNormal);
        vec3 normalMapValue = texture(NormalsSampler, UV).xyz;
        normal.x = normal.x * normalMapValue.x;
        normal.y = normal.y * normalMapValue.y;
        normal.z = normal.z * normalMapValue.z;
        
        if (displayNormals == 1) {
                color = vec4(normal * 0.5 + 0.5, 1.0);
        } else {
                vec3 lightpos = pos + vec3(20.0, 60.0, 30.0);
                vec3 lightdir = normalize(lightpos - pos);
                
                vec3 roughness = texture(RoughnessSampler, UV).xyz;
                vec3 metalness = texture(MetalnessSampler, UV).xyz;
                
                vec3 w0 = normalize(camPos - pos);
                vec3 wl = normalize(lightpos - pos);

                int steps = 100;
                float sum = 0.0;
                float dw = 1.0 / float(steps);
                for (int i = 0; i < steps; i++) {
                        vec3 wi = normalize(mix(w0, w1, float(i) * dw));
                        sum += Fr(pos, w0, wi) * L(pos, wi) * dot(normal, wi) * dw;
                }



                color = texture(TextureSampler, UV);
        }
}
