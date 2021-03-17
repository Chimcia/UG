#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stuff.h"

glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matProjView;


// Identyfikatory obiektow
GLuint idProgram;
GLuint idGrassProgram;
GLuint idVAO;

// Listy atrybutow z pliku obj
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;

float Timer = 0.0f;
// ---------------------------------------
void DisplayScene()
{
    if (Timer > 20.0)
        Timer = 0.0f;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Macierze rzutowania
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	matProjView = matProj * matView;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram( idProgram );


		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));
		glUniform1f(glGetUniformLocation(idProgram, "Timer"), Timer);

		// Generowanie obiektow na ekranie
		glBindVertexArray( idVAO );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
		glBindVertexArray( 0 );
    glUseProgram(0);
    //proba zrobienia strzalek v1.0
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram( idGrassProgram );


		glUniformMatrix4fv( glGetUniformLocation(idGrassProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));
		glUniform1f(glGetUniformLocation(idGrassProgram, "Timer"), Timer);

		// Generowanie obiektow na ekranie
		glBindVertexArray( idVAO );
		glDrawArrays( GL_POINTS, 0, OBJ_uvs.size() );
		glBindVertexArray( 0 );


	// Wylaczanie
	glUseProgram( 0 );

	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{

	// OpenGL
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glEnable(GL_DEPTH_TEST);



	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
	{
		printf("Not loaded!\n");
		exit(1);
	}


	// 1. Program i shadery
	idProgram = glCreateProgram();

	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	// NOWE: wczytujemy plik shadera geometrii
	glAttachShader( idProgram, LoadShader(GL_GEOMETRY_SHADER, "geometry.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	//program do robienia trawy/strzalek
	idGrassProgram = glCreateProgram();

	glAttachShader( idGrassProgram, LoadShader(GL_VERTEX_SHADER, "vertex2.glsl"));
	// NOWE: wczytujemy plik shadera geometrii
	glAttachShader( idGrassProgram, LoadShader(GL_GEOMETRY_SHADER, "geometry2.glsl"));
	glAttachShader( idGrassProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment2.glsl"));
	LinkAndValidateProgram( idGrassProgram );


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	GLuint idVBO_coord;
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	GLuint idVBO_normals;
	glGenBuffers( 1, &idVBO_normals );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_normals );
	glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	matProj = glm::perspective(glm::radians(80.0f), width/(float)height, 0.1f, 20.0f);
}

void Animation(int frame){
    Timer += 0.001f;
    glutTimerFunc(10, Animation, 0);
    glutPostRedisplay();
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Programownie grafiki w OpenGL" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );
	glutTimerFunc(1, Animation, 0);

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
	glutMainLoop();

	return 0;
}
