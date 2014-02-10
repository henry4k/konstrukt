varying vec2 TexCoord;
varying vec4 ShadowCoord;
varying float FixedShadowBias;
varying float LightVisibillity;

attribute vec3 VertexPosition;
attribute vec3 VertexColor;
attribute vec2 VertexTexCoord;
attribute vec3 VertexNormal;
attribute vec4 VertexTangent;

uniform mat4 MVP;
uniform mat4 DepthBiasMVP;
uniform vec3 LightDirection;
uniform float ShadowBias;

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    ShadowCoord = DepthBiasMVP * vec4(VertexPosition, 1.0);
    TexCoord    = VertexTexCoord;

    vec3 normal = normalize(VertexNormal);
    float nDotL = dot(normal, LightDirection);
    LightVisibillity = max(0.0, nDotL); // Compute lambert term
    FixedShadowBias = clamp(
        ShadowBias * tan(acos(nDotL)),
        0.0, 0.01
    );
}
