#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;
out vec4 outPlayerColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform int selected = 0;
uniform int player1 = 0;
uniform float u_time;


vec3 colorA = vec3(0.149,0.141,0.912);
vec3 colorB = vec3(1.000,0.833,0.224);
vec3 colorC = vec3(0.149,0.141,0.912);


void main()
{
	outColor = texture( tex0, inoutUV );
	vec3 color = vec3(0.0);
	vec3 colorP = vec3(0.0);
    if (selected == 1)
    {
		float pct = abs(sin(u_time));
		color = mix(colorA, colorB, pct);
        outColor = texture( tex0, inoutUV ) * vec4(color, 1.0);
    }
    if (player1 == 1)
    {
		float pct = abs(sin(u_time));
		colorP = mix(colorA, colorB, pct);
        outPlayerColor = texture( tex0, inoutUV ) * vec4(colorP, 1.0);
    }
}
