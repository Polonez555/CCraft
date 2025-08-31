#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float block;

uniform sampler3D blockAtlas;

void main()
{
    FragColor = texture(blockAtlas,vec3(TexCoord,block));
} 