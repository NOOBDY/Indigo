#version 450 core

#define KERNEL_SIZE 64

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

layout(location = 0) in vec2 UV;

layout(location = 0) out float fragColor;

uniform sampler2D screenNormal;
uniform sampler2D screenDepth;
uniform sampler2D noise;

layout(std140, binding = 3) uniform Camera {
    CameraData cameraInfo;
};

// hard-coded C++ generated random floats
const vec3 kernel[KERNEL_SIZE] = {
    vec3(-0.0687772, -0.0506844, 0.0519692),
    vec3(-0.0340995, 0.0270217, 0.0902831),
    vec3(-0.0769596, 0.0303901, 0.0577081),
    vec3(0.0853145, -0.0228643, 0.0509713),
    vec3(0.0599235, -0.0842688, 0.00483979),
    vec3(0.0180326, 0.103914, 0.002337),
    vec3(-0.0254276, -0.0944736, 0.0455278),
    vec3(0.0406332, 0.0193572, 0.10121),
    vec3(0.112735, 0.00876978, 0.0149749),
    vec3(0.0462533, -0.0252426, 0.105355),
    vec3(0.0992398, 0.063415, 0.0317383),
    vec3(-0.106845, 0.0557397, 0.0387486),
    vec3(0.0304468, 0.0588584, 0.113745),
    vec3(-0.0324689, -0.0609928, 0.118454),
    vec3(0.0679332, 0.0772983, 0.0993884),
    vec3(-0.101527, 0.0312755, 0.1051),
    vec3(-0.078904, -0.0220749, 0.133045),
    vec3(-0.0122628, -0.144404, 0.0756939),
    vec3(-0.0552434, -0.130908, 0.0954875),
    vec3(0.124795, 0.128421, 0.00950338),
    vec3(0.158396, 0.00177043, 0.101047),
    vec3(-0.0619741, 0.166656, 0.0845838),
    vec3(-0.0721961, -0.126349, 0.146297),
    vec3(-0.197145, 0.000327035, 0.0888345),
    vec3(-0.0936086, 0.173772, 0.111227),
    vec3(-0.0190424, 0.236183, 0.0134122),
    vec3(0.116226, 0.120089, 0.183956),
    vec3(-0.13879, -0.179356, 0.127525),
    vec3(0.18686, 0.0657343, 0.186791),
    vec3(0.0909989, 0.201631, 0.179358),
    vec3(-0.212699, -0.154402, 0.139916),
    vec3(0.00954089, 0.0654906, 0.304037),
    vec3(-0.0756958, 0.29403, 0.115937),
    vec3(-0.114105, 0.0503108, 0.315533),
    vec3(-0.191543, -0.289557, 0.0691864),
    vec3(0.0849965, 0.358712, 0.0196041),
    vec3(0.0276769, -0.00122107, 0.383767),
    vec3(0.194059, 0.0426614, 0.348089),
    vec3(0.139757, 0.382882, 0.0894225),
    vec3(-0.342174, 0.255765, 0.0776832),
    vec3(-0.395478, -0.217954, 0.00145018),
    vec3(-0.0673123, -0.371582, 0.278739),
    vec3(0.412768, -0.245164, 0.0852575),
    vec3(-0.171729, 0.36423, 0.306854),
    vec3(-0.418862, 0.217234, 0.231083),
    vec3(-0.45544, -0.00105056, 0.299232),
    vec3(0.106455, 0.421946, 0.36026),
    vec3(0.573552, 0.0707116, 0.0932926),
    vec3(0.589632, -0.111305, 0.0865101),
    vec3(0.115052, -0.439427, 0.433016),
    vec3(-0.0500366, 0.641423, 0.0876599),
    vec3(-0.428087, -0.257715, 0.448607),
    vec3(-0.509313, 0.20637, 0.424078),
    vec3(0.471888, -0.392613, 0.370905),
    vec3(-0.258605, -0.693322, 0.0331477),
    vec3(0.741061, -0.135607, 0.13101),
    vec3(0.765807, -0.153486, 0.112255),
    vec3(0.553457, -0.585285, 0.116388),
    vec3(-0.770691, -0.241952, 0.22734),
    vec3(-0.612871, 0.475585, 0.382368),
    vec3(-0.3088, -0.0978877, 0.830042),
    vec3(0.842735, 0.296051, 0.210109),
    vec3(0.365778, 0.832856, 0.254717),
    vec3(0.794294, -0.0632549, 0.556834),
};

// Gold Noise ©2015 dcerisano@standard3d.com
// - based on the Golden Ratio
// - uniform normalized distribution
// - fastest static noise generator function (also runs at low precision)
// - use with indicated fractional seeding method.

float PHI = 1.61803398874989484820459; // Φ = Golden Ratio

float gold_noise(in vec2 xy, in float seed) {
    return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

vec3 depth2viewSpace(float depth, mat4 projection, mat4 view) {
    mat4 invert_view_projection = inverse(projection);

    float ViewZ;
    ViewZ = depth * 2.0 - 1.0;

    // way1
    vec4 sPos = vec4(UV * 2.0 - 1.0, ViewZ, 1.0);
    vec4 worldPosition = invert_view_projection * sPos;
    worldPosition = vec4((worldPosition.xyz / worldPosition.w), 1.0f);
    return worldPosition.xyz;
}

// Using this instead of C++ generated random would produce weird patterns
// void GenKernel() {
//     for (int i = 0; i < KERNEL_SIZE; ++i) {
//         kernel[i].x = gold_noise(vec2(UV.x), 0) * 0.5 - 0.5;
//         kernel[i].y = gold_noise(vec2(UV.y), 69) * 0.5 - 0.5;
//         kernel[i].z = gold_noise(UV, 420);

//         float scale = float(i) / float(KERNEL_SIZE);
//         scale = mix(0.1, 1.0, scale * scale);

//         kernel[i] *= scale;
//     }
// }

void main() {
    // screen size / noise texture size
    const vec2 noiseScale = vec2(1280.0 / 20.0, 720.0 / 20.0);

    // This setting would have artifacts and weird "blooming" effect when
    // viewing raw SSAO pass but blended with ambient would hide it. If this
    // value is too small the effects are too insignificant
    const float radius = 10.0;
    const float bias = 0.4;

    vec3 fragPos = depth2viewSpace(texture(screenDepth, UV).x,
                                   cameraInfo.projection, cameraInfo.view);

    vec3 normal = texture(screenNormal, UV).xyz * 2.0 - 1.0;
    normal = (cameraInfo.view * vec4(normal, 0)).xyz;

    vec3 randomVec =
        vec3(texture(noise, UV * noiseScale).xy * 2.0 - 1.0, 0.5 * noiseScale);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; ++i) {
        // get sample position
        vec3 samplePos = TBN * kernel[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = cameraInfo.projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        // get sample depth
        vec3 temp = depth2viewSpace(texture(screenDepth, offset.xy).x,
                                    cameraInfo.projection, cameraInfo.view);
        float sampleDepth = temp.z;

        float rangeCheck =
            smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion +=
            (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / KERNEL_SIZE);

    fragColor = occlusion;
}
