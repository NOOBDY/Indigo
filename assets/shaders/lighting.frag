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
    int pad0;
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
struct DeferredData {
    vec3 albedo;
    vec3 emission;
    vec3 normal;
    vec3 position;
    vec3 ARM;
    vec4 ID;
    vec3 depth;
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
// in vec2 UV;

layout(location = 0) out vec4 outScreenLight;
layout(location = 1) out vec4 outScreenVolume;

uniform sampler2D screenAlbedo;
uniform sampler2D screenNormal;
uniform sampler2D screenPosition;
uniform sampler2D screenEmission;
uniform sampler2D screenARM;
uniform sampler2D screenID;
uniform sampler2D screenDepth;

uniform sampler2D screenLight;
uniform sampler2D screenVolume;

uniform sampler2D reflectMap;

uniform sampler2D directionShadowMap;
uniform samplerCube pointShadowMap;

uniform sampler2D LUT;
uniform sampler2D ssao;
uniform sampler2D noise;

// uniform samplerCube shadowMap[LIGHT_NUMBER]; // frame buffer texture
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
vec3 gridSamplingDisk[20] =
    vec3[](vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
           vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
           vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
           vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
           vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));
float pointShadow(vec3 position, LightData light) {
    vec3 dir = position - light.transform.position;
    // float lightDepth = texture(pointShadowMap, dir).x;
    // lightDepth *= light.farPlane;
    float currentDepth = length(dir);
    float shadow = 0.0;
    float bias = 1.5;
    int samples = 20;
    float diskRadius = (1.0 + (currentDepth / light.farPlane)) / 15.0;

    for (int i = 0; i < samples; ++i) {
        float closestDepth =
            texture(pointShadowMap, dir + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= light.farPlane; // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return (shadow);
}
float directionShadow(vec3 position, vec3 normal, LightData light) {
    vec4 lightSpace = light.lightProjections[0] * vec4(position, 1.0);
    vec3 projCoords = lightSpace.xyz / lightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // keep the shadow at 0.0 when outside the far_plane region of the light's
    // frustum.
    if (projCoords.z > 1.0)
        return 0.0;
    // get closest depth value from light's perspective (using [0,1] range
    // fragPosLight as coords)
    float closestDepth = texture(directionShadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(light.transform.position - position);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.003);
    float shadow = 0.0;
    // shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // return shadow;
    // PCF
    vec2 texelSize = 1.0 / textureSize(directionShadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(directionShadowMap,
                                     projCoords.xy + vec2(x, y) * texelSize)
                                 .r;
            shadow += (currentDepth - bias) > (pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}
vec4 DirectionVolume(vec3 position, vec3 cameraPos, LightData light) {
    mat4 ditherPattern = {{0.0f, 0.5f, 0.125f, 0.625f},
                          {0.75f, 0.22f, 0.875f, 0.375f},
                          {0.1875f, 0.6875f, 0.0625f, 0.5625},
                          {0.9375f, 0.4375f, 0.8125f, 0.3125}};

    const int n = 64;
    const float sigma_a = 0.1;
    const float bias = 0.001;
    // float maxDistance = 1000;
    float maxDistance=light.radius;
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
        float currentDensity = 0.2;
        if (currentDepth - bias < closestDepth) {
            transmittance *= exp(-(currentDensity)*sigma_a);
            illumination += transmittance * light.lightColor;
        }
    }

    illumination = illumination / n;
    // transmittance = clamp(0, 1, transmittance);
    // return vec4(smoothstep(vec3(0.8), vec3(1.5), illumination), transmittance);
    // return vec4(vec3(v/n)*0.5,transmittance);
    return vec4(vec3(illumination),transmittance);
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
        float currentDensity = 0.02;
        transmittance *= exp(-(currentDensity)*sigma_a);
        if (length(lightDir) - bias < closestDepth) {
            illumination += transmittance * light.lightColor;
        }
    }
    illumination = illumination * light.power / n;
    // transmittance = clamp(0, 1, transmittance);
    return vec4(illumination, transmittance);
}
vec2 panoramaUV(vec3 nuv) {
    vec2 uv = vec2(0.0);
    uv.x = 0.5 + atan(nuv.x, nuv.z) / (2 * PI);
    uv.y = 0.5 + asin(nuv.y) / (PI);
    return uv;
}
float DistributionGGX(float dotNH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = pow(dotNH, 2.0);
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(float dotNV, float dotNL, float roughness) {
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) *
                    pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// St. Peter's Basilica SH
// https://www.shadertoy.com/view/lt2GRD
struct SHCoefficients {
    vec3 l00, l1m1, l10, l11, l2m2, l2m1, l20, l21, l22;
};
const SHCoefficients SH_STPETER =
    SHCoefficients(vec3(0.3623915, 0.2624130, 0.2326261),
                   vec3(0.1759131, 0.1436266, 0.1260569),
                   vec3(-0.0247311, -0.0101254, -0.0010745),
                   vec3(0.0346500, 0.0223184, 0.0101350),
                   vec3(0.0198140, 0.0144073, 0.0043987),
                   vec3(-0.0469596, -0.0254485, -0.0117786),
                   vec3(-0.0898667, -0.0760911, -0.0740964),
                   vec3(0.0050194, 0.0038841, 0.0001374),
                   vec3(-0.0818750, -0.0321501, 0.0033399));

vec3 SHIrradiance(vec3 nrm) {
    const SHCoefficients c = SH_STPETER;
    const float c1 = 0.429043;
    const float c2 = 0.511664;
    const float c3 = 0.743125;
    const float c4 = 0.886227;
    const float c5 = 0.247708;
    return (c1 * c.l22 * (nrm.x * nrm.x - nrm.y * nrm.y) +
            c3 * c.l20 * nrm.z * nrm.z + c4 * c.l00 - c5 * c.l20 +
            2.0 * c1 * c.l2m2 * nrm.x * nrm.y +
            2.0 * c1 * c.l21 * nrm.x * nrm.z +
            2.0 * c1 * c.l2m1 * nrm.y * nrm.z + 2.0 * c2 * c.l11 * nrm.x +
            2.0 * c2 * c.l1m1 * nrm.y + 2.0 * c2 * c.l10 * nrm.z);
}
// https://www.unrealengine.com/en-US/blog/phiysically-based-shading-on-mobile
vec3 EnvBRDFApprox(vec3 SpecularColor, float Roughness, float dotNV) {
    const vec4 c0 = {-1, -0.0275, -0.572, 0.022};
    const vec4 c1 = {1, 0.0425, 1.04, -0.04};
    vec4 r = (Roughness * Roughness) * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * dotNV)) * r.x + r.y;
    vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
    return SpecularColor * AB.x + AB.y;
}
vec4 lighting(vec3 cameraPosition, DeferredData deferredInfo, LightData light,
              int index) {
    vec3 position = deferredInfo.position;
    // mesh normal
    vec3 N = deferredInfo.normal;
    // n = TBN * (texture(texture3, UV).xyz * 2 - 1);
    vec3 direction = normalize(light.transform.direction);
    // direction :light to mesh
    vec3 L = light.lightType == DIRECTION
                 ? direction
                 : normalize(light.transform.position - position);
    // direction :cam to mesh
    vec3 V = normalize(cameraPosition - position);
    // direction :reflection
    vec3 R = normalize(reflect(-V, N));
    // for Blinn-Phong lighting
    vec3 H = normalize(V + L);

    vec3 lightColor = light.useColorTexture == 1
                          ? texture(reflectMap, panoramaUV(-L)).rgb
                          : light.lightColor;
    float fadeOut = light.lightType != DIRECTION
                        ? fade(light.transform.position, position, light.radius)
                        : 1.0;
    // spotlight
    float theta = dot(L, normalize(direction));
    float epsilon =
        cos(radians(light.innerCone)) - cos(radians(light.outerCone));
    float intensity =
        clamp((theta - cos(radians(light.outerCone))) / epsilon, 0.0, 1.0);
    float spot = light.lightType == SPOT ? intensity : 1.0;

    // diffuse lighting
    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);
    float dotNH = max(dot(N, H), 0.0);
    float dotHV = max(dot(H, V), 0.0);
    // shadow
    float shadow = 0;
    if (light.castShadow != 0)
        if (light.lightType == POINT || light.lightType == SPOT) {
            shadow = pointShadow(position, light);
            // outScreenVolume+=PointVolume(position,cameraInfo.transform.position,light);
        } else if (light.lightType == DIRECTION) {
            shadow = directionShadow(position, N, light);
            outScreenVolume +=
                DirectionVolume(position, cameraInfo.transform.position, light);
        }

    // color tranform
    //  diffuse = pow(diffuse, vec3(2.2));

    // Cook-Torrance BRDF
    vec3 albedo = deferredInfo.albedo;
    float ao = deferredInfo.ARM.x *
               (pipelineInfo.useSSAO == 1 ? texture(ssao, UV).r : 1.0);
    float roughness = deferredInfo.ARM.y;
    float metallic = deferredInfo.ARM.z;

    vec3 F0 = mix(vec3(0.01), albedo, metallic);
    float NDF = DistributionGGX(dotNH, roughness);
    float G = GeometrySmith(dotNV, dotNL, roughness);
    vec3 F = light.lightType == AMBIENT
                 ? fresnelSchlickRoughness(dotNV, F0, roughness)
                 : fresnelSchlick(dotHV, F0);
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    vec3 Lo;
    if (light.lightType == AMBIENT) {
        if (light.useColorTexture != 1)
            Lo = albedo * lightColor * light.power * ao * fadeOut;
        else {
            // vec2 brdf = texture(LUT, vec2(dotNV,roughness-0.03)).rg;
            // vec3 diffuse = albedo*texture(reflectMap, panoramaUV(N)).xyz;
            vec3 diffuse = albedo * SHIrradiance(N);
            vec3 env = texture(reflectMap, panoramaUV(R)).xyz;
            vec3 specular = env * EnvBRDFApprox(F0, roughness, dotNV);
            specular *= pow(dotNV + ao, roughness * roughness) - 1.0 + ao;

            Lo = (kD * diffuse * ao + specular) * light.power * fadeOut;
            // outScreenVolume.xyz += specular * light.power * fadeOut;
        }

    } else {
        vec3 diffuse = albedo;
        vec3 specular = NDF * G * F * 0.25 / max(dotNV * dotNL, 0.001) *
                        float(deferredInfo.ID.a != 1.0);
        specular *= pow(dotNV + ao, roughness * roughness) - 1.0 + ao;
        Lo = (kD * ao * diffuse / PI + specular) * dotNL * lightColor *
             (1 - shadow) * light.power * fadeOut * spot;
        // outScreenVolume.xyz += specular * light.power * fadeOut;
    }
    return vec4(Lo, 1);
}
vec4 allLight(DeferredData deferredInfo, CameraData cameraInfo,
              LightData lights[LIGHT_NUMBER]) {
    vec4 color4 = vec4(0);
    for (int i = 0; i < LIGHT_NUMBER; i++) {
        LightData light = lights[i];
        if (light.lightType == 0)
            continue;
        // color3 = light.lightColor;
        color4 +=
            lighting(cameraInfo.transform.position, deferredInfo, light, i) /
            LIGHT_NUMBER;
    }

    return color4;
}

void main() {
    vec3 col = vec3(1.0);

    DeferredData baseInfo;
    baseInfo.albedo = texture(screenAlbedo, UV).rgb;
    baseInfo.normal = texture(screenNormal, UV).rgb * 2.0 - 1.0;
    baseInfo.emission = texture(screenEmission, UV).rgb;
    baseInfo.depth = texture(screenDepth, UV).rgb;
    baseInfo.ARM = texture(screenARM, UV).rgb;
    baseInfo.ID = texture(screenID, UV).rgba;
    baseInfo.position = depth2position(baseInfo.depth.x, cameraInfo.projection,
                                       cameraInfo.view);

    outScreenVolume = texture(screenVolume, UV);
    outScreenLight =
        texture(screenLight, UV) + allLight(baseInfo, cameraInfo, lights);
}
