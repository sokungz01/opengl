#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 ourColor;
uniform bool isDynamicColor;
uniform bool isUpside;
uniform bool isRotate;
uniform float angle;
uniform vec4 ourUniformColor;

void main()
{

    vec3 pos = aPos;
    if (isUpside) {
        pos.y = -pos.y;
    }
    if (isRotate) {
        float s = sin(angle);
        float c = cos(angle);
        float x = pos.x * c - pos.y * s;
        float y = pos.x * s + pos.y * c;
        pos.x = x;
        pos.y = y;
    }
    gl_Position = vec4(pos, 1.0);

    if (isDynamicColor){
        ourColor = ourUniformColor;
    }else{
        ourColor = vec4(aColor, 1.0);
    }
}