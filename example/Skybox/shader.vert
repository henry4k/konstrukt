#version 120

varying vec3 TexCoord;

attribute vec3 VertexPosition;

uniform mat4 ModelViewProjection;

void main()
{
    gl_Position = ModelViewProjection * vec4(VertexPosition, 1.0);
    TexCoord = -normalize( VertexPosition );
}
