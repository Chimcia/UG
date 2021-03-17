#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <iterator>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Window_width = 800;
int Window_height = 600;

glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjView;
glm::mat4x4 matModel(1.0);
//glm::vec3 Position;
//glm::vec3 OldPosition = Position;
glm::vec3 last_mouse;
float scale = 0.1f;
int selected_number=0;
int number_of_players=0;
int frame=0;
int frames = 0;
int collected_orbs  = 0;
static float initial_time = GetTickCount() * 0.001f, final_time;
//float consumed=0.0;
float growth_factor = 1.0;
float radius=1.1;
float plus_radius=collected_orbs*0.2;
int Mode=0;  //0-setup,1-game

#include "stuff.h"
#include "ground.h"
#include "player.h"
#include "text.h"
#include "text-ft.h"



#define NUMBER_OF_MESHES 2
int NUMBER_OF_OBJECTS = 50;

enum {
    GROUND = 0,
    SPHERE = 1,
    KONIEC=0,
    FULLSCREEN,
    PLAYER2,

};

enum {
    PROGRAM_SCENE,
    NUMBER_OF_PROGRAMS,
};


// ---------------------------------------
GLuint program[NUMBER_OF_PROGRAMS];
GLuint vBuffer_pos[NUMBER_OF_MESHES];
GLuint vBuffer_uv[NUMBER_OF_MESHES];
GLuint vArray[NUMBER_OF_MESHES];

std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_MESHES];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_MESHES];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_MESHES];
GLuint TextureID[NUMBER_OF_MESHES];



// ---------------------------------------

class CSceneObject {

public:

    glm::vec3 Position;
    glm::vec3 Resize;
    glm::vec3 point;
  //  float Size;
   // glm::vec3 last_mouse;
    float radius;


    CGround *Ground = NULL;

    GLuint VAO;
    int VBO_Size;

    glm::mat4x4 matModel;

    int CollisionID = 0;

    CSceneObject() {
        Position.x = rand()%40 - 20;
        Position.y = 0;
        Position.z = rand()%40 - 20;
        radius = 1.1;


        matModel = glm::translate(glm::mat4(1.0), Position);

    }




    void UpdateLatitute()
	{
		float newAlt = Ground->getAltitute(glm::vec2(Position.x, Position.z));

		Position.y = newAlt;
		matModel = glm::translate(glm::mat4(1.0), Position);
	}


	void MouseButtonCoords() {
	    Position.y = last_mouse.y;
        Position.x = last_mouse.x;
        Position.z = last_mouse.z;
        matModel = glm::translate(glm::mat4(1.0), Position);
	}


    void Set(GLuint _vao, int _size, CGround *_ground) {
        VAO = _vao;
        VBO_Size = _size;
        Ground = _ground;
        UpdateLatitute();
    }

    void Set_E(GLuint _vao, int _size, CGround *_ground) {
        VAO = _vao;
        VBO_Size = _size;
        Ground = _ground;
        MouseButtonCoords();
        printf("World  Set_E (%f,%f,%f) \n", Position.x, Position.y, Position.z );
    }



