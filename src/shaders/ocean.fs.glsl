#version 410 core

// afl_ext 2017-2024
// MIT License

#define DRAG_MULT 0.38 // changes how much waves pull on the water
#define WATER_DEPTH 1.0 // how deep is the water
#define CAMERA_HEIGHT 1.5 // how high the camera should be
#define ITERATIONS_RAYMARCH 12 // waves iterations of raymarching
#define ITERATIONS_NORMAL 36 // waves iterations when calculating normals

uniform mat4 sky_rotation_matrix;

uniform samplerCube skybox_texture;
uniform float elapsed_time;
uniform vec2 resolution;
uniform vec3 moon_direction;

// Send colour to screen.
layout (location = 0) out vec4 fcolour;

smooth in vec3 direction;

// Calculates wave value and its derivative,
// for the wave direction, position in space, wave frequency and time
vec2 wavedx(vec2 position, vec2 direction, float frequency, float timeshift) {
    float x = dot(direction, position) * frequency + timeshift;
    float wave = exp(sin(x) - 1.0);
    float dx = wave * cos(x);
    return vec2(wave, -dx);
}

// Calculates waves by summing octaves of various waves with various parameters
float getwaves(vec2 position, int iterations) {
    float wavePhaseShift = length(position) * 0.1; // this is to avoid every octave having exactly the same phase everywhere
    float iter = 0.0; // this will help generating well distributed wave directions
    float frequency = 1.0; // frequency of the wave, this will change every iteration
    float timeMultiplier = 2.0; // time multiplier for the wave, this will change every iteration
    float weight = 1.0;// weight in final sum for the wave, this will change every iteration
    float sumOfValues = 0.0; // will store final sum of values
    float sumOfWeights = 0.0; // will store final sum of weights
    for (int i = 0; i < iterations; i++) {
        // generate some wave direction that looks kind of random
        vec2 p = vec2(sin(iter), cos(iter));

        // calculate wave data
        vec2 res = wavedx(position, p, frequency, elapsed_time * timeMultiplier + wavePhaseShift);

        // shift position around according to wave drag and derivative of the wave
        position += p * res.y * weight * DRAG_MULT;

        // add the results to sums
        sumOfValues += res.x * weight;
        sumOfWeights += weight;

        // modify next octave ;
        weight = mix(weight, 0.0, 0.2);
        frequency *= 1.18;
        timeMultiplier *= 1.07;

        // add some kind of random value to make next wave look random too
        iter += 1232.399963;
    }
    // calculate and return
    return sumOfValues / sumOfWeights;
}

// Raymarches the ray from top water layer boundary to low water layer boundary.
float raymarchwater(vec3 camera, vec3 start, vec3 end, float depth) {
    vec3 pos = start;
    vec3 dir = normalize(end - start);
    for (int i = 0; i < 64; i++) {
        // The height is from 0 to -depth.
        float height = getwaves(pos.xz, ITERATIONS_RAYMARCH) * depth - depth;
        // Ff the waves height almost nearly matches the ray height, assume its a hit and return the hit distance.
        if (height + 0.01 > pos.y) {
            return distance(pos, camera);
        }
        // Iterate forwards according to the height mismatch.
        pos += dir * (pos.y - height);
    }
    // If hit was not registered, just assume hit the top layer,
    // this makes the raymarching faster and looks better at higher distances.
    return distance(start, camera);
}

// Calculate normal at point by calculating the height at the pos and 2 additional points very close to pos.
vec3 normal(vec2 pos, float e, float depth) {
    vec2 ex = vec2(e, 0);
    float H = getwaves(pos.xy, ITERATIONS_NORMAL) * depth;
    vec3 a = vec3(pos.x, H, pos.y);
    return normalize(
        cross(
            a - vec3(pos.x - e, getwaves(pos.xy - ex.xy, ITERATIONS_NORMAL) * depth, pos.y),
            a - vec3(pos.x, getwaves(pos.xy + ex.yx, ITERATIONS_NORMAL) * depth, pos.y + e)
        )
    );
}

// Ray-Plane intersection checker.
float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal) {
    return clamp(dot(point - origin, normal) / dot(direction, normal), -1.0, 9991999.0);
}

