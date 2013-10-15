varying vec2 TexCoord;
uniform sampler2D DiffuseSampler;

void main()
{
    gl_FragColor.rgb = vec3(1.0,1.0,1.0);//texture2D(DiffuseSampler, TexCoord).rgb;
}
