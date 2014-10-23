#version 120

varying vec3 Position;

const float CheckerScale = 1.0;
const vec3 Color0 = vec3(1.0, 1.0, 1.0) * 0.4;
const vec3 Color1 = vec3(1.0, 1.0, 1.0) * 0.6;

void main()
{
    vec3  fp = floor(Position*CheckerScale);
    float fs = fp.x + fp.y + fp.z;
    float fm = mod(fs, 2.0);
    if(fm == 0.0)
        gl_FragColor = vec4(Color0, 1.0);
    else
        gl_FragColor = vec4(Color1, 1.0);
}
