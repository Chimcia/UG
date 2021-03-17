#version 150 core

in vec3 fragNormal;

out vec4 outColor;

void main()
{

	//vec3 myColor = sin(gl_FragCoord/100.0).xyz;
    vec3 newColor = vec3(1.0, 0.0, 0.0);
	//outColor = vec4(fragNormal, 1.0);
	outColor = vec4(newColor, 1.0);
}
