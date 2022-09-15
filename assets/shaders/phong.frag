#version 460 core

#define LIGHT_NUMBER 1

in vec3 geoPosition;
in vec3 worldPosition;
in vec3 normal;
in vec2 UV;

out vec4 color;

struct TransformData{
    mat4 transform;
    vec3 position;
    float pad1;
    vec3 rotation;
    float pad2;
    vec3 scale;
    float pad3;
};
struct LightData{
    TransformData transform;
    
    vec3 lightColor;
    float radius;
    float power;
    int lightType;
};

struct MaterialData{
    vec3 baseColor;
    float maxShine;
    // vec3 normal;
};

layout(std140,binding=1)uniform Materials{
    MaterialData material;
};

layout(std140,binding=2)uniform Lights{
    LightData lights[LIGHT_NUMBER];
};

uniform sampler2D texture1;// samplers are opaque types and
uniform sampler2D texture2;// cannot exist in uniform blocks

vec3 PointLight(vec3 cameraPosition,vec3 position,LightData light,
MaterialData matter){
    // mesh normal
    vec3 n=normal;
    // direction :light to mesh
    vec3 l=normalize(light.transform.position-position);
    // direction :cam to mesh
    vec3 v=normalize(cameraPosition-position);
    // direction :reflection
    vec3 r=normalize(reflect(-l,n));
    float dotLN=dot(l,n);
    float dotRV=dot(r,v);
    vec3 baseColor=material.baseColor;
    
    if(dotLN<0.)
    return vec3(0.,0.,0.);
    
    if(dotRV<0.)
    return light.power*baseColor*dotLN;
    
    return light.power*(baseColor*dotLN+light.lightColor*pow(dotRV,matter.maxShine));
}

vec3 PhongLight(vec3 cameraPosition,vec3 position,
LightData lights[LIGHT_NUMBER],MaterialData material){
    vec3 color3=vec3(0);
    for(int i=0;i<LIGHT_NUMBER;i++){
        LightData light=lights[i];
        vec3 ambientLight=.2*light.lightColor;
        color3+=ambientLight*material.baseColor;
        if(light.lightType==0)
        color3+=PointLight(cameraPosition,position,light,material);
    }
    
    return color3;
}

void main(){
    vec3 cameraPosition=vec3(0,3,4);
    vec3 color3=vec3(0.);
    color3=PhongLight(cameraPosition,worldPosition,lights,material);
    color=vec4(color3,1.);
}