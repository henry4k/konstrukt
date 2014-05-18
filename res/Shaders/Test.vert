#version 120

attribute vec3 VertexPosition;
attribute vec3 VertexColor;
attribute vec2 VertexTexCoord;
attribute vec3 VertexNormal;
attribute vec4 VertexTangent;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
