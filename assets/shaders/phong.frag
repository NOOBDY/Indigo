#version 460 core

in vec3 geo_pos;
in vec3 world_pos;
in vec3 normal;
in vec2 UV;

out vec4 color;
struct TransformData{
    mat4 transform;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};
struct LightData{
    TransformData transform;
    vec3 light_Color;
    float radius;
    float power;
    int light_Type;
};
struct Material{
    vec3 base_color;
    float max_shine;
    // vec3 normal;
};
layout(std140,binding=1)uniform material_Data{
    Material matter;
    
};
#define light_number 1
layout(std140,binding=2)uniform light_Data{
    LightData lights[light_number];
};

uniform sampler2D texture1;// samplers are opaque types and
uniform sampler2D texture2;// cannot exist in uniform blocks

vec3 point_light(vec3 cam_pos,vec3 pos,LightData light,Material matter){
    //mesh normal
    vec3 n=normal;
    //direction :light to mesh
    vec3 l=normalize(light.transform.position-pos);
    //direction :cam to mesh
    vec3 v=normalize(cam_pos-pos);
    //direction :reflection
    vec3 r=normalize(reflect(-l,n));
    float dot_ln=dot(l,n);
    float dot_rv=dot(r,v);
    vec3 base_color=matter.base_color;
    return vec3(dot_rv);
    
    if(dot_ln<0.)
    return vec3(0.,0.,0.);
    
    if(dot_rv<0.)
    return light.power*base_color*dot_ln;
    
    return light.power*(base_color*dot_ln+light.light_Color*pow(dot_rv,matter.max_shine));
}

vec3 phong_light(vec3 cam_pos,vec3 position,LightData lights[light_number],Material matter){
    vec3 color3=vec3(0);
    for(int i=0;i<light_number;i++){
        LightData light=lights[i];
        light.light_Color=vec3(1);
        vec3 amb_light=.3*light.light_Color;
        color3+=amb_light*matter.base_color;
        color3=point_light(cam_pos,position,light,matter);
        // color3=light.transform.position;
    }
    
    return color3;
}

void main(){
    // vec3 light_pos=vec3(0,2,0);
    vec3 cam_pos=vec3(0,3,4);
    vec3 am_c=vec3(.702,.702,.702);
    vec3 spec_c=vec3(1.,1.,1.);
    // float max_shine=100.;
    vec3 color3=vec3(0.);
    // color3=lights[0].light_Color;
    color3=phong_light(cam_pos,world_pos,lights,matter);
    // color3=world_pos;
    color=vec4(color3,1.);
    // color = vec4(normal, 1.0);
}