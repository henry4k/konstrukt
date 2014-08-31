#version 120

varying vec3 TexCoord;

attribute vec3 VertexPosition;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
    gl_Position = Projection * View * vec4(VertexPosition, 1.0);
    TexCoord = -normalize( VertexPosition );
}
