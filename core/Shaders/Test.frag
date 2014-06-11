#version 120

uniform sampler2D DiffuseSampler;

varying vec2 TexCoord;

void main()
{
    vec4 diffuse = texture2D(DiffuseSampler, TexCoord);
    gl_FragColor = diffuse;
}
