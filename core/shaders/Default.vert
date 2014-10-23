#version 120

uniform mat4 ModelViewProjection;

attribute vec3 VertexPosition;

varying vec3 Position;

void main()
{
    gl_Position = ModelViewProjection * vec4(VertexPosition, 1.0);
    Position    = VertexPosition;
}
