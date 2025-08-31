#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float blockType;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 TexCoord;
out float block;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    block = blockType;
    TexCoord = aTexCoord;
}