/*
 * Author: Henry Jochaniewicz
 * Date Modified: October 1, 2025
 */

#version 410 core

layout (triangles, equal_spacing, ccw) in;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

in vec2 TextureCoord[];
out vec2 texCoord;

void main() {
    // gl_TessCoord in barycentric coordinates
    vec2 t0 = TextureCoord[0]; 
    vec2 t1 = TextureCoord[1]; 
    vec2 t2 = TextureCoord[2]; 

    texCoord = t0 * gl_TessCoord[0] + t1 * gl_TessCoord[1] + t2 * gl_TessCoord[2];

    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    vec4 position = p0 * gl_TessCoord[0] + p1 * gl_TessCoord[1] + p2 * gl_TessCoord[2];

    gl_Position = proj * view * model * position;
} 
