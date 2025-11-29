/*
 * Author: Henry Jochaniewicz
 * Date Modified: November 20, 2025
 * Main ideas borrowed from https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation.
 */

#version 410 core

layout (quads, fractional_odd_spacing, ccw) in;

uniform mat4 model;
uniform sampler2D heightTex;
uniform usampler2D tex;

in vec2 TextureCoord[];
out vec2 texCoord;
out vec4 position;

void main() {
    // gl_TessCoord in barycentric coordinates
    vec2 t00 = TextureCoord[0]; 
    vec2 t01 = TextureCoord[1]; 
    vec2 t10 = TextureCoord[2]; 
    vec2 t11 = TextureCoord[3];

    // bilinear interpolation
    vec2 t0 = (t01 - t00) * gl_TessCoord.y + t00;
    vec2 t1 = (t10 - t11) * gl_TessCoord.y + t11;
    texCoord = (t1 - t0) * gl_TessCoord.x + t0;
    // texCoord.x = clamp(texCoord.x, 0.0, 0.9);

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 p0 = (p01 - p00) * gl_TessCoord.y + p00;
    vec4 p1 = (p10 - p11) * gl_TessCoord.y + p11;
    vec4 thisPosition = (p1 - p0) * gl_TessCoord.x + p0;

    thisPosition.y += texture(heightTex, texCoord).r;
    thisPosition.y += float(texture(tex, texCoord).a) / 255.0 / 1.0;

    position = model * thisPosition;
    
    gl_Position = model * thisPosition;
} 
