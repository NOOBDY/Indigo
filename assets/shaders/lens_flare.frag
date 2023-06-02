#version 450 core

#define LIGHT_NUMBER 1
#define NONE 0
#define POINT 1
#define SPOT 2
#define DIRECTION 3
#define AMBIENT 4

struct PipelineData {
    int ID;
    float time;
    float detiaTime;
    int selectPass;

    int useSSAO;
    int useOutline;
    int useHDRI;
    int useVolume;

    vec3 volumeColor;
    float density;
};

struct TransformData {
    mat4 transform;

    vec3 position;
    float pad1;

    vec3 rotation;
    float pad2;

    vec3 scale;
    float pad3;

    vec3 direction;
    float pad4;
};

struct LightData {
    TransformData transform;

    vec3 lightColor;
    float radius;

    float power;
    int lightType;
    float innerCone;
    float outerCone;

    mat4 lightProjections[6];

    float nearPlane;
    float farPlane;
    int castShadow;
    int useColorTexture;
};

struct CameraData {
    TransformData transform;
    mat4 projection;
    mat4 view;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float FOV;
};

layout(std140, binding = 0) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

layout(std140, binding = 2) uniform Lights {
    LightData lights[LIGHT_NUMBER];
};
layout(std140, binding = 3) uniform Camera {
    CameraData cameraInfo;
};
layout(std140, binding = 4) uniform PipelineUniform {
    PipelineData pipelineInfo;
};

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outScreenLensFlare;

uniform sampler2D screenEmission;
uniform sampler2D screenLensFlare;
uniform sampler2D noiseTexture;

#define PI 3.1415926

vec3 depth2position(highp float depth, mat4 projection, mat4 view) {
    mat4 invert_view_projection = inverse(projection * view);

    float ViewZ;
    ViewZ = depth * 2.0 - 1.0;

    // way1
    vec4 sPos = vec4(UV * 2.0 - 1.0, ViewZ, 1.0);
    vec4 worldPosition = invert_view_projection * sPos;
    worldPosition = vec4((worldPosition.xyz / worldPosition.w), 1.0f);
    return worldPosition.xyz;
}
float fade(vec3 center, vec3 position, float radius) {
    return (1 - clamp(length(position - center) / radius, 0, 1));
    // return 1.0 / (length(position - center) / radius + 0.1);
    // return 1.0 - clamp(length(position - center) / radius, 0, 1);
}
vec2 panoramaUV(vec3 nuv) {
    vec2 uv = vec2(0.0);
    uv.x = 0.5 + atan(nuv.x, nuv.z) / (2 * PI);
    uv.y = 0.5 + asin(nuv.y) / (PI);
    return uv;
}
// vec4 lensFlare(vec3 cameraPosition, LightData light) {
//     vec3 position = deferredInfo.position;
//     // mesh normal
//     // vec3 N = deferredInfo.normal;
//     // n = TBN * (texture(texture3, UV).xyz * 2 - 1);
//     vec3 direction = normalize(light.transform.direction);
//     // direction :light to mesh
//     vec3 L = light.lightType == DIRECTION
//                  ? direction
//                  : normalize(light.transform.position - position);
//     // direction :cam to mesh
//     vec3 V = normalize(cameraPosition - position);
//     // direction :reflection
//     // vec3 R = normalize(reflect(-V, N));
//     // for Blinn-Phong lighting
//     vec3 H = normalize(V + L);

//     vec3 lightColor = light.lightColor;
//     float fadeOut = light.lightType != DIRECTION
//                         ? fade(light.transform.position, position, light.radius)
//                         : 1.0;
//     // spotlight
//     float theta = dot(L, normalize(direction));
//     float epsilon =
//         cos(radians(light.innerCone)) - cos(radians(light.outerCone));
//     float intensity =
//         clamp((theta - cos(radians(light.outerCone))) / epsilon, 0.0, 1.0);
//     float spot = light.lightType == SPOT ? intensity : 1.0;

//     // diffuse lighting
//     // float dotNL = max(dot(N, L), 0.0);
//     // float dotNV = max(dot(N, V), 0.0);
//     // float dotNH = max(dot(N, H), 0.0);
//     float dotHV = max(dot(H, V), 0.0);
//     vec3 Lo=vec3(0);
//     return vec4(Lo, 1);
// }

ivec2 noiseSize=textureSize(noiseTexture,0);
float noise(float t) {
	return textureLod(noiseTexture,vec2(t,.0)/noiseSize.xy, 0.0).x;
}

float noise( in vec2 x ) {
    vec2 p = floor(x);
    vec2 f = fract(x);

	vec2 uv = p.xy + f.xy*f.xy*(3.0-2.0*f.xy);

	return -1.0 + 2.0*textureLod( noiseTexture, (uv+0.5)/256.0, 0.0 ).x;
}

float noise( in vec3 x )
{
	float  z = x.z*64.0;
	vec2 offz = vec2(0.317,0.123);
	vec2 uv1 = x.xy + offz*floor(z); 
	vec2 uv2 = uv1  + offz;
	return mix(textureLod( noiseTexture, uv1 ,0.0).x,textureLod( noiseTexture, uv2 ,0.0).x,fract(z))-0.5;
}
vec3 lensflare(vec2 uv,vec2 pos) {
	vec2 main = uv-pos;
	vec2 uvd = uv*(length(uv));
	
	float ang = atan(main.x,main.y);
	float dist=length(main); dist = pow(dist,.1);
	float n = noise(vec2(ang*16.0,dist*32.0));
	
	float f0 = 1.0/(length(uv-pos)*20.0+1.0);
	
	f0 = f0+f0*(sin(noise((pos.x+pos.y)*2.2+ang*4.0+5.954)*16.0)*.1+dist*.1+.8);
	
	float f1 = max(0.01-pow(length(uv+1.2*pos),1.9),.0)*7.0;

	float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
	float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
	float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;
	
	vec2 uvx = mix(uv,uvd,-0.5);
	
	float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
	float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
	float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;
	
	uvx = mix(uv,uvd,-.4);
	
	float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
	float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
	float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;
	
	uvx = mix(uv,uvd,-0.5);
	
	float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
	float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
	float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;
	
	vec3 c = vec3(.0);
	
	c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
	c = c*1.3 - vec3(length(uvd)*.05);
	c+=vec3(f0);
	
	return c;
}

void main() {
    vec3 col = vec3(1.0);

    // outScreenLight
    // outScreenLensFlare=vec4(1.0);

    LightData light=lights[0];
    vec4 cameraSpace = (cameraInfo.projection*cameraInfo.view)* vec4(light.transform.position, 1.0);
    vec3 projCoords = cameraSpace.xyz / cameraSpace.w;
    projCoords = projCoords * 0.5 ;
    outScreenLensFlare.xyz=lensflare(UV-0.5,vec2(projCoords.xy));
    // outScreenLensFlare=texture(screenLensFlare, UV) +vec4(0.5);
}
