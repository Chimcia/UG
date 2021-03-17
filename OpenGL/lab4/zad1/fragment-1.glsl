#version 330


// Predefiniowane dane wejsciowe
//in  vec4 gl_FragCoord;
//in  int  gl_PrimitiveID;


// Dane wyjsciowe
out vec4 outColor;
in vec3 fragColor;

//float random (vec2 uv)
//    {
//        return sin(dot(uv,vec2(12.9898,78.233)))*43758.5453123
//        - floor(sin(dot(uv,vec2(12.9898,78.233)))*43758.5453123);

 //   };
void main()
{
  //  vec2 uv = gl_FragCoord.xy;
  //  float rnd = random(uv);
  //  gl_FragColor = vec4(vec3(rnd),1.0);
	//if (gl_PrimitiveID%2 == 0)
	//	outColor = vec4(1.0, 0.0, 0.0, 1.0);
	//	outColor.r =(gl_FragCoord.x/600.0f)*2;
	//	outColor.b = (gl_PrimitiveID%2);
	//else
	//	outColor = vec4(0.0, 1.0, 0.0, 1.0);
    outColor = vec4(fragColor, 1.0);
}
