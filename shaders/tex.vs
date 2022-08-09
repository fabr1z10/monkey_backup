R"(
#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexture;
layout (location = 2) in vec4 vColor;

out vec2 tex;
out vec4 col;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
	tex = vTexture;
	col = vColor;
	gl_Position = projection * modelview * vec4(vPosition, 1.0);
}
)"
