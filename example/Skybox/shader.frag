#version 120

varying vec3 TexCoord;
uniform samplerCube Texture;

void main()
{
    vec3 textureColor = textureCube(Texture, TexCoord).rgb;
    gl_FragColor = vec4(textureColor, 1.0);
}
