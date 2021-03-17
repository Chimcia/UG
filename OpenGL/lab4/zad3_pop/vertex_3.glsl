#version 330
//BUDYNEK
// Wspolrzedne wierzcholkow
layout( location = 0 ) in vec4 inPosition;

// Macierz rzutowania
uniform mat4 matProjView;


out vec4 inoutPos;

void main()
{
	gl_Position = matProjView * inPosition;
	inoutPos = inPosition + 2.5f;
}
