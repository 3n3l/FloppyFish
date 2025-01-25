#version 410 core

#define DRAG_MULT 0.38 // changes how much waves pull on the water
#define WATER_DEPTH 1.0 // how deep is the water
#define CAMERA_HEIGHT 1.5 // how high the camera should be
#define ITERATIONS_RAYMARCH 12 // waves iterations of raymarching
#define ITERATIONS_NORMAL 36 // waves iterations when calculating normals

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

uniform samplerCube skybox_texture;
uniform float elapsed_time;
uniform vec2 resolution;
uniform vec3 moon_direction;

// Send colour to screen.
layout (location = 0) out vec4 fcolour;

//smooth in vec3 direction;

// afl_ext 2017-2024
// MIT License

// Use your mouse to move the camera around! Press the Left Mouse Button on the image to look around!


#define NormalizedMouse vec2(0, 0) // normalize mouse coords

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

//// Helper function generating a rotation matrix around the axis by the angle
//mat3 createRotationMatrixAxisAngle(vec3 axis, float angle) {
//    float s = sin(angle);
//    float c = cos(angle);
//    float oc = 1.0 - c;
//    return mat3(
//    oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
//    oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
//    oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c
//    );
//}

// Helper function that generates camera ray based on UV.
vec3 getRay(vec2 fragCoord) {
    vec2 uv = ((fragCoord.xy / resolution.xy) * 2.0 - 1.0) * vec2(resolution.x / resolution.y, 1.0);
    // For fisheye, uncomment following line and comment the next one.
    //    vec3 proj = normalize(vec3(uv.x, uv.y, 1.0) + vec3(uv.x, uv.y, -1.0) * pow(length(uv), 2.0) * 0.05);
    vec3 proj = normalize(vec3(uv.x, uv.y, 1.5));
    if (resolution.x < 600.0) {
        return proj;
    }
    return vec3(inverse(transpose(modelview_matrix)) * vec4(proj, 1.0));
}

// Ray-Plane intersection checker
float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal) {
    return clamp(dot(point - origin, normal) / dot(direction, normal), -1.0, 9991999.0);
}

// Some very barebones but fast atmosphere approximation
vec3 extra_cheap_atmosphere(vec3 raydir, vec3 sundir) {
    //sundir.y = max(sundir.y, -0.07);
    float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
    float special_trick2 = 1.0 / (sundir.y * 11.0 + 1.0);
    float raysundt = pow(abs(dot(sundir, raydir)), 2.0);
    float sundt = pow(max(0.0, dot(sundir, raydir)), 8.0);
    float mymie = sundt * special_trick * 0.2;
    vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - vec3(5.5, 13.0, 22.4) / 22.4), special_trick2);
    vec3 bluesky = vec3(5.5, 13.0, 22.4) / 22.4 * suncolor;
    vec3 bluesky2 = max(vec3(0.0), bluesky - vec3(5.5, 13.0, 22.4) * 0.002 * (special_trick + -6.0 * sundir.y * sundir.y));
    bluesky2 *= special_trick * (0.24 + raysundt * 0.24);

    return vec3(texture(skybox_texture, raydir));

    return bluesky2 * (1.0 + 1.0 * pow(1.0 - raydir.y, 3.0));
}

// Get atmosphere color for given direction
vec3 getAtmosphere(vec3 dir) {
    return extra_cheap_atmosphere(dir, moon_direction) * 0.5;
}

// Get sun color for given direction
float getMoon(vec3 dir) {
    return pow(max(0.0, dot(dir, moon_direction)), 2160.0) * 210.0;
}

// Main.
void main(void) {
    vec2 fragCoord = gl_FragCoord.xy;

    // Get the ray.
    vec3 ray = getRay(fragCoord);

    // If ray.y is positive, render the sky.
    if (ray.y >= 0.0) {
        //        float moonIntensity = getMoon(ray);
        //        vec4 moonColour = vec4(246, 241, 213, 255) / 255;
        //        fcolour = moonColour * moonIntensity;
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

    // mooth the normal with distance to avoid disturbing high frequency noise.
    N = mix(N, vec3(0.0, 1.0, 0.0), 0.8 * min(1.0, sqrt(dist * 0.01) * 1.1));

    // Calculate fresnel coefficient.
    float fresnel = (0.04 + (1.0 - 0.04) * (pow(1.0 - max(0.0, dot(-N, ray)), 5.0)));

    // reflect the ray and make sure it bounces up.
    vec3 R = normalize(reflect(ray, N));
    R.y = abs(R.y);

    // Calculate the reflection and approximate subsurface scattering.
    vec3 reflection = getAtmosphere(R) + getMoon(R);
    vec3 scattering = vec3(0.0293, 0.0698, 0.1717) * 0.1 * (0.2 + (waterHitPos.y + WATER_DEPTH) / WATER_DEPTH);

    // Return the combined result.
    vec3 C = fresnel * reflection + scattering;
    fcolour = vec4((C * 2.0), 1.0);

    //    vec4 texColour = texture(skybox_texture, direction);
}
