/*
Authors: Finley Huggins, Everton Albuquerque, Henry Jochaniewicz
Date Modified: November 15, 2025
*/

#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 view;
uniform mat4 proj;

smooth in vec2 texCoord[];
smooth in vec4 position[];

out vec3 normal;
smooth out vec4 thePosition;
smooth out vec2 theTexCoord;

vec3 GetNormal()
{
    vec4 a = gl_in[1].gl_Position - gl_in[0].gl_Position;
    vec4 b = gl_in[2].gl_Position - gl_in[0].gl_Position;
    return normalize(cross(b.xyz, a.xyz));
}

void main()
{
    normal = GetNormal();

    gl_Position = proj * view * gl_in[0].gl_Position;
    theTexCoord = texCoord[0];
    thePosition = view * position[0];
    EmitVertex();

    gl_Position = proj * view * gl_in[1].gl_Position;
    theTexCoord = texCoord[1];
    thePosition = view * position[1];
    EmitVertex();

    gl_Position = proj * view * gl_in[2].gl_Position;
    theTexCoord = texCoord[2];
    thePosition = view * position[2];
    EmitVertex();

	EndPrimitive();
}
