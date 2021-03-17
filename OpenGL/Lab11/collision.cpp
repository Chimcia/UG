#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <experimental/random>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"
#include "ground.h"
#include "text-ft.h"

#include <ft2build.h>
#include FT_FREETYPE_H


glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel(1.0);

#define NUMBER_OF_OBJECTS 4

enum {
    GROUND = 0,
    SPHERE = 1,
    THING = 2,
};

// ---------------------------------------

GLuint program;
GLuint vBuffer_pos[NUMBER_OF_OBJECTS];
GLuint vBuffer_uv[NUMBER_OF_OBJECTS];
GLuint vArray[NUMBER_OF_OBJECTS];

std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_OBJECTS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_OBJECTS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_OBJECTS];


GLuint TextureID[NUMBER_OF_OBJECTS];
int count  = 0;
int frames = 0;
static float initial_time = GetTickCount() * 0.001f, final_time;



// ----------------------------------------------------------------
class CSceneObject
{

public:

	// pozycja obiektu na scenie
    glm::vec3 Position;
    glm::mat4x4 matModel;

	// potok openGL
    GLuint VAO;
    int VBO_Size;
    int visible = 1;
    int collectable = 0;

	// NOWE: srednica sfery otaczajacej obiekt
	float radius = 1.0f;

	// Podlaczenie do CGround
	CGround *Ground = NULL;

    CSceneObject()
    {
    }


    // ustawienie potoku
    void Init(GLuint _vao, int _size, CGround *_ground)
    {
        VAO = _vao;
        VBO_Size = _size;
        Ground = _ground;
    }

