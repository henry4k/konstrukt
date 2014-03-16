#version 120

varying vec2 TexCoord;

uniform float AlphaThreshold;
uniform sampler2D DiffuseSampler;

void main()
{
    float alpha = texture2D(DiffuseSampler, TexCoord).a;
    if(alpha > AlphaThreshold)
        discard;
    gl_FragDepth = gl_FragCoord.z;
}
