#version 120

varying vec3 TexCoord;

attribute vec3 VertexPosition;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    TexCoord = -normalize( VertexPosition );
}
