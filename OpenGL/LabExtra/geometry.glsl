#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vertNormal[];

out vec3 fragNormal;

uniform mat4 matProjView;
uniform float Timer;

void main()
{
	vec4 shift = vec4(0.0, 2.0, 0.0, 0.0);

	//oryginalny trojkat
    fragNormal = vertNormal[0];
	gl_Position = matProjView *  gl_in[0].gl_Position;
	EmitVertex();

    fragNormal = vertNormal[1];
	gl_Position = matProjView * gl_in[1].gl_Position;
	EmitVertex();

    fragNormal = vertNormal[2];
	gl_Position = matProjView * gl_in[2].gl_Position;
	EmitVertex();
/*
	fragNormal = vertNormal[2];
	gl_Position = matProjView * (gl_in[2].gl_Position + shift);
	EmitVertex();
*/
	EndPrimitive();
/*
	//teraz drugi trojkat
	for(int i=0; i<3; i++){
        fragNormal = vertNormal[i];
        gl_Position = matProjView * (gl_in[i].gl_Position + vec4(fragNormal/2.0, 0.0));
        EmitVertex();
	}
	EndPrimitive();
*/

}
