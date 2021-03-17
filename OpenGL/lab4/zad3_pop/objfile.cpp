#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.hpp"
#include "obj_loader.hpp"
#include "object3d.hpp"


glm::mat4 matProj;
glm::mat4 matView;
glm::mat4 matProjView;


// Identyfikatory obiektow

GLuint id2;
GLuint id3;
GLuint id4;
GLuint id5;
GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora na wspolrzedne
GLuint idVBO_color; // bufora na kolory
GLuint TextureID;

// -----------------------------------
// NOWE: wektory na dane z pliku OBJ
// -----------------------------------
std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;
std::vector<glm::vec4> Material;


CObject Scene, Human, Building, Koliber, Sphere;

typedef struct SMaterial
{
	glm::vec4 Ambient;
} SMaterial;


SMaterial Material1 = {
	glm::vec4(0.7f, 0.8f, 0.2f, 1.0f)
};

SMaterial Material2 = {
	glm::vec4(0.2f, -0.5f, 0.6f, 1.0f)
};

// ---------------------------------------
void DisplayScene()
{

	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );


	// Obliczanie macierzy rzutowania
	matProjView = matProj * matView;

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO );
	glUseProgram( idProgram );

		// Przekazanie macierzy rzutowania
		glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));
		glUniformMatrix4fv( glGetUniformLocation(id2, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));
        glUniformMatrix4fv( glGetUniformLocation(id3, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));


		// Generowanie obiektow na ekranie
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );


        //skorzystam z pliku object3d.hpp zamiast obj_loader'a
		//OBIEKT1 - PLANSZA
        glm::mat4 Matrix_model = glm::mat4(1.0);
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
        matProjView = matProj * matView * Matrix_model;
        glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );
        glUniform1i(glGetUniformLocation(idProgram, "tex0"), 0);

        Scene.Draw();

    //glUseProgram( 0 );
        //OBIEKT2 - HUMAN
        glUseProgram( id2 );

        glAttachShader(id2, LoadShader(GL_FRAGMENT_SHADER, "fragment_2.glsl"));
        glAttachShader(id2, LoadShader(GL_VERTEX_SHADER, "vertex_2.glsl"));
        glUniform4fv( glGetUniformLocation( id2, "myMaterial.Ambient"), 1, &Material1.Ambient[0]);
        Matrix_model = glm::translate(glm::mat4(1.0), glm::vec3(1.3, 0.1, 2.5));
        matProjView = matProj * matView * Matrix_model;
        glUniformMatrix4fv( glGetUniformLocation( id2, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );
        Human.Draw();


        //OBIEKT3 - BUILDING
        glUseProgram( id3 );
        //glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment_2.glsl"));
        glAttachShader(id3, LoadShader(GL_FRAGMENT_SHADER, "fragment_3.glsl"));
        glAttachShader(id3, LoadShader(GL_VERTEX_SHADER, "vertex_3.glsl"));
        glUniform4fv( glGetUniformLocation( id3, "myMaterial.Ambient"), 1, &Material2.Ambient[0]);
        Matrix_model = glm::translate(glm::mat4(1.0), glm::vec3(5.0, 0.1, -2.0));//miejsce pokazania sie na mapie
        matProjView = matProj * matView * Matrix_model;//must have
        glUniformMatrix4fv( glGetUniformLocation( id3, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );
        Building.Draw();

        //OBIEKT4 - TEST
        glUseProgram( idProgram );
        Matrix_model = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.1, 6.5));
        matProjView = matProj * matView * Matrix_model;
        glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );
        Koliber.Draw();

        //OBIEKT5 - SPHERE
        glBindVertexArray(idVAO);
        glUseProgram(id4);
      glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id4);
        Matrix_model = glm::translate(glm::mat4(1.0), glm::vec3(4.0, 0.1, 10.5));
        matProjView = matProj * matView * Matrix_model;
        glUniformMatrix4fv( glGetUniformLocation(id4, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));
        glUniform1i(glGetUniformLocation(id4, "tex5"), 0);

        glDrawArrays(GL_TRIANGLES, 0, OBJ_vertices.size());
        Sphere.Draw();


	// Wylaczanie
	glUseProgram( 0 );
	//glBindVertexArray( 0 );


	glutSwapBuffers();

}

// ---------------------------------------
void Initialize()
{

	// -----------------------------------
	// NOWE: wczytanie pliku obj
	// -----------------------------------

//	if (!loadOBJ("scene.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
//	{
//		printf("Not loaded!\n");
	//	exit(1);
//	}

	// ustawienie poczatkowej translacji na z
	_scene_translate_z = -10.0f;




	// 1. Program i shadery
	idProgram = glCreateProgram();//plansza
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	id2 = glCreateProgram();//human
    glAttachShader(id2, LoadShader(GL_VERTEX_SHADER, "vertex_2.glsl"));
    glAttachShader(id2, LoadShader(GL_FRAGMENT_SHADER, "fragment_2.glsl"));
    LinkAndValidateProgram(id2);

    id3 = glCreateProgram();//building
    glAttachShader(id3, LoadShader(GL_VERTEX_SHADER, "vertex_3.glsl"));
    glAttachShader(id3, LoadShader(GL_FRAGMENT_SHADER, "fragment_3.glsl"));
    LinkAndValidateProgram(id3);

    id4 = glCreateProgram();//sphere
    glAttachShader(id4, LoadShader(GL_VERTEX_SHADER, "vertex_4.glsl"));
    glAttachShader(id4, LoadShader(GL_FRAGMENT_SHADER, "fragment_4.glsl"));
    LinkAndValidateProgram(id4);

    id5 = glCreateProgram();//koliber
    glAttachShader(id5, LoadShader(GL_VERTEX_SHADER, "vertex_5.glsl"));
    glAttachShader(id5, LoadShader(GL_FRAGMENT_SHADER, "fragment_5.glsl"));
    LinkAndValidateProgram(id5);

    glEnable(GL_DEPTH_TEST);
    glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );

	Scene.Load("scene.obj");
	Human.Load("Human.obj");
	Building.Load("buildings.obj");
	Koliber.Load("koliber.obj");
	Sphere.Load("sphere.obj");

	// 2. Vertex arrays
	//glGenVertexArrays( 1, &idVAO );
	//glBindVertexArray( idVAO );

	// Bufor na wspolrzedne wierzcholkow
	//glGenBuffers( 1, &idVBO_coord );

//	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
//	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );

	//glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
//	glEnableVertexAttribArray( 0 );



//	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );

	matProj = glm::ortho(-1.0, 1.0, -1.0, 1.0);
	matProj = glm::perspective(glm::radians(80.0f), width/(float)height, 0.1f, 100.0f);

}




// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "Programownie grafiki w OpenGL" );

	// -----------------------------
	// NOWE: Handlery
	// -----------------------------

	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );


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


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteProgram( id2 );
	glDeleteProgram( id3 );
	glDeleteProgram( id4 );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
