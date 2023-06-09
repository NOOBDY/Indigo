#version 450 core
layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 FragColor;
// out vec4 FragColor;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenEmission;
uniform sampler2D screenARM;
uniform sampler2D screenID;
uniform sampler2D screenDepth;
uniform sampler2D screenLensFlare;

uniform sampler2D ssao;

uniform sampler2D reflectMap;

uniform sampler2D directionShadowMap;
uniform samplerCube pointShadowMap;

uniform sampler2D screenLight;
uniform sampler2D screenVolume;

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
struct CameraData {
    TransformData transform;
    mat4 projection;
    mat4 view;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float FOV;
};
layout(std140, binding = 3) uniform Camera {
    CameraData cameraInfo;
};
layout(std140, binding = 4) uniform PipelineUniform {
    PipelineData pipelineInfo;
};
#define PI 3.1415926
vec3 gaussianBlur(sampler2D sampleTexture, float blurStrength, vec2 texCoord,
                  int MAX_LENGTH, int MAX_ROUND) {
    float blurWidth = blurStrength;
    vec4 blurColor = vec4(texture(sampleTexture, texCoord).xyz, 1.0);
    for (int i = 1; i <= MAX_LENGTH; ++i) {
        float weight = 1.0 - float(i) / float(MAX_LENGTH);
        weight = weight * weight * (3.0 - 2.0 * weight); // smoothstep way2

        for (int j = 0; j < MAX_ROUND; ++j) {
            float angle = (float(j) / float(MAX_ROUND) + 0.125) * 2.0 * PI;
            vec2 blurOffset = vec2(cos(angle), sin(angle)) * blurWidth * i /
                              float(MAX_LENGTH);
            // screen aspect ratio

            blurOffset *= 1.0 / vec2(textureSize(sampleTexture, 0));
            vec4 sampleColor = texture(sampleTexture, texCoord + blurOffset);
            blurColor += vec4(sampleColor.rgb, 1.0) * weight;
        }
    }
    return clamp(blurColor.xyz / blurColor.w, 0.0, 1.0);
}
float idBorder(sampler2D idPass, int id) {
    if (id == -1)
        return 0.0;
    const vec2 offset = 1.0 / vec2(textureSize(idPass, 0));
    // https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/

    vec2 offsets[9] = vec2[](vec2(-offset.x, offset.y),  // 左上
                             vec2(0.0f, offset.y),       // 正上
                             vec2(offset.x, offset.y),   // 右上
                             vec2(-offset.x, 0.0f),      // 左
                             vec2(0.0f, 0.0f),           // 中
                             vec2(offset.x, 0.0f),       // 右
                             vec2(-offset.x, -offset.y), // 左下
                             vec2(0.0f, -offset.y),      // 正下
                             vec2(offset.x, -offset.y)   // 右下
    );

    // edge detction
    float kernel[9] = float[](-1, -1, -1, -1, 8, -1, -1, -1, -1);

    float v = 0;
    for (int i = 0; i < 9; i++) {
        float temp = float(int(texture(idPass, UV + offsets[i]).a * 255) == id);
        v += temp * kernel[i];
    }
    return floor(abs(v) * 0.4);
}
vec2 panoramaUV(vec3 nuv) {
    vec2 uv = vec2(0.0);
    uv.x = 0.50 + atan(nuv.x, nuv.z) / (2.0 * PI);
    uv.y = 0.50 + asin(nuv.y) / (PI);
    return uv;
}
vec4 displayPass(int i) {
    switch (i) {
    case 0:
        return texture(screenAlbedo, UV);
    case 1:
        return texture(screenEmission, UV);
    case 2:
        return texture(screenNormal, UV);
    case 3:
        return texture(screenARM, UV);
    case 4:
        return texture(screenPosition, UV);
    case 5:
        return texture(screenID, UV);
    case 6:
        return vec4(texture(screenDepth, UV).r);
    case 9:
        return vec4(texture(ssao, UV).r);
    case 10:
        return texture(screenLight, UV);
    case 11:
        return texture(screenVolume, UV);
    case 14:
        return texture(screenLensFlare, UV);
    case 15:
        return texture(screenLight, UV) +
               vec4(gaussianBlur(screenVolume, 10, UV, 1, 4), 0.0) +
               //    vec4(gaussianBlur(screenEmission, 15, UV, 3, 4), 0.0) +
               texture(screenLensFlare, UV);
    default:
        return vec4(1);
    }
}

vec3 viewDirection(mat4 projection, mat4 view, vec2 uv) {
    mat4 invert_view_projection = inverse(projection * view);
    float ViewZ;
    ViewZ = 0.9 * 2.0 - 1.0;
    // way1
    vec4 sPos = vec4(uv * 2.0 - 1.0, 1, 1.0);
    vec4 worldPosition = invert_view_projection * sPos;
    return normalize(worldPosition.xyz);
}

// Based on
// https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
const mat3 ACESInput = {
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777},
};

const mat3 ACESOutput = {
    {1.60475, -0.53108, -0.07367},
    {-0.10208, 1.10813, -0.00605},
    {-0.00327, -0.07276, 1.07602},
};

vec3 RRTAndODTFit(vec3 v) {
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

// This is meant to be paired with sRGB
vec3 ACESFitted(vec3 color) {
    color = color * ACESInput;

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = color * ACESOutput;

    return color;
}

float max3(float a, float b, float c) {
    return max(max(a, b), c);
}

float max3(vec3 vec) {
    return max(max(vec.x, vec.y), vec.z);
}

float min3(float a, float b, float c) {
    return min(min(a, b), c);
}

float min3(vec3 vec) {
    return min(min(vec.x, vec.y), vec.z);
}

vec3 RGBtoHSV(vec3 rgb) {
    float h, s, v;

    float cmax = max3(rgb);
    float cmin = min3(rgb);
    float delta = cmax - cmin;

    if (cmax == rgb.r)
        h = 0 + (rgb.g - rgb.b) / delta;

    if (cmax == rgb.g)
        h = 2 + (rgb.b - rgb.r) / delta;

    if (cmax == rgb.b)
        h = 4 + (rgb.r - rgb.g) / delta;

    h = fract(h / 6);

    if (cmax == 0)
        s = 0;
    else
        s = delta / cmax;

    v = cmax;

    return vec3(h, s, v);
}

vec3 HSVtoRGB(vec3 hsv) {
    vec3 rgb;

    float h = fract(hsv.x);
    rgb.r = abs(h * 6 - 3) - 1;
    rgb.g = 2 - abs(h * 6 - 2);
    rgb.b = 2 - abs(h * 6 - 4);

    rgb = clamp(rgb, vec3(0.0), vec3(1.0));

    rgb = mix(vec3(1.0), rgb, hsv.y);
    rgb *= hsv.z;

    return rgb;
}

void main() {
    vec4 col = displayPass(pipelineInfo.selectPass);

    // environment
    if (pipelineInfo.useHDRI != 0) {
        vec3 dir = viewDirection(cameraInfo.projection, cameraInfo.view, UV);
        col.xyz += texture(reflectMap, panoramaUV(dir)).xyz *
                   float(texture(screenID, UV).w == 0.0);
    }

    if (pipelineInfo.useOutline != 0) {
        col.xyz =
            mix(col.xyz, vec3(0, 1, 0), idBorder(screenID, pipelineInfo.ID));
    }

    // if (pipelineInfo.useToneMap != 0) {
    if (true) {
        col.xyz = ACESFitted(col.xyz);
    }

    FragColor = vec4(col.xyz, 1.0);
}
