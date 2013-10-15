varying vec2 TexCoord;
uniform sampler2D DiffuseSampler;

void main()
{
    gl_FragColor = texture2D(DiffuseSampler, TexCoord);
}
