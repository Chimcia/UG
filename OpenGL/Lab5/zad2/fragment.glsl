#version 150

in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

out vec4 outColor;

// Sampler tekstury
uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{

	// kolor fragmentu bedzie polaczeniem koloru z tekstury i
	// koloru z atrybutow wierzcholkow

	//vec4 col0 = texture(tex0, inoutUV);
	//vec4 col1 = texture(tex1, inoutUV);
	//outColor = texture(tex0, inoutUV) * vec4(inoutColor, 1.0);
	//³adne ale basic, zwyk³y mix 2 obrazów
	vec2 newUV = vec2(inoutUV.s, 1.0 - inoutUV.t);

	outColor = texture( tex0, newUV );// + vec4(inoutColor, 1.0) ;
    vec4 transColor = texture(tex1, newUV);
    if(transColor.a<0.1)
        discard;
    outColor = transColor;

	//outColor = mix(texture(tex0, inoutUV), texture(tex1, inoutUV), 1);

}
