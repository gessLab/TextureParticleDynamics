/*
 * Author: Henry Jochaniewicz
 * Date Modified: November 20, 2025
 */

#version 410 core

layout (vertices=4) out;

in vec2 vTexCoord[];

uniform mat4 view;
uniform mat4 model;

out vec2 TextureCoord[];

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];

    if(gl_InvocationID == 0) {
        const int MAX_TESS_LEVEL = 16;
        const int MIN_TESS_LEVEL = 2;

        /*
        const float MIN_DISTANCE = 0.0;
        const float MAX_DISTANCE = 2.5;

        vec4 eyeSpaceP0 = view * model * gl_in[0].gl_Position;
        vec4 eyeSpaceP1 = view * model * gl_in[1].gl_Position;
        vec4 eyeSpaceP2 = view * model * gl_in[2].gl_Position;
        vec4 eyeSpaceP3 = view * model * gl_in[3].gl_Position;

        float d0 = clamp((abs(eyeSpaceP0.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float d1 = clamp((abs(eyeSpaceP1.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float d2 = clamp((abs(eyeSpaceP2.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
        float d3 = clamp((abs(eyeSpaceP3.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);

        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(d0, d3));
        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(d0, d1));
        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(d1, d2));
        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(d2, d3));

        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);

        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, d0);
        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, d0);
        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, d0);
        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, d0);
        */

        gl_TessLevelOuter[0] = MAX_TESS_LEVEL;
        gl_TessLevelOuter[1] = MAX_TESS_LEVEL;
        gl_TessLevelOuter[2] = MAX_TESS_LEVEL;
        gl_TessLevelOuter[3] = MAX_TESS_LEVEL;

        gl_TessLevelInner[0] = MAX_TESS_LEVEL;
        gl_TessLevelInner[1] = MAX_TESS_LEVEL;
    }
}
