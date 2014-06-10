#version 120

uniform float Debug;

void main()
{
    gl_FragColor = vec4(Debug, Debug, Debug, 1.0);
}
