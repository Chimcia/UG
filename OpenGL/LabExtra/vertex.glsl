#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec3 vertNormal;

void main()
{
	gl_Position = inPosition;
	vertNormal = inNormal;
}