    	// Obliczenie wysokosci nad ziemia
	void UpdateLatitute()   {
		float newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z));

		Position.y = newAlt;
		matModel = glm::translate(glm::mat4(1.0), Position);
	}


    // rysowanie na scenie
    void Draw()
    {
		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

        glUseProgram(current_program);
        glUniformMatrix4fv( glGetUniformLocation( current_program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z)
    {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    // zmiana pozycji na scenie
    void MoveXZ(float _x, float _z)
    {
        Position += glm::vec3(_x, 0.0, _z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    // zmiana pozycji na scenie gdy nie zachodzi kolizja z obiektem other
    void MoveXZ(float _x, float _z,const CSceneObject &_other)
    {
        glm::vec3 OldPosition = Position;

        //printf("%d", _x);
        //float _y = 0.2f;
        Position += glm::vec3(_x, 0.0, _z);
        if(this->isCollision(_other)) {
        Position = OldPosition;
        }
        matModel = glm::translate(glm::mat4(1.0), Position);

        // Aktualizacja polozenia na Y
        UpdateLatitute();
    }



	// NOWE: sprawdzanie kolizji z innym obiektem
    bool isCollision(const CSceneObject &_other)
    {
        float distance = glm::distance(this->Position, _other.Position);
        if (distance < this->radius + _other.radius)
        {
            return true;
        }
        return false;
    }


};

//-------------------------------------------
class CSceneCollect {

public:

	// pozycja obiektu na scenie
    glm::vec3 Position;
    glm::mat4x4 matModel;

	// potok openGL
    GLuint VAO;
    int VBO_Size;
    int visible = 1;
    int collectable = 0;

	// NOWE: srednica sfery otaczajacej obiekt
	float radius = 1.0f;

	// Podlaczenie do CGround
	CGround *Ground = NULL;

    CSceneCollect()
    {
    }


    // ustawienie potoku
    void Init(GLuint _vao, int _size, CGround *_ground)
    {
        VAO = _vao;
        VBO_Size = _size;
        Ground = _ground;
    }

    // zmienienie widzialnosci przedmiotu
    void UpdateVisibility(const CSceneObject chPos) {
        if(this->isCollision(chPos)){
            if(this->visible){
                this->visible=0;
                count += 1;
            }
        }
        else{

        }
    }

    	// Obliczenie wysokosci nad ziemia
	void UpdateLatitute()   {
		float newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z));

		Position.y = newAlt;
		matModel = glm::translate(glm::mat4(1.0), Position);
	}


    // rysowanie na scenie
    void Draw()
    {
        //UpdateLatitute();
		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

        glUseProgram(current_program);
        glUniformMatrix4fv( glGetUniformLocation( current_program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z)
    {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

	// NOWE: sprawdzanie kolizji z innym obiektem
    bool isCollision(const CSceneObject &_other)
    {
        float distance = glm::distance(this->Position, _other.Position);
        if (distance < this->radius + _other.radius)
        {
            return true;
        }
        return false;
    }


};


// Obiekty na scenie
CSceneObject Stone;
CSceneObject myCharacter;
CSceneCollect Collect1;
CSceneCollect Collect2;
CSceneCollect Collect3;
CGround myGround;


// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Geometria sceny
	matView = glm::translate( glm::mat4( 1.0 ), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	matModel = glm::mat4x4( 1.0 );


    glUseProgram( program );

		// Wyslanie macierzy rzutowania
		glUniformMatrix4fv( glGetUniformLocation( program, "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );



		// GROUND
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID[GROUND]);
		glUniform1i(glGetUniformLocation(program, "tex0"), 0);

		glBindVertexArray( vArray[GROUND] );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[GROUND].size() );
		glBindVertexArray( 0 );


		// SPHERE
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);
		glUniform1i(glGetUniformLocation(program, "tex0"), 0);

		// Rendering obiektu statycznego
		Stone.Draw();

		// Rendering obiektu znikaj�cego
		if(Collect1.visible){
            Collect1.Draw();
		}

		// Rendering postaci, ktora sie poruszamy
		myCharacter.Draw();


    glUseProgram( 0 );

    char txt[255];
    sprintf(txt,"Collected trees???: %d", count);
    RenderText(txt, 20, 20, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    int FPS;
	//char txt[255];
    frames++;
	final_time = GetTickCount() * 0.001f;
	if(final_time - initial_time > 0.0f) {
       // SPRAWDZENIE W KONSOLI CZY DZIALA FPS
       //std::cout<<"FPS: "<<frames/(final_time - initial_time) <<std::endl;
        printf("%d", FPS);
        FPS = (frames/(final_time - initial_time));
        //FPS = 1000.0/double(frames);
        sprintf(txt, "FPS: %d", FPS);

        RenderText(txt, 20, 470, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

        frames = 0;
        initial_time = final_time;
    }


	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height ) {
	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// --------------------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    float move_vec = 0.2;

    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

        case 'w':
            myCharacter.MoveXZ(move_vec, 0.0f, Stone);
            Collect1.UpdateVisibility(myCharacter);
            break;
         case 's':
            myCharacter.MoveXZ(-move_vec, 0.0f, Stone);
            Collect1.UpdateVisibility(myCharacter);
            break;

         case 'd':
            myCharacter.MoveXZ(0.0f, move_vec, Stone);
            Collect1.UpdateVisibility(myCharacter);
            break;
         case 'a':
            myCharacter.MoveXZ(0.0f, -move_vec, Stone);
            Collect1.UpdateVisibility(myCharacter);
            break;

    }

    glutPostRedisplay();
}

// ---------------------------------------------------
void Initialize() {

    InitText("arial.ttf", 36);


	// tekstury
	int tex_width;
	int tex_height;
	unsigned char *tex_data;



	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -8;
	_scene_translate_y = -3;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );


	// Programowanie potoku
	program = glCreateProgram();

	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( program );




	// GROUND
	if (!loadOBJ("scene-plane.obj", OBJ_vertices[GROUND], OBJ_uvs[GROUND], OBJ_normals[GROUND]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[GROUND] );
	glBindVertexArray( vArray[GROUND] );

	glGenBuffers( 1, &vBuffer_pos[GROUND] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[GROUND] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[GROUND].size() * sizeof(glm::vec3), &(OBJ_vertices[GROUND])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[GROUND] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[GROUND] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[GROUND].size() * sizeof(glm::vec2), &(OBJ_uvs[GROUND])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );


	loadBMP_custom("chess.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[GROUND]);
	glBindTexture(GL_TEXTURE_2D, TextureID[GROUND]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



    // SPHERE
    if (!loadOBJ("sphere.obj", OBJ_vertices[SPHERE], OBJ_uvs[SPHERE], OBJ_normals[SPHERE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	// Vertex arrays
	glGenVertexArrays( 1, &vArray[SPHERE] );
	glBindVertexArray( vArray[SPHERE] );

	glGenBuffers( 1, &vBuffer_pos[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SPHERE].size() * sizeof(glm::vec3), &(OBJ_vertices[SPHERE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[SPHERE].size() * sizeof(glm::vec2), &(OBJ_uvs[SPHERE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );

    loadBMP_custom("grass.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[SPHERE]);
	glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// TREE
	if (!loadOBJ("tree.obj", OBJ_vertices[THING], OBJ_uvs[THING], OBJ_normals[THING]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[THING] );
	glBindVertexArray( vArray[THING] );

	glGenBuffers( 1, &vBuffer_pos[THING] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[THING] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[THING].size() * sizeof(glm::vec3), &(OBJ_vertices[THING])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[THING] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[THING] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[THING].size() * sizeof(glm::vec2), &(OBJ_uvs[THING])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );


	loadBMP_custom("bubbles.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[THING]);
	glBindTexture(GL_TEXTURE_2D, TextureID[THING]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Inicjalizacja obiektow

    //plansza
    myGround.CreateFromOBJ(OBJ_vertices[GROUND]);//wczytuje te "trojkaty"

    //przeszkoda
    Stone.Init(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &myGround );
    //Stone.SetPosition(5, 0, -5);
    int r1 = (rand() % 20) + 2;
    int r2 = std::experimental::randint(0,60);
    int r3 = rand() % 21;
    Stone.SetPosition(0, 0, -5);

    //znajd�ka 1
    Collect1.Init(vArray[THING], OBJ_vertices[THING].size(), &myGround);
    Collect1.SetPosition(10, 0, -2);


    //postac
    myCharacter.Init(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &myGround );
    myCharacter.SetPosition(0, 0, 0);

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
	glutInitWindowSize( 500, 500 );
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