    void Draw(GLuint prog) {

        scale += 0.001f;
        glUniformMatrix4fv( glGetUniformLocation( prog, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );
        glUniform1f( glGetUniformLocation(prog, "u_time" ), scale);

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }
   // void glScalef(GLfloat x, GLfloat y, GLfloat z);
 //   void Grow(float x, float y, float z){
    //    Resize= glm::vec3(x, y, z);
    //    Position = glm::vec3(x, y, z);
       // glm::mat4 matModel = glm::mat4(2.0f);
       // matModel = glm::translate(matModel, glm::vec3(5.5f, 5.5f, 5.0f));
        //matModel = glm::translate(glm::mat4(1.0),Resize);
     //   matModel = glm::scale(
     //           glm::translate(glm::mat4(1.0),Position));
     //           )glm::mat4(1.0),Resize);
        //matModel = glm::translate(glm::mat4(1.0))
 //   }
 void Grow(){
   // growth_factor=growth_factor + consumed ;
    matModel = glm::scale(matModel, glm::vec3(growth_factor, growth_factor, growth_factor));
    radius=radius+plus_radius;
        }
//float growth_factor
    void SetPosition(float x, float y, float z) {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    // zmiana pozycji na scenie
    void MoveXZ(float _x, float _z) {
        Position += glm::vec3(_x, 0.0, _z);
        matModel = glm::translate(glm::mat4(1.0), Position);
        UpdateLatitute();
    }
    void MoveXZF() {
        glTranslatef(5.0f,5.0f,5.0f);
    }

    // zmiana pozycji na scenie gdy nie zachodzi kolizja z obiektem other
    void MoveXZ(float _x, float _z, const CSceneObject &_other) {
        glm::vec3 OldPosition = Position;
        Position += glm::vec3(_x, 0.0, _z);
        if(this->isCollision(_other)) {
            Position = OldPosition;
        }

        matModel = glm::translate(glm::mat4(1.0), Position);
        UpdateLatitute();

    }


    // sprawdzanie kolizji z innym obiektem
    bool isCollision(const CSceneObject &_other) {
        float distance = glm::distance(this->Position, _other.Position);
        if (distance < this->radius + _other.radius) {

            return true;
        }
        return false;
    }

};

CGround ziemia;

CSceneObject* Table_of_Objects = new CSceneObject[NUMBER_OF_OBJECTS];
CSceneObject Candy;
CSceneObject User;
//CPlayer Player;\\
//int NUMBER_OF_PLAYERS = 50;
//CPlayer* Table_of_Players = new CPlayer[NUMBER_OF_PLAYERS];


std::vector < CSceneObject > vectors_of_objects;
std::vector < CPlayer > vectors_of_players;

void Consume(float consumed){

    //printf("powinno dodac growth ratio: i=%d \n", Growth_ratio);
    growth_factor=consumed+growth_factor;
    radius+=1;
   // growth_factor=growth_factor + consumed;
   // printf("consumed: i=%d \n", growth_factor);
   // printf("growth factor: i=%d \n", growth_factor);
}

//int CollisionID = 2;
//int CollisionIDArray[NUMBER_OF_OBJECTS] = { };//wypelnienie tablicy zerami.

//zwiekszenie ilosci obiektow
void ExtendVector(){
    CSceneObject newObj;
    newObj.Set(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &ziemia);
    vectors_of_objects.push_back(newObj);
    NUMBER_OF_OBJECTS += 1;
}
void ExtendVector_E(){
    CSceneObject newObj;
    newObj.Set_E(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &ziemia);
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
            selected_number-=1;
            printf("collision 1 deleted \n");

        }
        else if(vectors_of_objects[i].CollisionID == 3){
            vectors_of_objects.erase( vectors_of_objects.begin() + i );
            i -= 1;
            NUMBER_OF_OBJECTS -= 1;
            printf("collision 5 deleted \n");
        }
    }
}

void BecomePlayer(){
for(int i=0; i<vectors_of_objects.size(); i++){
        if(vectors_of_objects[i].CollisionID == 1 && selected_number==1 && number_of_players==0) {
            vectors_of_objects[i].CollisionID = 2;
            number_of_players+=1;
            selected_number-=1;
          //  printf("jestem graczem \n");
            //printf("test zaznaczonych selected_number=%d \n", selected_number);
        }
    }
}
void UpdateRadius(float consumed){
    radius=radius+consumed;
    }
#include "Keyboard_setup.h"
#include "Keyboard_game.h"
#include "mouse_setup.h"
#include "mouse_game.h"
#include "menu.h"
//PLAYERS
/*void ExtendVector_Player(){
    CPlayer newObj;
    newObj.Set(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &ziemia);
    vectors_of_players.push_back(newObj);
    NUMBER_OF_OBJECTS += 1;
}
void ExtendVector_EPlayer(){
    CPlayer newObj;
    newObj.Set_E(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &ziemia);
    vectors_of_players.push_back(newObj);
    NUMBER_OF_OBJECTS += 1;
}
//zmniejszenie ilosci obiektow
void ShortenVector_Player(){
    //std::vector < CSceneObject > temp;

    for(int i=0; i<vectors_of_players.size(); i++){
        if(vectors_of_players[i].CollisionID == 1){
            vectors_of_players.erase( vectors_of_players.begin() + i );
            i -= 1;
            NUMBER_OF_OBJECTS -= 1;
        }
    }
}
*/

