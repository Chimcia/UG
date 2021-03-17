// References:
// - https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// - Janusz Ganczarski, OpenGL. Podstawy programowania grafiki 3D, Helion.


#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"


enum {
	SCENE = 0,
	SCREEN,
	NUMBER_OF_BUFFERS,
};

enum {
	PROGRAM_SCENE = 0,
	NUMBER_OF_PROGRAMS,
};


// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_BUFFERS];
GLuint vBuffer_normals[NUMBER_OF_BUFFERS];
GLuint vBuffer_uvs[NUMBER_OF_BUFFERS];
GLuint vArray[NUMBER_OF_BUFFERS];


GLuint FrameBufferID;
GLuint DepthBufferID;
GLuint TextureBuffer;


// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;


int Window_Width = 800;
int Window_Height = 600;

// ---------------------------------------
std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_BUFFERS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_BUFFERS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_BUFFERS];
GLuint TextureID;

// Kwadrat wspolrzedne
GLfloat vertices_pos[] =
{
    -1.0f, -1.0f, 0.0f,
	 2.0f, -1.0f, 0.0f,
     2.0f,  2.0f, 0.0f,

     2.0f,  2.0f, 0.0f,
	-1.0f,  2.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
};

// Kwadrat tekstura
GLfloat vertices_tex[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
};
// ---------------------------------------
// Swiatlo kierunkowe
glm::vec3 Light_Direction = glm::normalize(glm::vec3(0.2, -0.8f, 1.1f));
glm::vec3 Light_Position = glm::vec3(0.0f, 5.0f, -25.0f);
glm::vec3 Camera_Position;



// ---------------------------------------
// NOWE: Cienie
// ---------------------------------------
GLuint DepthMap_Program;
GLuint DepthMap_FrameBuffer;
GLuint DepthMap_Texture;
const unsigned int DepthMap_Width = 2024, DepthMap_Height = 2024;

// macierze rzutowania dla kamery patrzacej z punktu widzenia oswietlenia
glm::mat4 lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 2.0f, 30.5f);

glm::mat4 lightView = glm::lookAt(Light_Position,
                                  Light_Position+Light_Direction,
                                  glm::vec3( 0.0f, 1.0f,  0.0f));
void DrawScene_to_texture()
{
	glViewport(0, 0, Window_Width, Window_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );

		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

		// AKTYWUJEMY tekstury
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 0);


		// Scena
		glBindVertexArray( vArray[SCENE] );
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );

glUseProgram(0);
}

void DrawScene_on_screen()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );

		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matProjView" ), 1, GL_FALSE, glm::value_ptr(matModel) );

		// AKTYWUJEMY tekstury
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 0);


		// Scena
		glBindVertexArray( vArray[SCENE] );
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );


		// Ekran
		glBindVertexArray( vArray[SCREEN] );
		glBindTexture(GL_TEXTURE_2D, TextureBuffer);
		glDrawArrays( GL_TRIANGLES, 0, 6 );



    // WYLACZAMY program
    glUseProgram(0);
}

// -------------------------------------------------------
void DrawToShadowMap()
{
	// AKTYWUJEMY program
	glUseProgram( DepthMap_Program );

		glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
		glUniformMatrix4fv( glGetUniformLocation( DepthMap_Program, "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );

		glBindVertexArray( vArray[SCENE] );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );
		glBindVertexArray( 0 );


	// WYLACZAMY program
    glUseProgram(0);

}

// -------------------------------------------------------
void DrawOnScreen()
{
	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	Camera_Position = ExtractCameraPos(matView);


	// Macierz modelu
    matModel =  glm::mat4x4( 1.0 );

	// AKTYWUJEMY program
	glUseProgram( program[PROGRAM_SCENE] );


		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "lightProj" ), 1, GL_FALSE, glm::value_ptr(lightProj) );
		glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "lightView" ), 1, GL_FALSE, glm::value_ptr(lightView) );


		// Swiatlo kierunkowe
		glUniform3fv( glGetUniformLocation( program[PROGRAM_SCENE], "Light_Direction" ), 1, glm::value_ptr(Light_Direction) );
		glUniform3fv( glGetUniformLocation( program[PROGRAM_SCENE], "Camera_Position" ), 1, glm::value_ptr(Camera_Position) );


		// Shadow map textura na slot 1
		glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
		glUniform1i(glGetUniformLocation( program[PROGRAM_SCENE], "shadowMap" ), 1);


		// Scena
		glBindVertexArray( vArray[SCENE] );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SCENE].size() );
		glBindVertexArray( 0 );


    // WYLACZAMY program
    glUseProgram(0);
}

