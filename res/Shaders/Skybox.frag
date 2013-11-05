varying vec3 TexCoord;
uniform samplerCube Texture;

void main()
{
    gl_FragColor = textureCube(Texture, TexCoord);
}
