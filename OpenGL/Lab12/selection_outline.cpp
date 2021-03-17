#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <iterator>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Window_width = 800;
int Window_height = 600;

glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjView;
glm::mat4x4 matModel(1.0);

#include "stuff.h"

#define NUMBER_OF_MESHES 2

enum {
    GROUND = 0,
    SPHERE = 1,
};


// ---------------------------------------
GLuint program;
GLuint vBuffer_pos[NUMBER_OF_MESHES];
GLuint vBuffer_uv[NUMBER_OF_MESHES];
GLuint vArray[NUMBER_OF_MESHES];

std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_MESHES];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_MESHES];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_MESHES];
GLuint TextureID[NUMBER_OF_MESHES];

// ---------------------------------------
class CSceneObject
{

public:

    glm::vec3 Position;
    float radius;


    GLuint VAO;
    int VBO_Size;

    glm::mat4x4 matModel;

    int CollisionID = 0;

    CSceneObject()
    {
        Position.x = rand()%50 - 25;
        Position.y = 0;
        Position.z = rand()%50 - 25;
        radius = 1.1;

        matModel = glm::translate(glm::mat4(1.0), Position);

    }

    /*
    CSceneObject(const CSceneObject &o2) {
        Position = o2.Position;
        radius = o2.radius;
        VAO = o2.VAO;
        VBO_Size = o2.VBO_Size;
        matModel = o2.matModel;
        CollisionID = o2.CollisionID;
    }
    *///nieudana proba

    void Set(GLuint _vao, int _size)
    {
        VAO = _vao;
        VBO_Size = _size;
    }

