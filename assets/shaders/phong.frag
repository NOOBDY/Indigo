#version 460 core

in vec3 geo_pos;
in vec3 world_pos;
in vec3 normal;
in vec2 UV;

out vec4 color;
layout (std140, binding = 1) uniform Data {
    vec3 diff_c;
};

uniform sampler2D texture1; // samplers are opaque types and
uniform sampler2D texture2; // cannot exist in uniform blocks

vec3 calc_light(vec3 diff_c, vec3 spec_c, float alpha, vec3 pos, vec3 eye, vec3 light_pos, vec3 power) {
    vec3 n = normal;
    //direction :light to mesh 
    vec3 l = normalize(light_pos - pos);
    //direction :cam to mesh
    vec3 v = normalize(eye - pos);
    //direction :reflection
    vec3 r = normalize(reflect(-l, n));
    float dot_ln = dot(l, n);
    float dot_rv = dot(r, v);

    if(dot_ln < 0.0)
        return vec3(0.0, 0.0, 0.0);
    
    if(dot_rv < 0.0)
        return power * diff_c * dot_ln;

    return power * ( diff_c * dot_ln + spec_c * pow(dot_rv, alpha));
}

vec3 phong_light(float alpha, vec3 light_pos, vec3 pos, vec3 eye, vec3 am_c, vec3 diff_c, vec3 spec_c){
    vec3 amb_light = .5 * am_c;
    vec3 color3 = amb_light * diff_c;
    vec3 light_power = vec3(1);

    color3 += calc_light(am_c, spec_c, alpha, pos, eye, light_pos, light_power);

    return color3;
}

void main() {
        vec3 light_pos = vec3(3,0,0);
        vec3 cam_pos = vec3(0,3,4);
        vec3 am_c = vec3(0.702, 0.702, 0.702);
        vec3 spec_c = vec3(1.0, 1.0, 1.0);
        float max_shine = 100.0;
        vec3 color3 = phong_light(max_shine, light_pos, world_pos, cam_pos, am_c, diff_c, spec_c);
        color = vec4(color3,1.0);
}