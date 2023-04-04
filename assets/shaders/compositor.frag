#version 450 core
layout(location = 0) in vec2 UV;
// layout(location = 0) out vec4 FragColor;
// in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenEmission;
uniform sampler2D screenARM;
uniform sampler2D screenID;
uniform sampler2D screenDepth;

uniform sampler2D screenLight;
uniform sampler2D screenVolume;

// uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
#define PI 3.1415926
vec3 gaussianBlur(sampler2D sampleTexture, float blurStrength, vec2 texCoord) {
#define MAX_LENGTH 8
#define MAX_ROUND 8
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

    col.xyz += gaussianBlur(screenVolume, 0.5, UV);

    // screenID.a = model id ;model start from 1 and need to
    // col = texture(screenID, UV);
    // col.xyz = vec3(col.a * 255 == 2);

    // col = cube_uv(UV);
    FragColor = vec4(col.xyz, 1.0);
}