// -------------------------------------------------------
void DisplayScene()
{



	// 1. Renderowanie z pozycji swiatla do textury DepthMap
	glViewport(0, 0, DepthMap_Width, DepthMap_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);


		DrawToShadowMap();


	// 2. Renderowanie z pozycji kamery na ekran
	glViewport(0, 0, Window_Width, Window_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawOnScreen();


	DrawScene_to_texture();
	DrawScene_on_screen();

	glutSwapBuffers();
}

// -------------------------------------------------------
void CreateShadowMap()
{
	// ---------------------------------------
	// NOWE: Shadow mapping
	// ---------------------------------------

	// 1. Stworzenie obiektu tekstury
	glGenTextures(1, &DepthMap_Texture);
	glBindTexture(GL_TEXTURE_2D, DepthMap_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DepthMap_Width, DepthMap_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 2. Stworzenie obiektu ramki bufora jedynie z buforem glebokosci
	// Skladowa koloru nie jest nam potrzebna
	glGenFramebuffers(1, &DepthMap_FrameBuffer);

	// 3. Dolaczanie tekstury do ramki bufora
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMap_FrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMap_Texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. Stworzenie oddzielnego programu, ktory bedzie generowal mape cieni
	DepthMap_Program = glCreateProgram();
	glAttachShader( DepthMap_Program, LoadShader(GL_VERTEX_SHADER, "depthmap.vertex.glsl"));
	glAttachShader( DepthMap_Program, LoadShader(GL_FRAGMENT_SHADER, "depthmap.fragment.glsl"));
	LinkAndValidateProgram( DepthMap_Program );
}

// ---------------------------------------
void Reshape( int width, int height )
{
	Window_Width = width;
	Window_Height = height;

	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{
	_scene_translate_z = -25;
	_scene_translate_y = -5;

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Ladowanie pliku OBJ
	if (!loadOBJ("scene.obj", OBJ_vertices[SCENE], OBJ_uvs[SCENE], OBJ_normals[SCENE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	// Tworzenie potoku OpenGL
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program[PROGRAM_SCENE] );

	// SCENA
	glGenVertexArrays( 1, &vArray[SCENE] );
	glBindVertexArray( vArray[SCENE] );

	glGenBuffers( 1, &vBuffer_pos[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SCENE].size() * sizeof(glm::vec3), &(OBJ_vertices[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_normals[SCENE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normals[SCENE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_normals[SCENE].size() * sizeof(glm::vec3), &(OBJ_normals[SCENE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

    // Inne ustawienia openGL i sceny
	glEnable( GL_DEPTH_TEST );

	// Utworzenie wszystkiego co potrzebne jest do mapy cieni
	CreateShadowMap();

	// EKRAN
	glGenVertexArrays( 1, &vArray[SCREEN] );
	glBindVertexArray( vArray[SCREEN] );

	glGenBuffers( 1, &vBuffer_pos[SCREEN] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SCREEN] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uvs[SCREEN] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uvs[SCREEN] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

    // 1. Stworzenie tekstury do ktorej odbedzie sie rendering

    glGenTextures(1, &TextureBuffer);
    glBindTexture(GL_TEXTURE_2D, TextureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window_Width, Window_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // --------------------------------------------------
    // 2. Stworzenie obiektu bufora ramki, ktory zawiera bufor koloru

    glGenFramebuffers(1, &FrameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferID);

    // Polaczenie tekstury do skladowej koloru bufora ramki
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureBuffer, 0);

    // --------------------------------------------------
    // 3. Stworzenie obiektu render-buffer dla testu glebokosci

    glGenRenderbuffers(1, &DepthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window_Width, Window_Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Polaczenie bufora glebokosci z aktualnym obiektem bufora ramki
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBufferID);

    // --------------------------------------------------
    // 4. Sprawdzenie czy pomyslnie zostal utworzony obiekt bufora ramki
    // i powrot do domyslnego bufora ramki

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: Framebuffer is not complete!\n");
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Inne ustawienia openGL i sceny
    glEnable( GL_DEPTH_TEST );

}



// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( Window_Width, Window_Height );
	glutCreateWindow( "OpenGL" );

	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_2 )
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}


	Initialize();
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );

	glutMainLoop();


	return 0;
}
