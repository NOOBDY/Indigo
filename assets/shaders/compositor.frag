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
vec3 cube_uv(samplerCube sampleTexture, vec2 uv) {
    vec3 nuv = vec3(0.0);
    uv = (uv - vec2(0.5)) * 2 * PI;

    nuv.x = sin(uv.x);
    nuv.z = cos(uv.x);
    nuv.y = cos(uv.y);
    return texture(sampleTexture, normalize(nuv)).rgb;
}

void main() {
    vec4 col = texture(screenLight, UV).rgba;
    // col+=gaussianBlur(screenEmission,0.5, UV);
    // if(UV.x<0.5)
    //     col=texture(screenVolume,(UV*vec2(2.0,1.0)));    for(int i = 0; i < 9; i++) color +=;

    col.xyz=mix(col.xyz,vec3(0,1,0),idBorder(screenID,pipelineInfo.ID));
    // if(pipelineInfo.selectPass==12)
    // col=vec4(1);
    // col.xyz=pipelineInfo.borderColor;
    // col.xyz += gaussianBlur(screenVolume, 1.0, UV);
    // col-=texture(screenVolume,UV).rgba;

    // screenID.a = model id ;model start from 1 and need to
    // col = texture(screenID, UV);
    // col.xyz = vec3(col.a * 255 == 2);

    // col = cube_uv(UV);
    FragColor = vec4(col.xyz, 1.0);
}