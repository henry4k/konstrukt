varying vec3 Color;

attribute vec3 VertexPosition;
attribute vec3 VertexColor;
//attribute vec2 VertexTexCoord;
//attribute vec3 VertexNormal;
//attribute vec4 VertexTangent;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
    Color = VertexColor;
}
