#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform bool useColor;
uniform vec3 objectColor;

void main()
{    
    vec4 sampled = texture(texture_diffuse1, TexCoords);
    if (useColor) {
        // multiply sampled alpha by objectColor; if white texture is used this yields solid color
        FragColor = vec4(objectColor, sampled.a);
    } else {
        FragColor = sampled;
    }
}