// ---------------------------------------
void DisplayScene() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Geometria sceny

	matView = glm::translate( glm::mat4( 1.0 ), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	matProjView = matProj * matView;

    glUseProgram( program[PROGRAM_SCENE] );

        glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matPV" ), 1, GL_FALSE, glm::value_ptr(matProjView) );
        glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
     //   glm::mat4 matModel(1.0);
        glUniformMatrix4fv( glGetUniformLocation( program[PROGRAM_SCENE], "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        //0-bubble
        //1-grass
        //2-jest wylaczona
        //3-metal

        // GROUND
        //glActiveTexture(GL_TEXTURE0);
      //  glBindTexture(GL_TEXTURE_2D, TextureID[GROUND]);
        glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 1);

        glBindVertexArray( vArray[GROUND] );
        glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[GROUND].size() );
        glBindVertexArray( 0 );


        // SPHERE
        glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 3);
        glBindVertexArray( vArray[SPHERE] );
     //   glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[SPHERE].size());
        glBindVertexArray( 0 );



       // glUniform1i(glGetUniformLocation(program[PROGRAM_SCENE], "tex0"), 3);
       // glBindVertexArray( vArray[PLAYER] );
      //  glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[PLAYER].size());
      //  glBindVertexArray( 0 );




        for (int i=0; i<NUMBER_OF_OBJECTS; ++i) {
            if (vectors_of_objects[i].CollisionID == 1) {
                // Wlaczenie zapisywania
                glStencilMask(0xFF);
                // Wpisanie 1
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                // Renderowanie na ekran i do stencil buffera
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
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
                //glUseProgram( 0 );
                vectors_of_objects[i].matModel = glm::scale(vectors_of_objects[i].matModel, glm::vec3(1.5, 1.5, 1.5));
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 1);
               // glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 2);
                //glUseProgram(program);
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 0);
                vectors_of_objects[i].matModel = matModel;
                //printf("CollisionID = %d \n", CollisionID);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
            }
            if (vectors_of_objects[i].CollisionID == 2) {
                // Wlaczenie zapisywania
                glStencilMask(0xFF);
                // Wpisanie 1
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                // Renderowanie na ekran i do stencil buffera
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
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
                //glUseProgram( 0 );
                vectors_of_objects[i].matModel = glm::scale(vectors_of_objects[i].matModel, glm::vec3(1.5, 1.5, 1.5));
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "player1" ), 1);
               // glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 2);
               // glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "player1" ), 2);
                //glUseProgram(program);
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "player1" ), 0);
              //  glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 3);
                vectors_of_objects[i].matModel = matModel;
                //printf("CollisionID = %d \n", CollisionID);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
            }
          /*  if (vectors_of_objects[i].CollisionID == 3) {
                // Wlaczenie zapisywania
                glStencilMask(0xFF);
                // Wpisanie 1
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                // Renderowanie na ekran i do stencil buffera
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
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
                //glUseProgram( 0 );
                vectors_of_objects[i].matModel = glm::scale(vectors_of_objects[i].matModel, glm::vec3(1.5, 1.5, 1.5));
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 1);
               // glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 2);
              // glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "player1" ), 2);
                //glUseProgram(program);
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 0);
                glUniform1i( glGetUniformLocation( program[PROGRAM_SCENE], "selected" ), 3);
                vectors_of_objects[i].matModel = matModel;
                //printf("CollisionID = %d \n", CollisionID);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
            }*/
            else {
                vectors_of_objects[i].Draw(program[PROGRAM_SCENE]);
            }

        }
      /*  for (int i=0; i<NUMBER_OF_OBJECTS; ++i) {
            if (vectors_of_players[i].CollisionID == 1) {
                // Wlaczenie zapisywania
                glStencilMask(0xFF);
                // Wpisanie 1
                glStencilFunc(GL_ALWAYS, 1, 0xFF);

                // Renderowanie na ekran i do stencil buffera
                vectors_of_players[i].Draw(program);

                // Wylaczenie zapisywania
                glStencilMask(0x00);

                //komentarz informacyjny. NIE usuwac tego u gory.

                // Wylaczenie zapisywania
                glStencilMask(0x00);
                // Ustwianie funkcji testujacej
                // Obiekt bedzie rysowany tylko tam, gdzie stencil buffer nie jest rowny 1
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

                // Obliczenie nowej macierzy modelu (przeskalowanie obiektu)
                glm::mat4 matModel = vectors_of_players[i].matModel;

                //glUseProgram( 0 );
                vectors_of_players[i].matModel = glm::scale(vectors_of_players[i].matModel, glm::vec3(1.5, 1.5, 1.5));

                glUniform1i( glGetUniformLocation( program, "selected" ), 1);

                //glUseProgram(program);

                vectors_of_players[i].Draw(program);

                glUniform1i( glGetUniformLocation( program, "selected" ), 0);
                vectors_of_players[i].matModel = matModel;

                //printf("CollisionID = %d \n", CollisionID);

                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
            }
            else {
                vectors_of_players[i].Draw(program);
            }

        }
*/
    // ------------------------------------------------
    // Renderowanie powiekszonego obiektu zaznaczonego
    // ------------------------------------------------

    //glDisable(GL_DEPTH_TEST);

    //glEnable(GL_DEPTH_TEST);

                //TEXT(oddalenie od lewej,oddalenie od dolu,"tekst"
    //    DrawText8x16( 5, 5, "Staly tekst" );

      //  char txt[255];
      //  sprintf(txt, "Klatka %d!", frame++);
      //  DrawText8x16( 3, 21, txt, glm::vec4(2.0, 2.0, 0.0, 2.0) );
      //  DrawText8x16( 300, 3, "ESC - Wyjscie" );
    char txt[255];
    if(Mode==0){
        char txt[255];
    sprintf(txt,"SETUP MODE");
    RenderText(txt, 10, Window_height*0.90, 1.0f, glm::vec3(1.0f, 0.5f, 0.31f));

    sprintf(txt,"Shift+LPM - Wybranie wielekrotne");
    RenderText(txt, 10, 10, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprintf(txt,"Q - usuni�cie zaznaczonych obiekt�w");
    RenderText(txt, 10, 30, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprintf(txt,"R - dodanie obiektu");
    RenderText(txt, 10, 50, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprintf(txt,"E - dodanie w miejscu ostatniego LPM");
    RenderText(txt, 10, 70, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprintf(txt,"G - wyb�r gracza");
    RenderText(txt, 10, 90, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprintf(txt,"PPM - Menu");
    RenderText(txt, 10, 110,1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    sprintf(txt,"LPM - Wybranie obiektu");
    RenderText(txt, 10, 130, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    }
    if(collected_orbs<5 && Mode==1){
    sprintf(txt,"Collected orbs: %d", collected_orbs);
    RenderText(txt, 20, 20, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    int FPS;
	//char txt[255];
   // frames++;
	/*final_time = GetTickCount() * 0.001f;
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
    }*/
    if(collected_orbs>=5 && Mode==1){
        sprintf(txt,"WINNER WINNER CHICKEN DINNER");
        RenderText(txt, Window_width/5, Window_height/2, 1.0f, glm::vec3(0.65f, 0.90f, 0.15f));
        sprintf(txt,"Collected orbs: %d", collected_orbs);
        RenderText(txt, 20, 20, 1.0f, glm::vec3(0.65f, 0.90f, 0.15f));
    }

    glUseProgram( 0 );
	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height ) {
    Window_width = width;
    Window_height = height;

	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize() {

	// tekstura
	int tex_width;
	int tex_height;
	unsigned char *tex_data;
	InitDrawText();
    InitText("arial.ttf", 23);
	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -5;
	_scene_translate_y = -3;
	_scene_rotate_y = -2;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Programowanie potoku
	program[PROGRAM_SCENE] = glCreateProgram();
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program[PROGRAM_SCENE], LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program[PROGRAM_SCENE] );

	// GROUND
	if (!loadOBJ("chess.obj", OBJ_vertices[GROUND], OBJ_uvs[GROUND], OBJ_normals[GROUND])) {
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

glEnable( GL_DEPTH_TEST );


    // SPHERE
    if (!loadOBJ("sphere.obj", OBJ_vertices[SPHERE], OBJ_uvs[SPHERE], OBJ_normals[SPHERE])) {
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

	// PLAYER
	/*loadBMP_custom("bubbles.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[PLAYER]);
	glBindTexture(GL_TEXTURE_2D, TextureID[PLAYER]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    if (!loadOBJ("monkey.obj", OBJ_vertices[PLAYER], OBJ_uvs[PLAYER], OBJ_normals[PLAYER])) {
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[PLAYER] );
	glBindVertexArray( vArray[PLAYER] );

	glGenBuffers( 1, &vBuffer_pos[PLAYER] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[PLAYER] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[PLAYER].size() * sizeof(glm::vec3), &(OBJ_vertices[PLAYER])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[PLAYER] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[PLAYER] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[PLAYER].size() * sizeof(glm::vec2), &(OBJ_uvs[PLAYER])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );

*/


	loadBMP_custom("metal.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[PLAYER2]);
	glBindTexture(GL_TEXTURE_2D, TextureID[PLAYER2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



	ziemia.CreateFromOBJ(OBJ_vertices[GROUND]);





    // Inicjalizacja obiektow
    for (int i=0; i<NUMBER_OF_OBJECTS; ++i) {
        Table_of_Objects[i].Set(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &ziemia);
        vectors_of_objects.push_back(Table_of_Objects[i]);
    }

  //  for (int i=0; i<NUMBER_OF_PLAYERS; ++i) {
  //      Table_of_Players[i].Set(vArray[SPHERE], OBJ_vertices[SPHERE].size(), &ziemia);
  //      vectors_of_players.push_back(Table_of_Players[i]);
  //  }

   // Player.Set(vArray[PLAYER], OBJ_vertices[PLAYER].size(),&ziemia);
   // Player.SetPosition(10,5,0);

    //AKTYWOWANIE TEKSTUR
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureID[GROUND]);  //Chess

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);  //grass

//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, TextureID[PLAYER]);   //bubbless

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureID[PLAYER2]);   // metal

    // Test glebokosci
	glEnable( GL_DEPTH_TEST );
    glDepthFunc(GL_LESS);

    // Test szablonowy
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

}


// --------------------------------------------------------------


// --------------------------------------------------------------

void Animation(int frame){

    glutTimerFunc(5, Animation, 0);
    glutPostRedisplay();
}

// ---------------------------------------------------
int main( int argc, char *argv[] ) {

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
	glutMouseFunc( MouseButton_Setup );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard_Setup );
	glutSpecialFunc( SpecialKeys );
	glutTimerFunc(1, Animation, 0);

	int podmenuA = glutCreateMenu( Menu );
    glutAddMenuEntry( "Fullscreen", 'FULLSCREEN' );
    glutAddMenuEntry( "Windowed", 'WINDOWED' );
    int podmenuB = glutCreateMenu( Menu );
    glutAddMenuEntry( "Setup", 'SETUP' );
    glutAddMenuEntry( "Game", 'GAME' );
    glutCreateMenu( Menu );
    glutAddSubMenu( "Display", podmenuA );
    glutAddSubMenu( "Game Mode", podmenuB );
    glutAddMenuEntry( "Exit", 'KONIEC' );

	glutMainLoop();

	return 0;
}
