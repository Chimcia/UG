#version 150
in vec3 ourNormal;
in vec4 inoutPos;
out vec4 outColor;


void main()
{

	vec3 objectColor = vec3(0.2, 0.1, 0.2);

	//
	vec3 result = ourNormal;

	outColor = vec4(objectColor, 1.0);
}
