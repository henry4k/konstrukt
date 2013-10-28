varying vec3 TexCoord;
uniform samplerCube Texture;

void main()
{
    //gl_FragColor = textureCube(Texture, TexCoord);
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
