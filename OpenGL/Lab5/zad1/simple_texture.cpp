#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.hpp"

// NOWE: funkcje do ladowania plikow BMP
#include "texture_loader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// ---------------------------------------

GLuint program;
GLuint vBuffer_pos;
GLuint vBuffer_color;
GLuint vBuffer_uv;
GLuint vArray;

// NOWE: identyfikator nowej tekstury

GLuint TextureID[2];


// ---------------------------------------

glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjView;

// ---------------------------------------

GLfloat vertices_pos[] =

{
	-1.0f, -1.0f, 0.0f,	// bottom left
	 1.0f, -1.0f, 0.0f,	// bottom right
	 0.0f,  1.0f, 0.0f, // upper
};

GLfloat vertices_color[] =
{
   // -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
    // 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
    // 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
   // -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
};

GLfloat vertices_tex[] =

{
	0.0f, 2.0f, // for bottom left vertex
	2.0f, 0.0f, // for bottom right vertex
	0.5f, -1.0f, // for upper vertex
};

// ---------------------------------------

void DisplayScene()

{
	// Czyszczenie ramki (jak zwykle)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Geometria sceny  (jak zwykle)

	matView = glm::mat4x4(1.0);
	matView = glm::translate(matView, glm::vec3(_scene_translate_x, _scene_translate_y, _scene_translate_z));
	matView = glm::rotate(matView, _scene_rotate_x, glm::vec3(1.0f, 0.0f, 0.0f));
	matView = glm::rotate(matView, _scene_rotate_y, glm::vec3(0.0f, 1.0f, 0.0f));

	matProjView = matProj * matView;

	glBindVertexArray(vArray);

	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "matProjView"), 1, GL_FALSE, &(matProjView[0][0]));

	// NOWE: Przekazanie identyfikatora samplera

	glUniform1i(glGetUniformLocation(program, "tex0"), 0);
	glUniform1i(glGetUniformLocation(program, "tex1"), 1);





	// Generowanie obiektow na ekranie (jak zwykle)

	glDrawArrays(GL_TRIANGLES, 0, 3 * 3);
	glUseProgram(0);
	glBindVertexArray(0);
	glutSwapBuffers();
}
// ---------------------------------------------------

void CreateTexture(const char* name, int id_table)

{
	//unsigned char* tex_data;

	//loadBMP_custom(name, tex_width, tex_height, &tex_data);
    int tex_width,tex_height,n;
    unsigned char *tex_data = stbi_load(name, &tex_width, &tex_height, &n, 0);
	//tex_data = SOIL_load_image(name, &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, TextureID[id_table]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glBindTexture(GL_TEXTURE_2D, TextureID[id_table]);
}

// ---------------------------------------------------

void Initialize()

{
	glGenTextures(2, TextureID);
	glActiveTexture(GL_TEXTURE0);
	CreateTexture("uvtemplate.bmp", 0);
	glActiveTexture(GL_TEXTURE1);
	CreateTexture("bubbles.bmp", 1);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Programowanie potoku

	program = glCreateProgram();
	glAttachShader(program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram(program);

	// Vertex arrays

	glGenVertexArrays(1, &vArray);
	glBindVertexArray(vArray);

	// Wspolrzedne wierzchokow

	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Kolor wierzcholkow

	glGenBuffers(1, &vBuffer_color);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_color), vertices_color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// Wspolrzedne textury UV

	glGenBuffers(1, &vBuffer_uv);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);
}

// ---------------------------------------

void Reshape(int width, int height)

{
	glViewport(0, 0, width, height);
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f);
}



// ---------------------------------------------------

int main(int argc, char* argv[])

{

	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL");
	// GLEW

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("GLEW Error\n");
		exit(1);
	}
	// OpenGL
	if (!GLEW_VERSION_3_2)
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}
	Initialize();
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutMainLoop();


	// Cleaning();

	glDeleteProgram(program);
	glDeleteBuffers(1, &vBuffer_pos);
	glDeleteBuffers(1, &vBuffer_uv);
	glDeleteBuffers(1, &vBuffer_color);
	glDeleteVertexArrays(1, &vArray);
	return 0;

}
