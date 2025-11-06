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
        float alpha = (sampled.a == 0.0) ? 1.0 : sampled.a;
        FragColor = vec4(sampled.rgb * objectColor, alpha);
    } else {
        FragColor = sampled;
    }
}