#version 120

varying vec3 Color;

attribute vec3 VertexPosition;
attribute vec3 VertexColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    Color = VertexColor;
}
