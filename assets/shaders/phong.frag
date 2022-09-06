#version 460 core

in vec3 normal;
in vec3 world_pos;
in vec3 geo_pos;
in vec2 UV;

out vec4 color;

layout (std140, binding = 1) uniform Data {
    vec3 diffuseColor;
};

uniform sampler2D texture1; // samplers are opaque types and
uniform sampler2D texture2; // cannot exist in uniform blocks
vec3 culc_light(vec3 diff_c,vec3 ,float al, vec3 pos,vec3 eye,vec3 light_pos,vec3 power){
	vec3 n=normal;
    //light direction
    vec3 l=normalize(light_pos-pos);

    vec3 v=normalize(eye-pos);
    vec3 r=normalize(reflect(-l,n));
	float dot_ln=dot(l,n);
    float dot_rv=dot(r,v);
    if(dot_ln<0.)return vec3(0.0, 0.0, 0.0);
    
    if(dot_rv<0.)return power*diff_c*dot_ln;
    return power*( diff_c*dot_ln + spec_c*pow(dot_rv,al));
}
vec3 phong_light(float al,vec3 light_pos,vec3 pos,vec3 eye,vec3 am_c,vec3 diff_c,vec3 spec_c){
    vec3 amb_light=.1*am_c;
    vec3 color3=amb_light;
    vec3 light_power=vec3(.4);
    color3+=culc_light(am_c,spec_c,al,pos,eye,light_pos,light_power);
    return color3;
}
void main() {
        vec3 light_pos =vec3(0,3,3);
        vec3 cam_pos=vec3(0,3,4);
        //vec3 light_pos=vec3(2.950,3.689,4.000);
        vec3 am_c=vec3(0.1725, 0.1725, 0.1725);
        vec3 diff_c=vec3(0.1961, 0.2863, 0.9765);
        vec3 spec_c=vec3(1.0, 1.0, 1.0);
        float max_shine=1.0;
		vec3 color3=phong_light(max_shine,light_pos,world_pos,cam_pos,am_c,diff_c,spec_c);
        color=vec4(color3,1.0);
        // color=vec4(world_pos,1.0);
        // color=vec4(abs(normal),1.0);
        // color = texture(texture1, UV);
}