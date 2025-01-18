#version 410 core

// Get values from vertex shader.
smooth in vec2 vTexCoords;
smooth in vec3 vNormal;
smooth in vec3 vView;

smooth in vec3 vLightDir;
smooth in float vLightDistance;

// The texture of the current planetoid.
// Primary texture mostly used for albedo.
uniform sampler2D albedo;

// Components for Cook-Toorance.
uniform float kdMaterialDiffuse;
uniform float roughness;
uniform float transparency;
uniform vec3 emissiveColour;
uniform float eta;

uniform vec3 lightColour = vec3(10.0f, 10.0f, 10.0f);
uniform vec3 materialSpecularColour = vec3(1.0f, 1.0f, 1.0f);

const float pi = 3.14159265358979323846f;

// Send color to screen.
layout (location = 0) out vec4 fColour;

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

    // Combine lambertian diffuse with cook-torrance specular.
    return materialDiffuseColour * lightColour * dotNL
    + materialSpecularColour * lightColour * factorCookTorranceSpec;
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
    // Direction towards the light, aka. -Omega_in.
    vec3 light_vec = normalize(vLightDir);

    // Viewing direction, aka. Omega_out.
    vec3 view_vec = normalize(vView);

    // Attenuate the light source.
    float a_quadratic_attenuation_term = 0.03f;
    float b_linear_attenuation_term = 0.6f;
    float intensity_attenuation_factor = 1.0f / (a_quadratic_attenuation_term * vLightDistance * vLightDistance + b_linear_attenuation_term * vLightDistance + 1.0f);

    // Incorporate illumination from the light.
    vec3 colourCookTorrance = cook_torrance(textureColour.rgb,
                                            materialSpecularColour,
                                            normal,
                                            light_vec,
                                            view_vec,
                                            lightColour * intensity_attenuation_factor);
    fColour = vec4(colourCookTorrance, transparency);
}