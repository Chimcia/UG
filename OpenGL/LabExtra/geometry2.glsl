#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vertNormal[];

out vec3 fragNormal;

uniform mat4 matProjView;
uniform float Timer;

void main()
{

	float tempf = 0.2f;
    fragNormal = vertNormal[0];
    vec3 temp1 = vec3(fragNormal.x * 1.2, fragNormal.y * 1.2, fragNormal.z * 1.2);
    vec3 temp2 = vec3(fragNormal.x + tempf, fragNormal.y + tempf, fragNormal.z + tempf);
    vec3 temp3 = vec3(fragNormal.x - tempf , fragNormal.y - tempf, fragNormal.z - tempf);

	gl_Position = matProjView * (gl_in[0].gl_Position);
	EmitVertex();

	gl_Position = matProjView * (gl_in[0].gl_Position + vec4(temp1/2.0, 0.0));
	EmitVertex();

	EndPrimitive();


    gl_Position = matProjView * (gl_in[0].gl_Position + vec4(temp1/2.0, 0.0));
	EmitVertex();

	gl_Position = matProjView * (gl_in[0].gl_Position + vec4(temp2/2.0, 0.0));
	EmitVertex();

	EndPrimitive();


    gl_Position = matProjView * (gl_in[0].gl_Position + vec4(temp1/2.0, 0.0));
	EmitVertex();

	gl_Position = matProjView * (gl_in[0].gl_Position + vec4(temp3/2.0, 0.0));
	EmitVertex();

	EndPrimitive();

}
