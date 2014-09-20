#version 120

uniform mat4 ModelViewProjection;

attribute vec3 VertexPosition;
attribute vec3 VertexColor;
attribute vec2 VertexTexCoord;
attribute vec3 VertexNormal;
attribute vec4 VertexTangent;

varying vec2 TexCoord;

void main()
{
    gl_Position = ModelViewProjection * vec4(VertexPosition, 1.0);
    TexCoord    = VertexTexCoord;
}
