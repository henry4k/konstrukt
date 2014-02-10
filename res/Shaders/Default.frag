varying vec2 TexCoord;
varying vec4 ShadowCoord;
varying float FixedShadowBias;
varying float LightVisibillity;

uniform sampler2D DiffuseSampler;
uniform sampler2D LightDepthSampler;
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;

void main()
{
    float lightVisibillity = LightVisibillity;
    if(texture2D(LightDepthSampler, ShadowCoord.xy).z < ShadowCoord.z-FixedShadowBias)
        lightVisibillity = 0.0;

    vec3 lightColor = LightAmbient + LightDiffuse*lightVisibillity;

    vec4 diffuse = texture2D(DiffuseSampler, TexCoord);
    gl_FragColor.rgb = diffuse.rgb * lightColor;
    gl_FragColor.a   = diffuse.a;
}