// Some very barebones but fast atmosphere approximation.
vec3 extra_cheap_atmosphere(vec3 raydir, vec3 moondir) {
    moondir.y = max(moondir.y, -0.07);

    // Skybox.
    float sky_colour_factor = 0.8f;
    float sky_colour_power = 2.2f;
    vec3 sky_direction = vec3(inverse(sky_rotation_matrix) * vec4(raydir, 1.0f));
    vec3 stars = vec3(texture(skybox_texture, sky_direction));
    stars = pow(stars, vec3(sky_colour_power)) * sky_colour_factor;

    float moon_intensity = smoothstep(-0.07, 0.07, moon_direction.y);
    if (moondir.y <= -0.07f) return stars / (moon_intensity + 1.0f);

    // Atmosphere.
    // Darker towards sky - lighter towards horizon.
    float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
    // Bright when moon at horizon.
    float special_trick2 = 1.0 / (moondir.y * 11.0 + 1.0);
    // Moon glow.
    float raymoondt = pow(abs(dot(moondir, raydir)), 2.0);
    // Moon glow.
    float moondt = pow(max(0.0, dot(moondir, raydir)), 8.0);

    // Colours.
    vec3 brightblue = vec3(5.5, 7.0, 18.4) / 5.0f;

    // Sunset effect.
    vec3 mooncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0, 0.9, 1.1) - brightblue / 22.4), special_trick2);
    // Blue sky, greenish when moon at horizon.
    vec3 bluesky = brightblue / 22.4 * mooncolor;

    vec3 bluesky2 = max(vec3(0.0), bluesky - brightblue * 0.002 * (special_trick + -6.0 * moondir.y * moondir.y));
    bluesky2 *= special_trick * (0.24 + raymoondt * 0.24);
    bluesky2 *= (1.0 + 1.0 * pow(1.0 - raydir.y, 3.0));

    return max(bluesky2 * moon_intensity, stars / (moon_intensity + 1.0f));
}

// Get atmosphere color for given direction.
vec3 getAtmosphere(vec3 dir) {
    return extra_cheap_atmosphere(dir, moon_direction) * 0.5;
}

// Get moon color for given direction.
float getMoon(vec3 dir) {
    return pow(max(0.0, dot(dir, moon_direction)), 2160.0) * 10.0;
}

// Main.
void main(void) {
    vec2 fragCoord = gl_FragCoord.xy;

    vec3 ray = normalize(direction);

    // If ray.y is positive, render the sky.
    if (ray.y >= 0.0) {
        vec3 C = getAtmosphere(ray) + getMoon(ray);
        fcolour = vec4(C * 2.0, 1.0);
        return;
    }

    // Now ray.y must be negative, water must be hit.
    // Define water planes.
    vec3 waterPlaneHigh = vec3(0.0, 0.0, 0.0);
    vec3 waterPlaneLow = vec3(0.0, -WATER_DEPTH, 0.0);

    // Define ray origin, moving around.
    vec3 origin = vec3(elapsed_time * 0.2, CAMERA_HEIGHT, 1);

    // Calculate intersections and reconstruct positions.
    float highPlaneHit = intersectPlane(origin, ray, waterPlaneHigh, vec3(0.0, 1.0, 0.0));
    float lowPlaneHit = intersectPlane(origin, ray, waterPlaneLow, vec3(0.0, 1.0, 0.0));
    vec3 highHitPos = origin + ray * highPlaneHit;
    vec3 lowHitPos = origin + ray * lowPlaneHit;

    // Raymatch water and reconstruct the hit pos.
    float dist = raymarchwater(origin, highHitPos, lowHitPos, WATER_DEPTH);
    vec3 waterHitPos = origin + ray * dist;

    // Calculate normal at the hit position.
    vec3 N = normal(waterHitPos.xz, 0.01, WATER_DEPTH);

    // Sooth the normal with distance to avoid disturbing high frequency noise.
    N = mix(N, vec3(0.0, 1.0, 0.0), 0.8 * min(1.0, sqrt(dist * 0.01) * 1.1));

    // Calculate fresnel coefficient.
    float fresnel = (0.04 + (1.0 - 0.04) * (pow(1.0 - max(0.0, dot(-N, ray)), 5.0)));

    // Reflect the ray and make sure it bounces up.
    vec3 R = normalize(reflect(ray, N));
    R.y = abs(R.y);

    // Calculate the reflection and approximate subsurface scattering.
    vec3 reflection = getAtmosphere(R) + getMoon(R);
    vec3 scattering = vec3(0.0293, 0.0698, 0.1717) * 0.1 * (0.2 + (waterHitPos.y + WATER_DEPTH) / WATER_DEPTH);

    // Return the combined result.
    vec3 C = fresnel * reflection + scattering;
    fcolour = vec4((C * 2.0), 1.0);
}
