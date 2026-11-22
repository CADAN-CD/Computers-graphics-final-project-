#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	texCoords = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
	//the position is set to the far plane, so that the skybox is always rendered behind all other objects
	gl_Position = pos.xyww;   
}