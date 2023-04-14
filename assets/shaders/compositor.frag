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
layout(std140, binding = 4) uniform PipelineUniform{
    PipelineData pipelineInfo;
};
#define PI 3.1415926
vec3 gaussianBlur(sampler2D sampleTexture, float blurStrength, vec2 texCoord) {
#define MAX_LENGTH 8
#define MAX_ROUND 4
    float blurWidth = blurStrength;
    vec4 blurColor = vec4(texture(sampleTexture, texCoord).xyz, 1.0);
    for (int i = 1; i <= MAX_LENGTH; ++i) {
        float weight = 1.0 - float(i) / float(MAX_LENGTH);
        // weight =1.0-smoothstep(1.,float(MAX_LENGTH),float(i)); // smoothstep
        // way1
        weight = weight * weight * (3.0 - 2.0 * weight); // smoothstep way2

        for (int j = 0; j < MAX_ROUND; ++j) {
            float angle = (float(j) / float(MAX_ROUND) + 0.125) * 2.0 * PI;
            vec2 blurOffset = vec2(cos(angle), sin(angle)) * blurWidth * 0.05;
            // screen aspect ratio
            blurOffset *= vec2(1.0 / 16., 1.0 / 9.0);
            vec4 sampleColor =
                texture(sampleTexture, texCoord + blurOffset * float(i));
            blurColor += vec4(sampleColor.rgb, 1.0) * weight;
        }
    }
    return blurColor.xyz / blurColor.w;
}
float idBorder(sampler2D idPass,int id) {
    if(id==-1)return 0.0;
    const vec2 offset = 1.0 / vec2(textureSize(idPass,0));
        // https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/

    vec2 offsets[9] = vec2[](vec2(-offset.x, offset.y), // 左上
    vec2(0.0f, offset.y),     // 正上
    vec2(offset.x, offset.y),   // 右上
    vec2(-offset.x, 0.0f),    // 左
    vec2(0.0f, 0.0f),       // 中
    vec2(offset.x, 0.0f),     // 右
    vec2(-offset.x, -offset.y), // 左下
    vec2(0.0f, -offset.y),    // 正下
    vec2(offset.x, -offset.y)   // 右下
    );

        // edge detction
    float kernel[9] = float[](-1, -1, -1, -1, 8, -1, -1, -1, -1);
        // blur
        // float kernel[9] = float[](1.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 4.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 1.0 / 16);

    float sampleTex[9];
    float v=0;
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = float(texture(idPass, UV + offsets[i]).a*255==id);
        v+= sampleTex[i] * kernel[i];
    }
    return abs(v);
}
vec2 panoramaUV(vec3 nuv) {
    vec2 uv = vec2(0.0);
    uv.x = 0.5 + atan(nuv.x, nuv.z) / (2 * PI);
    uv.y = 0.5 + asin(nuv.y) / (PI);
    return uv;
}
vec3 cubeUV(samplerCube sampleTexture, vec2 uv) {
    vec3 nuv = vec3(0.0);
    uv = (uv - vec2(0.5)) * 2 * PI;

    nuv.x = sin(uv.x);
    nuv.z = cos(uv.x);
    nuv.y = cos(uv.y);
    return texture(sampleTexture, normalize(nuv)).rgb;
}
vec4 displayPass(int i){
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
        return texture(screenDepth, UV);
    case 8:
        return texture(screenLight, UV);
    case 9:
        return texture(screenVolume, UV);
    case 12:
        return texture(screenLight, UV);
        return texture(screenLight, UV)+vec4(gaussianBlur(screenVolume, 1.0, UV),0.0);
    default:
        return vec4(1);
    }
}

vec3 viewDirection(mat4 projection,mat4 view,vec2 uv) {
    mat4 invert_view_projection = inverse(projection *view);
    float ViewZ;
    ViewZ = 0.5 * 2.0 - 1.0;
    // way1
    vec4 sPos = vec4(uv * 2.0 - 1.0, 1, 1.0);
    vec4 worldPosition = invert_view_projection * sPos;
    return normalize(worldPosition.xyz);
}
void main() {
    vec4 col = displayPass(pipelineInfo.selectPass);

    vec3 dir=viewDirection(cameraInfo.projection,cameraInfo.view,UV);

    col.xyz=mix(col.xyz,texture(reflectMap,panoramaUV(dir)).xyz,float(texture(screenID,UV).w==1.0));

    col.xyz=mix(col.xyz,vec3(0,1,0),idBorder(screenID,pipelineInfo.ID));

    FragColor = vec4(col.xyz, 1.0);
}