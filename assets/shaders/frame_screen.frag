
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform samplerCube depthTexture;
uniform sampler2D uvcheck;
#define PI 3.1415926
vec3 cnn(vec3 color) {
    const float offset = 1.0 / 300.0;
        // https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/05%20Framebuffers/

    vec2 offsets[9] = vec2[](vec2(-offset, offset), // 左上
    vec2(0.0f, offset),     // 正上
    vec2(offset, offset),   // 右上
    vec2(-offset, 0.0f),    // 左
    vec2(0.0f, 0.0f),       // 中
    vec2(offset, 0.0f),     // 右
    vec2(-offset, -offset), // 左下
    vec2(0.0f, -offset),    // 正下
    vec2(offset, -offset)   // 右下
    );

        // edge detction
    float kernel[9] = float[](-1, -1, -1, -1, 8, -1, -1, -1, -1);
        // blur
        // float kernel[9] = float[](1.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 4.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 1.0 / 16);

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    for(int i = 0; i < 9; i++) color += sampleTex[i] * kernel[i];
    return color;

}
vec3 cube_uv(vec2 uv) {
    vec3 nuv = vec3(0.0);
    uv = (uv - vec2(0.5)) * 2 * PI;
    nuv.x = sin(uv.x);
    // nuv.x = cos(uv.x * 2 * PI);
    // nuv.x = uv.x * 2 - 1;

    nuv.z = cos(uv.x);
    // nuv.z = sin(uv.x * 2 * PI);
    // nuv.z = uv.x * 2 - 1;

    nuv.y = cos(uv.y);
    // nuv.y = sin(uv.y);
    // nuv.y = uv.y * 2 - 1;
    // return nuv;
    // return texture(uvcheck, uv).xyz;
    return texture(depthTexture, normalize(nuv)).rgb;

}
vec3 test(vec3 nuv) {
    vec2 uv = vec2(0.0);
    // uv.x = fract(2 * (nuv.x + 1));
    // uv.y= fract(2 * (nuv.y + 1));
    uv.x = 0.5 + atan(nuv.x, nuv.z) / (2 * PI);
    uv.y = 0.5 + asin(nuv.y) / (PI);
    return texture(uvcheck, uv).xyz;
}

void main() {
    vec3 screen = texture(screenTexture, TexCoords).rgb;
    // vec3 depth = texture(depthTexture, TexCoords).rgb;
    // vec3 depth = textureCube(depthTexture, TexCoords).rgb;
    vec3 col = screen;
    // if(TexCoords.x < 0.5)
    //     col = cube_uv(TexCoords);
    // col = test(col);
    FragColor = vec4(col, 1.0);
}