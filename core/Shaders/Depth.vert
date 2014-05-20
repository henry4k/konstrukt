#version 120

varying vec2 TexCoord;

attribute vec3 VertexPosition;
attribute vec2 VertexTexCoord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    TexCoord = VertexTexCoord;
}
