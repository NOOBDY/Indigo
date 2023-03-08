#version 450 core
// layout(location = 0) in vec2 UV;
// layout(location = 0) out vec4 FragColor;
in vec2 UV;
out vec4 FragColor;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenEmisstion;
uniform sampler2D screenARM;
uniform sampler2D screenDepth;

uniform sampler2D screenLight;
uniform sampler2D screenVolume;

// uniform sampler2D screenTexture;
// uniform sampler2D uvCheck;
// uniform samplerCube depthTexture;

// uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
#define PI 3.1415926
vec3 gaussianBlur(sampler2D sampleTexture,float blurStrength, vec2 texCoord) {
    #define MAX_BLUR_WIDHT 5
    float blurWidth = blurStrength * float(MAX_BLUR_WIDHT);
    vec4 blurColor=vec4(texture(sampleTexture,texCoord).xyz,1.0);
    vec2 blurOffset = vec2( 0.0005);
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
vec3 cube_uv(samplerCube sampleTexture,vec2 uv) {
    vec3 nuv = vec3(0.0);
    uv = (uv - vec2(0.5)) * 2 * PI;

    nuv.x = sin(uv.x);
    nuv.z = cos(uv.x);
    nuv.y = cos(uv.y);
    return texture(sampleTexture, normalize(nuv)).rgb;
}

void main() {
    vec3 screen = texture(screenEmisstion, UV).rgb;
    if (UV.x > 0.5) {
        // screen = texture(uvCheck, UV).rgb;
        // screen=pow(screen, vec3(1.0/2.2));
        // screen=log(screen);
    }
    vec3 col = screen;
    // col=gaussianBlur(screenLight,UV.x, UV);
    // vec3 col;
    // col = texture(albedoMap, UV).rgb;

    // col = cube_uv(UV);
    FragColor = vec4(col, 1.0);
}