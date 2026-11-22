#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 Normal;
out vec3 color;

uniform mat4 camMatrix;
uniform mat4 model;        
uniform mat4 translation;  
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	mat4 world = translation * rotation * scale * model;
	Normal = mat3(rotation * model) * aNormal;
	color = aColor;
	gl_Position = camMatrix * world * vec4(aPos, 1.0);
}