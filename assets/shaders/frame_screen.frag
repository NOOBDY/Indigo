#version 450 core
// layout(location = 0) in vec2 UV;
// layout(location = 0) out vec4 FragColor;
in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenARM;
uniform sampler2D screenDepth;

uniform sampler2D screenTexture;
uniform sampler2D uvCheck;
uniform samplerCube depthTexture;

// uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
#define PI 3.1415926
const float kernel[9] = float[](0.0625, 0.125, 0.0625,
                                 0.125, 0.25, 0.125,
                                 0.0625, 0.125, 0.0625); // 3x3 kernel

vec2 offsets[9] = vec2[](vec2(-1.0, 1.0),  // 左上
                            vec2(0.0f, 1.0),     // 正上
                            vec2(1.0, 1.0),   // 右上
                            vec2(-1.0, 0.0f),    // 左
                            vec2(0.0f, 0.0f),       // 中
                            vec2(1.0, 0.0f),     // 右
                            vec2(-1.0, -1.0), // 左下
                            vec2(0.0f, -1.0),    // 正下
                            vec2(1.0, -1.0)   // 右下
);
vec3 gaussianBlur(sampler2D sampleTexture,float blurStrength, vec2 texCoord) {
    #define MAX_BLUR_WIDHT 1000.0
    float blurWidth = blurStrength * float(MAX_BLUR_WIDHT);
    vec4 blurColor=vec4(texture(sampleTexture,texCoord).xyz,1.0);
    vec2 blurOffset = vec2( 0.5);
    for (int i = 1; i <= MAX_BLUR_WIDHT; ++ i)
    {
        if ( float(i) >= blurWidth )
            break;

        float weight = 1.0 - float(i) / blurWidth;
        weight = weight * weight * (3.0 - 2.0 * weight); // smoothstep

        vec4 sampleColor1 = texture(sampleTexture, texCoord + blurOffset * float(i));
        vec4 sampleColor2 = texture(sampleTexture, texCoord - blurOffset * float(i));
        blurColor += vec4(sampleColor1.rgb + sampleColor2.rgb, 2.0) * weight; 
    }
    return blurColor.xyz/blurColor.w;

}
vec3 cnn(vec3 color) {
    const float offset = 1.0 / 300.0;
    // https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/


    // edge detction
    float kernel[9] = float[](-1, -1, -1, -1, 8, -1, -1, -1, -1);
    // blur
    // float kernel[9] = float[](1.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 4.0 /
    // 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 1.0 / 16);

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, UV.st + offsets[i]*offset));
    }
    for (int i = 0; i < 9; i++)
        color += sampleTex[i] * kernel[i];
    return color;
}
vec3 cube_uv(vec2 uv) {
    vec3 nuv = vec3(0.0);
    uv = (uv - vec2(0.5)) * 2 * PI;

    nuv.x = sin(uv.x);
    nuv.z = cos(uv.x);
    nuv.y = cos(uv.y);
    return texture(depthTexture, normalize(nuv)).rgb;
}
vec3 test(vec3 nuv) {
    vec2 uv = vec2(0.0);
    // uv.x = fract(2 * (nuv.x + 1));
    // uv.y = fract(2 * (nuv.y + 1));
    uv.x = 0.5 + atan(nuv.x, nuv.z) / (2 * PI);
    uv.y = 0.5 + asin(nuv.y) / (PI);
    return texture(uvCheck, uv).xyz;
}

void main() {
    vec3 screen = texture(screenTexture, UV).rgb;
    if (UV.x > 0.5) {
        screen = texture(uvCheck, UV).rgb;
        // screen=pow(screen, vec3(1.0/2.2));
        // screen=log(screen);
    }
    vec3 col = screen;
    // vec3 col;
    // col = texture(albedoMap, UV).rgb;

    // col = cube_uv(UV);
    FragColor = vec4(col, 1.0);
}