    void Draw(GLuint prog)
    {


        glUniformMatrix4fv( glGetUniformLocation( prog, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

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
    void MoveXZ(float _x, float _z, const CSceneObject &_other)
    {
        glm::vec3 OldPosition = Position;

        //printf("%d", _x);
        //float _y = 0.2f;
        Position += glm::vec3(_x, 0.0, _z);
        if(this->isCollision(_other)) {
            Position = OldPosition;
        }
        matModel = glm::translate(glm::mat4(1.0), Position);

    }

    // sprawdzanie kolizji z innym obiektem
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

int NUMBER_OF_OBJECTS = 50;
CSceneObject* Table_of_Objects = new CSceneObject[NUMBER_OF_OBJECTS];

std::vector < CSceneObject > vectors_of_objects;

int CollisionID = 2;
//int CollisionIDArray[NUMBER_OF_OBJECTS] = { };//wypelnienie tablicy zerami.

/*nieudana proba

//zwiekszenie ilosci obiektow
void ExtendTable(){
    int lengthOfTable = sizeof(Table_of_Objects)/sizeof(Table_of_Objects[0]);
    //CSceneObject newObject = new CSceneObject();
    CSceneObject* Extended_Table;
    Extended_Table = new CSceneObject[lengthOfTable + 1];
    for(int i = 0; i != lengthOfTable; i++){
        Extended_Table[i] = Table_of_Objects[i];
    }
    //Extended_Table[lengthOfTable] = newObject;
    Extended_Table[lengthOfTable].Set(vArray[SPHERE], OBJ_vertices[SPHERE].size());
    delete[] Table_of_Objects;
    Table_of_Objects = Extended_Table;
    NUMBER_OF_OBJECTS += 1;
    //delete[] Extended_Table;
    Extended_Table = NULL;
}
*/

//zwiekszenie ilosci obiektow
void ExtendVector(){
    CSceneObject newObj;
    newObj.Set(vArray[SPHERE], OBJ_vertices[SPHERE].size());
    vectors_of_objects.push_back(newObj);
    NUMBER_OF_OBJECTS += 1;
}

//zmniejszenie ilosci obiektow
void ShortenVector(){
    //std::vector < CSceneObject > temp;

    for(int i=0; i<vectors_of_objects.size(); i++){
        if(vectors_of_objects[i].CollisionID == 1){
            vectors_of_objects.erase( vectors_of_objects.begin() + i );
            i -= 1;
            NUMBER_OF_OBJECTS -= 1;
        }
    }
}


// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Geometria sceny

	matView = glm::translate( glm::mat4( 1.0 ), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	matProjView = matProj * matView;

    glUseProgram( program );

        glUniformMatrix4fv( glGetUniformLocation( program, "matPV" ), 1, GL_FALSE, glm::value_ptr(matProjView) );
        glUniformMatrix4fv( glGetUniformLocation( program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );



        glm::mat4 matModel(1.0);
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



        for (int i=0; i<NUMBER_OF_OBJECTS; ++i)
        {
            if (vectors_of_objects[i].CollisionID == 1)
            {
                // Wlaczenie zapisywania
                glStencilMask(0xFF);
                // Wpisanie 1
                glStencilFunc(GL_ALWAYS, 1, 0xFF);

                // Renderowanie na ekran i do stencil buffera
                vectors_of_objects[i].Draw(program);

                // Wylaczenie zapisywania
                glStencilMask(0x00);

                //komentarz informacyjny. NIE usuwac tego u gory.

                // Wylaczenie zapisywania
                glStencilMask(0x00);
                // Ustwianie funkcji testujacej
                // Obiekt bedzie rysowany tylko tam, gdzie stencil buffer nie jest rowny 1
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

                // Obliczenie nowej macierzy modelu (przeskalowanie obiektu)
                glm::mat4 matModel = vectors_of_objects[i].matModel;
                vectors_of_objects[i].matModel = glm::scale(vectors_of_objects[i].matModel, glm::vec3(1.2, 1.2, 1.2));
                glUniform1i( glGetUniformLocation( program, "selected" ), 1);

                vectors_of_objects[i].Draw(program);

                glUniform1i( glGetUniformLocation( program, "selected" ), 0);
                vectors_of_objects[i].matModel = matModel;

                //printf("CollisionID = %d \n", CollisionID);

                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
            }
            else
            {
                 vectors_of_objects[i].Draw(program);
            }

        }

    // ------------------------------------------------
    // Renderowanie powiekszonego obiektu zaznaczonego
    // ------------------------------------------------

    //glDisable(GL_DEPTH_TEST);

    //glEnable(GL_DEPTH_TEST);



    glUseProgram( 0 );
	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
    Window_width = width;
    Window_height = height;

	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{

	// tekstura
	int tex_width;
	int tex_height;
	unsigned char *tex_data;

	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -5;
	_scene_translate_y = -3;
	_scene_rotate_y = -2;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Programowanie potoku
	program = glCreateProgram();
	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program );

	// GROUND
	if (!loadOBJ("chess.obj", OBJ_vertices[GROUND], OBJ_uvs[GROUND], OBJ_normals[GROUND]))
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

    // Inicjalizacja obiektow
    for (int i=0; i<NUMBER_OF_OBJECTS; ++i)
    {
        Table_of_Objects[i].Set(vArray[SPHERE], OBJ_vertices[SPHERE].size());
        vectors_of_objects.push_back(Table_of_Objects[i]);
    }

    // Test glebokosci
	glEnable( GL_DEPTH_TEST );
    glDepthFunc(GL_LESS);

    // Test szablonowy
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

}


// --------------------------------------------------------------
void MouseButton( int button, int state, int winX, int winY )
{
    /* stare
    if( button == GLUT_LEFT_BUTTON )
    {
        _mouse_buttonState = state;

        if( state == GLUT_DOWN )
        {
            _mouse_buttonX = winX;
            _mouse_buttonY = winY;
        }

    }
    */
    int mod;
    if(button == GLUT_RIGHT_BUTTON) {
                printf("RIGHT MOUSE KEY");
                for (int i=0; i<NUMBER_OF_OBJECTS; ++i){

                    vectors_of_objects[i].CollisionID = 0;
                }}
    if( button == GLUT_LEFT_BUTTON )  //SINGLE - TARGET
    {

        _mouse_buttonState = state;

        if( state == GLUT_DOWN )
        {
            _mouse_buttonX = winX;
            _mouse_buttonY = winY;
        }

        if ( state == GLUT_DOWN )
        {
            mod = glutGetModifiers();

            float winZ = 0;

            //odczytywanie glebi dla kliknietego piksela
            glReadPixels( winX, Window_height-winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

            //rzutowanie wsteczne uzyskanych wspolrzednych winx, winy i winz do przestrzeni obiektu
            glm::vec3 point = glm::unProject(glm::vec3(winX, Window_height-winY, winZ), matView, matProj, glm::vec4(0,0, Window_width, Window_height));

            printf("Window coordinates (%d,%d) \n", winX, winY );
            printf("World  coordinates (%f,%f,%f) \n", point.x, point.y, point.z );


            CollisionID = -1;
            for (int i=0; i<NUMBER_OF_OBJECTS; ++i)
            {
                float dist = glm::distance(vectors_of_objects[i].Position, point);
                if (dist < vectors_of_objects[i].radius )
                {
                    vectors_of_objects[i].CollisionID = 1;
                    printf("Selected object id=%d \n", i);
                }
                else if (mod == (GLUT_ACTIVE_SHIFT)) {
                    //Jesli dotarlo do tego miejsca to nie wazne w co klikniesz nie zmieni to juz zaznaczonych kulek
                }
                else{
                     vectors_of_objects[i].CollisionID = 0;
                }

            }

            }
        }

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
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 1){
                    vectors_of_objects[i].MoveXZ(move_vec, 0.0f);
                }
            }
            break;
         case 's':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 1){
                    vectors_of_objects[i].MoveXZ(-move_vec, 0.0f);
                }
            }
            break;

         case 'd':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 1){
                    vectors_of_objects[i].MoveXZ(0.0f, move_vec);
                }
            }
            break;
         case 'a':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 1){
                    vectors_of_objects[i].MoveXZ(0.0f, -move_vec);
                }
            }
            break;
         case 'r':
            //ExtendTable();
            ExtendVector();
            break;
         case 'q':
            ShortenVector();
            break;

    }

    glutPostRedisplay();
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    //test1
//    for(int i = 0; i != NUMBER_OF_OBJECTS; i++){
//        Table_of_Objects[i] = new CSceneObject();
//   }

	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( Window_width, Window_height );
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


	//delete[] Table_of_Objects;
//    for(int i = 0; i != NUMBER_OF_OBJECTS; i++){
//        delete Table_of_Objects[i];
//    }
	return 0;
}
