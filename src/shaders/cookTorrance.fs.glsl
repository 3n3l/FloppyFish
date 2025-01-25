#version 410 core

// NOTE: this must be the same as Config::obstacleAmount
#define NUM_LIGHTS 6

// Get values from vertex shader.
smooth in vec2 vTexCoords;
smooth in vec3 vNormal;
smooth in vec3 vView;

smooth in float vLightDistance[NUM_LIGHTS];
smooth in vec3 vLightDir[NUM_LIGHTS];

// Primary texture mostly used for albedo.
uniform sampler2D albedo;

// Components for Cook-Torrance.
uniform float kdMaterialDiffuse;
uniform float roughness;
uniform float transparency;
uniform vec3 emissiveColour;
uniform float eta;

uniform vec3 lightColour = vec3(1.0f, 1.0f, 1.0f) * 18.0f;
uniform vec3 materialSpecularColour = vec3(1.0f, 1.0f, 1.0f);

const float pi = 3.14159265358979323846f;

// Send colour to screen.
layout(location = 0) out vec4 fColour = vec4(0.0f);

vec3 cook_torrance(vec3 materialDiffuseColour,
    vec3 materialSpecularColour,
    vec3 normal,
    vec3 lightDir,
    vec3 viewDir,
    vec3 lightColour)
{
    // Dot pre-calculation one, sometimes called theta.
    float dotNL = max(0.00000001f, dot(normal, lightDir));
    float factorCookTorranceSpec = 0.0;

    // Half-way-vector.
    vec3 halfway = normalize(lightDir + viewDir);
    // Dot pre-calculation rest.
    float dotNV = max(0.0f, dot(normal, viewDir));
    // Sometimes called gamma.
    float dotNH = dot(normal, halfway);

    // Cook-Torrance.
    // Fresnel - Schlick's-approximation.
    float schlick_r = pow((eta - 1.0) / (eta + 1.0), 2.0);
    // Use dotNV instead of dotVH.
    float fresnelSchlick = schlick_r + ((1.0 - schlick_r) * pow(1.0 - dotNV, 5.0));

    // Microfacet reflections - Trowbridge-Reitz GGX.
    float m_sqrd = roughness * roughness;
    float denominatorGGX = dotNH * dotNH * (m_sqrd * m_sqrd - 1.0) + 1.0;
    denominatorGGX = pi * denominatorGGX * denominatorGGX;
    float microfacetGGXDistribution = m_sqrd / denominatorGGX;

    // Schlick GGX microfacet.
    float kDirect = pow(roughness + 1.0, 2.0) / 8.0;
    float GGX1 = dotNV / ((dotNV * (1.0 - kDirect)) + kDirect);
    float GGX2 = dotNL / ((dotNL * (1.0 - kDirect)) + kDirect);
    float geometryTermSchlickGGX = GGX1 * GGX2;

    // Specular factor.
    float numeratorCT = fresnelSchlick * microfacetGGXDistribution * geometryTermSchlickGGX;
    float denominatorCT = 4.0 * dotNL * dotNV;
    factorCookTorranceSpec = numeratorCT / denominatorCT;

    // Oren Nayar.
    float dotVL = dot(lightDir, viewDir);
    float s = dotVL - dotNL * dotNV;
    float t = mix(1.0, max(dotNL, dotNV), step(0.0, s));

    // The m_sqrd is the squared roughness.
    vec3 A = 1.0 + m_sqrd * (materialDiffuseColour / (m_sqrd + 0.13) + 0.5 / (m_sqrd + 0.33));
    float B = 0.45 * m_sqrd / (m_sqrd + 0.09);

    vec3 orenNayarDiffuse = materialDiffuseColour * lightColour * max(0.0, dotNL) * (A + B * s / t) / pi;

    // Combine oren-nayar diffuse with cook-torrance specular.
    return orenNayarDiffuse + materialSpecularColour * lightColour * factorCookTorranceSpec;
}

void main(void)
{
    // If the texture is emissive, just set the colour to a bright colour.
    if (emissiveColour.r + emissiveColour.g + emissiveColour.b > 0.01)
    {
        fColour = vec4(1.8f * emissiveColour, 1.0f);
        return;
    }

    // Sample the texture.
    vec4 textureColour = texture(albedo, vec2(vTexCoords.s, -vTexCoords.t));

    // Lighting prequisites.
    vec3 normal = normalize(vNormal);

    // Viewing direction, aka. Omega_out.
    vec3 view_vec = normalize(vView);

    for (int i = 0; i < NUM_LIGHTS; i++) {
        // Direction towards the light, aka. -Omega_in.
        vec3 light_vec = normalize(vLightDir[i]);

        // Attenuate the light source.
        float a_quadratic_attenuation_term = 0.18f;
        float b_linear_attenuation_term = 3.6f;
        float attenuation = b_linear_attenuation_term * vLightDistance[i] + 1.0f;
        attenuation += a_quadratic_attenuation_term * vLightDistance[i] * vLightDistance[i];
        vec3 attenuated_light = lightColour / attenuation;

        // Incorporate illumination from the light.
        vec3 colourCookTorrance = cook_torrance(textureColour.rgb, materialSpecularColour, normal, light_vec, view_vec, attenuated_light);
        fColour += vec4(colourCookTorrance, transparency);
    }
}
