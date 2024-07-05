#version 330 core
out vec4 FragColor;
in float ourImg;
in vec2 TexCoord;
// texture samplers
uniform sampler2D[2] texture3;
void main()
{
    FragColor = texture(texture3[int(ourImg)], TexCoord);
};
