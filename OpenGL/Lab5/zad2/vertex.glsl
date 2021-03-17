#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProjView;
uniform float push;

out vec4 inoutPos;
out vec3 inoutColor;
out vec2 inoutUV;

uniform float horizontal_shift;

void main()
{
	gl_Position = matProjView * inPosition;

	inoutPos = inPosition;
	inoutColor = inColor;
    //inoutUV = inUV;
	inoutUV = vec2(inUV.x + push, inUV.y);
}
