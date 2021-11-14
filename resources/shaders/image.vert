#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec3 position;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos + position, 1.0);
    TexCoord = aTexCoord;
}