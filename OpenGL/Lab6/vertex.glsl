#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;
layout( location = 2 ) in vec2 inUV;



uniform mat4 matProjView;

out vec4 inoutPos;
out vec2 inoutUV;

out vec3 myPosition;
out vec3 myNormal;

void main()
{

	gl_Position = matProjView * inPosition;

	myPosition = vec3(inPosition);
	myNormal = vec3(inNormal);

	inoutPos = inPosition + 2.5f;
	inoutUV = inUV;

}

