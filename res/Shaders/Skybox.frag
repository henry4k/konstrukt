varying vec3 TexCoord;
uniform samplerCube Texture;

void main()
{
    gl_FragColor.rgb = textureCube(Texture, -TexCoord).rgb;
}
