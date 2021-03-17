#version 150 core

in vec4 inoutPos;
in vec2 inoutUV;
in vec3 inoutNormal;

out vec4 outColor;
uniform vec3 Camera_Position;
uniform sampler2D tex0;
uniform samplerCube tex_cube;

//REFRAKCJA
//uniform samplerCube SkyBox_Texture;

//float Refraction_Coeff = 1.0/1.2;

//vec3 Dir = normalize(fragPos - cameraPos);
//vec3 Refraction = refract(Dir, fragNormal, Refraction_Coeff);
//vec3 RefractionColor = texture(SkyBox_Texture, Refraction).rgb;

void main()
{
    //ODBICIE
    vec3 Dir = normalize(inoutPos.xyz - Camera_Position);
    vec3 Reflection = reflect(Dir, inoutNormal);
    vec4 ReflectionColor = texture(tex_cube, Reflection);

    outColor = ReflectionColor;
    //outColor = BaseColor/3.0 + 2.0*ReflectionColor/4.0 + 2.0*RefractionColor/4.0;
	//outColor = texture(tex0, inoutUV);
}
