#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
out vec4 outColor;

uniform vec3 ourColor;
uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(aPos, 1.0);
	outColor = vec4(ourColor, 1.0);
}