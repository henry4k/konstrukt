varying vec2 TexCoord;
uniform sampler2D DiffuseSampler;

void main()
{
    //gl_FragColor = texture2D(DiffuseSampler, TexCoord);
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
