varying vec2 TexCoord;
varying vec3 LightColor;

attribute vec3 VertexPosition;
attribute vec3 VertexColor;
attribute vec2 VertexTexCoord;
attribute vec3 VertexNormal;
attribute vec4 VertexTangent;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition, 1.0);
    TexCoord = VertexTexCoord;

    vec3 normal = normalize(VertexNormal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));
    float lambert = max(0.0, dot(normal, lightDir));
    LightColor = vec3(0.1, 0.1, 0.1) + vec3(0.9, 0.9, 0.9)*lambert;
}
