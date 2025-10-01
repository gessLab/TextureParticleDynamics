/*
 * Author: Henry Jochaniewicz
 * Date Modified: September 30, 2025
 */

#version 410 core

layout (vertices=3) out;

in vec2 theTexCoord[];

uniform mat4 view;
uniform mat4 model;

out vec2 TextureCoord[];

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = theTexCoord[gl_InvocationID];

    if(gl_InvocationID == 0) {
        const int MIN_TESS_LEVEL = 2;
        const int MAX_TESS_LEVEL = 64;

        const float MIN_DISTANCE = 3;
        const float MAX_DISTANCE = 5;

        vec4 eyeSpaceP0 = view * model * gl_in[0].gl_Position;
        vec4 eyeSpaceP1 = view * model * gl_in[1].gl_Position;
        vec4 eyeSpaceP2 = view * model * gl_in[2].gl_Position;

        float d0 = clamp((abs(eyeSpaceP0.z) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float d1 = clamp((abs(eyeSpaceP1.z) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float d2 = clamp((abs(eyeSpaceP2.z) - MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

        float tessLevel0 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, min(d0, d1));
        float tessLevel1 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, min(d1, d2));
        float tessLevel2 = mix(MIN_TESS_LEVEL, MAX_TESS_LEVEL, min(d0, d2));

        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;

        gl_TessLevelInner[0] = max(tessLevel0, max(tessLevel1, tessLevel2));

        /*
        gl_TessLevelOuter[0] = MAX_TESS_LEVEL;
        gl_TessLevelOuter[1] = MAX_TESS_LEVEL;
        gl_TessLevelOuter[2] = MAX_TESS_LEVEL;

        gl_TessLevelInner[0] = max(MAX_TESS_LEVEL, max(MAX_TESS_LEVEL, MAX_TESS_LEVEL));
        */
    } 
} 
