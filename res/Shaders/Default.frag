varying vec2 TexCoord;
varying vec3 LightColor;

uniform sampler2D DiffuseSampler;
//uniform sampler2D LightDepthSampler;

void main()
{
    gl_FragColor.rgb = texture2D(DiffuseSampler, TexCoord).rgb * LightColor;
    gl_FragColor.a = 1.0;
}
