#version 120

varying vec2 TexCoord;
varying vec4 ShadowCoord;
varying float FixedShadowBias;
varying float LightVisibillity;

uniform sampler2D DiffuseSampler;
uniform sampler2DShadow LightDepthSampler;
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;


vec3 GetShadowCoord()
{
    return ( ShadowCoord.xyz / ShadowCoord.w ) - vec3(0,0,FixedShadowBias);
}

float GetLightVisibillity()
{
    vec3 shadowCoord = GetShadowCoord();
    return shadow2D(LightDepthSampler, shadowCoord.xyz).x;
}

void main()
{
    float lightVisibillity = LightVisibillity * GetLightVisibillity();

    vec3 lightColor = LightAmbient + LightDiffuse*lightVisibillity;

    vec4 diffuse = texture2D(DiffuseSampler, TexCoord);
    gl_FragColor.rgb = diffuse.rgb * lightColor;
    gl_FragColor.a   = 1.0;//diffuse.a;
}
