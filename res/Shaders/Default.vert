varying vec2 TexCoord;
varying vec3 LightColor;

attribute vec3 VertexPosition;
attribute vec3 VertexColor;
attribute vec2 VertexTexCoord;
attribute vec3 VertexNormal;
attribute vec4 VertexTangent;

uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightDirection;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
    TexCoord = VertexTexCoord;

    vec3 normal = normalize(VertexNormal);
    float lambert = max(0.0, dot(normal, LightDirection));
    LightColor = LightAmbient + LightDiffuse*lambert;
}
