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
    int useToneMap;

    vec3 volumeColor;
    int useVolume;

    float density;
    vec3 pad;
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
layout(location = 1) out vec4 outScreenVolume;

uniform sampler2D screenEmission;
uniform sampler2D screenDepth;
uniform sampler2D noiseTexture;

uniform sampler2D screenLensFlare;
uniform sampler2D screenVolume;

uniform sampler2D directionShadowMap;
uniform samplerCube pointShadowMap;

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

ivec2 noiseSize = textureSize(noiseTexture, 0);
float noise(float t) {
    return textureLod(noiseTexture, vec2(t, .0) / noiseSize.xy, 0.0).x;
}

float noise(in vec2 x) {
    vec2 p = floor(x);
    vec2 f = fract(x);

    vec2 uv = p.xy + f.xy * f.xy * (3.0 - 2.0 * f.xy);

    return -1.0 + 2.0 * textureLod(noiseTexture, (uv + 0.5) / 256.0, 0.0).x;
}

float noise(in vec3 x) {
    float z = x.z * 64.0;
    vec2 offz = vec2(0.317, 0.123);
    vec2 uv1 = x.xy + offz * floor(z);
    vec2 uv2 = uv1 + offz;
    return mix(textureLod(noiseTexture, uv1, 0.0).x,
               textureLod(noiseTexture, uv2, 0.0).x, fract(z)) -
           0.5;
}
vec3 lensflare(vec2 uv, vec2 pos, float z) {
    vec2 main = uv - pos;
    vec2 uvd = uv * (length(uv));

    float ang = atan(main.x, main.y);
    float dist = length(main);
    dist = pow(dist, .1);
    float n = noise(vec2(ang * 16.0, dist * 32.0));

    float f0 = 1.0 / (length(uv - pos) * 20.0 + 1.0);

    f0 = f0 +
         f0 * (sin(noise((pos.x + pos.y) * 2.2 + ang * 4.0 + 5.954) * 16.0) *
                   .1 +
               dist * .1 + .8);

    float f1 = max(0.01 - pow(length(uv + 1.2 * pos), 1.9), .0) * 7.0;

    float f2 =
        max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), .0) * 00.25;
    float f22 =
        max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), .0) *
        00.23;
    float f23 =
        max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), .0) * 00.21;

    vec2 uvx = mix(uv, uvd, -0.5);

    float f4 = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), .0) * 6.0;
    float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), .0) * 5.0;
    float f43 = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), .0) * 3.0;

    uvx = mix(uv, uvd, -.4);

    float f5 = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), .0) * 2.0;
    float f52 = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), .0) * 2.0;
    float f53 = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), .0) * 2.0;

    uvx = mix(uv, uvd, -0.5);

    float f6 = max(0.01 - pow(length(uvx - 0.3 * pos), 1.6), .0) * 6.0;
    float f62 = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), .0) * 3.0;
    float f63 = max(0.01 - pow(length(uvx - 0.35 * pos), 1.6), .0) * 5.0;

    vec3 c = vec3(.0);

    c.r += f2 + f4 + f5 + f6;
    c.g += f22 + f42 + f52 + f62;
    c.b += f23 + f43 + f53 + f63;
    c = c * 1.3 - vec3(length(uvd) * .05);
    // float pix=0.1*z;
    // float radius=max(0.5,0.7);
    // c+=smoothstep(radius+pix,radius-pix,dist);
    // c+=vec3(f0);

    return c;
}

vec4 DirectionVolume(vec3 position, vec3 cameraPos, LightData light) {

    const int n = 64;
    const float sigma_a = 0.1;
    const float bias = 0.001;
    // float maxDistance = 1000;
    float maxDistance = light.radius;
    vec3 view = (length(position - cameraPos) > maxDistance)
                    ? (normalize(position - cameraPos) * maxDistance / n)
                    : (position - cameraPos) / n;
    vec3 samplePos = cameraPos;
    float transmittance = 1;
    vec3 illumination = vec3(0);
    for (int i = 0; i < n; i++) {
        samplePos += view;
        vec4 lightSpace = light.lightProjections[0] * vec4(samplePos, 1.0);
        vec3 projCoords = lightSpace.xyz / lightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        float currentDepth = projCoords.z;
        float closestDepth = texture(directionShadowMap, projCoords.xy).r;
        float currentDensity = pipelineInfo.density;
        if (currentDepth - bias < closestDepth) {
            transmittance *= exp(-(currentDensity)*sigma_a);
            illumination += transmittance * light.lightColor;
        }
    }

    illumination =
        (pipelineInfo.volumeColor * light.lightColor) * illumination / n;
    transmittance = clamp(0, 1, transmittance);
    return vec4(vec3(illumination), transmittance);
}
vec4 PointVolume(vec3 position, vec3 cameraPos, LightData light) {
    const int n = 64;
    // float maxDistance = 1000;
    const float sigma_a = 0.1;
    const float bias = 1.5;
    // clamp distance
    float maxDistance = light.radius;
    vec3 view = (length(position - cameraPos) > maxDistance)
                    ? (normalize(position - cameraPos) * maxDistance / n)
                    : (position - cameraPos) / n;
    vec3 samplePos = cameraPos;
    float transmittance = 1;
    vec3 illumination = vec3(0);

    for (int i = 0; i < n; i++) {
        samplePos += view;
        vec3 lightDir = light.transform.position - samplePos;
        float closestDepth = texture(pointShadowMap, -normalize(lightDir)).r;
        closestDepth *= light.farPlane;
        float currentDensity = pipelineInfo.density;
        transmittance *= exp(-(currentDensity)*sigma_a);
        if (length(lightDir) - bias < closestDepth) {
            illumination += transmittance;
        }
    }
    illumination = (pipelineInfo.volumeColor * light.lightColor) *
                   illumination * light.power / n;
    // transmittance = clamp(0, 1, transmittance);
    return vec4(illumination, transmittance);
}
void main() {
    vec3 col = vec3(1.0);

    // outScreenLight
    // outScreenLensFlare=vec4(1.0);
    vec3 position = depth2position(texture(screenDepth, UV).x,
                                   cameraInfo.projection, cameraInfo.view);

    LightData light = lights[0];
    vec4 cameraSpace = (cameraInfo.projection * cameraInfo.view) *
                       vec4(light.transform.position, 1.0);
    vec3 projCoords = cameraSpace.xyz / cameraSpace.w;
    projCoords = projCoords * 0.5;

    outScreenLensFlare = texture(screenLensFlare, UV);
    outScreenVolume = texture(screenVolume, UV);

    // if light is not in shadow
    if (projCoords.z + 0.5 < texture(screenDepth, projCoords.xy + 0.5).r) {
        vec2 size = textureSize(screenLensFlare, 0);
        size /= size.y;
        size*=2;

        // if(light.lightType==DIRECTION)

        if(light.lightType==DIRECTION)
            outScreenLensFlare.xyz +=
                lensflare((UV - 0.5) * size, projCoords.xy * size,
                        projCoords.z + 0.5) *
                light.power*fade(light.transform.position,cameraInfo.transform.position,light.radius);
    }
    if (pipelineInfo.useVolume == 1)
        if (light.lightType == DIRECTION)
            outScreenVolume +=
                DirectionVolume(position, cameraInfo.transform.position, light);
}
