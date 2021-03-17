#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <GL/glew.h>
#include <GL/freeglut.h>
//#include <GL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.hpp"
#include "shader_stuff.hpp"
#include "object3d.hpp"
#include "stb_image.h"
#include "program.hpp"
#include "model.hpp"


GLuint TextureID;
GLuint id;
char const* path;
        int channels;
        int width, height;
//clock_t last_T = 0;

std::vector<glm::vec3> OBJ_vertices;
std::vector<glm::vec2> OBJ_uvs;
std::vector<glm::vec3> OBJ_normals;
std::vector<Model> models;
// ---------------------------------------
// NOWE : OSWIETLENIE - parametry
// ---------------------------------------
typedef struct SLight
{
	glm::vec4 Ambient;
	glm::vec4 Diffuse;
	glm::vec4 Specular;
	glm::vec4 Position;
	float Attenuation;

} SLight;


SLight Light1 = {
	glm::vec4(0.3, 0.3, 3.3, 1.0),
	glm::vec4(0.8, 0.8, 0.8, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0),
	0.01f,
};


// ---------------------------------------
// NOWE : MATERIALY - parametry
// ---------------------------------------
typedef struct SMaterial
{

	glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    GLfloat Shininess;

} SMaterial;


SMaterial Material1 = {
	glm::vec4(0.2f, 0.8f, 0.2f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	32.0f
};

SMaterial Material2 = {
	glm::vec4(1.0f, 0.2f, 0.2f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
	32.0f
};
//glm::vec3 spiders[] = {
  //      glm::vec3( 0.0f,  0.0f,  0.0f),
    //    glm::vec3( 2.0f,  5.0f, -15.0f),
      //  glm::vec3(-1.5f, -2.2f, -2.5f),
        //glm::vec3(-3.8f, -2.0f, -12.3f),
       // glm::vec3( 2.4f, -0.4f, -3.5f),
       // glm::vec3(-1.7f,  3.0f, -7.5f),
       // glm::vec3( 1.3f, -2.0f, -2.5f),
       // glm::vec3( 1.5f,  2.0f, -2.5f),
       // glm::vec3( 1.5f,  0.2f, -1.5f),
       // glm::vec3(-1.3f,  1.0f, -1.5f)
    //};


// ---------------------------------------


// ---------------------------------------
// Macierze przeksztalcen i rzutowania
// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel;
glm::mat4x4 matProjView;

glm::vec3 Camera_Position;

// ---------------------------------------
// Obiekty na scenie
// ---------------------------------------
CObject Scene, Tree;


void DrawModel(Model model) {
	glBindVertexArray(model.idVAO);
	glUseProgram(model.program->id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.texture->id);

	// Macierz widoku
	matView = glm::mat4x4( 1.0 );
	matModel = glm::mat4x4(1.0);

	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	matView = glm::translate( matView, glm::vec3(_scene_translate_x, _scene_translate_y, _scene_translate_z));

	matModel = glm::translate( matModel, glm::vec3(model.x, model.y, model.z));
	matModel = glm::rotate( matModel, model.rotation_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matModel = glm::rotate( matModel, model.rotation_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	matModel = glm::rotate( matModel, model.rotation_z, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	matModel = glm::scale(matModel, glm::vec3(model.scale));

	// Obliczanie macierzy rzutowania
	matProjView = matProj * matView * matModel;

	// Przekazanie macierzy rzutowania
	glUniformMatrix4fv( glGetUniformLocation(model.program->id, "matProjView"), 1, GL_FALSE,  glm::value_ptr(matProjView));
	glUniform1i(glGetUniformLocation(model.program->id, "tex0"), 0);

	glUniform1i(glGetUniformLocation(model.program->id, "tex0"), 0);
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
		glUniform3fv( glGetUniformLocation( model.program->id, "Camera_Position" ), 1, &Camera_Position[0] );

		// (b) oswietlenie
		glUniform4fv( glGetUniformLocation(model.program->id, "myLight.Ambient"), 1, &Light1.Ambient[0]);
		glUniform4fv( glGetUniformLocation(model.program->id, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
		glUniform4fv( glGetUniformLocation(model.program->id, "myLight.Specular"), 1, &Light1.Specular[0]);
		glUniform4fv( glGetUniformLocation(model.program->id, "myLight.Position"), 1, &Light1.Position[0]);
		glUniform1f( glGetUniformLocation(model.program->id, "myLight.Attenuation"), Light1.Attenuation);


		// 3. Renderowanie obiektow

		// Pierwszy obiekt
		{
			// (a) material
			glUniform4fv( glGetUniformLocation(model.program->id, "myMaterial.Ambient"), 1, &Material1.Ambient[0]);
			glUniform4fv( glGetUniformLocation(model.program->id, "myMaterial.Diffuse"), 1, &Material1.Diffuse[0]);
			glUniform4fv( glGetUniformLocation(model.program->id, "myMaterial.Specular"), 1, &Material1.Specular[0]);
			glUniform1f( glGetUniformLocation(model.program->id, "myMaterial.Shininess"), Material1.Shininess);

			// (b) macierz rzutowania
			glm::mat4 matModel = glm::mat4(1.0);
			matProjView = matProj * matView * matModel;
			glUniformMatrix4fv( glGetUniformLocation( model.program->id, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );

			// (c) Renderowanie
			Scene.Draw();
		}

		// Drugi obiekt
		{
			// (a) material
			glUniform4fv( glGetUniformLocation(model.program->id, "myMaterial.Ambient"), 1, &Material2.Ambient[0]);
			glUniform4fv( glGetUniformLocation(model.program->id, "myMaterial.Diffuse"), 1, &Material2.Diffuse[0]);
			glUniform4fv( glGetUniformLocation(model.program->id, "myMaterial.Specular"), 1, &Material2.Specular[0]);
			glUniform1f( glGetUniformLocation(model.program->id, "myMaterial.Shininess"), Material2.Shininess);

			// (b) macierz rzutowania
			glm::mat4 matModel = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.0, 2.0));
			matProjView = matProj * matView * matModel;
			glUniformMatrix4fv( glGetUniformLocation( model.program->id, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );

			// (c) Renderowanie
			Tree.Draw();
			glAttachShader( model.program->id, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( model.program->id, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( model.program->id );
		}


	glDrawArrays(GL_TRIANGLES, 0, model.vertex_count);
};

// ---------------------------------------
void DisplayScene()
{

    glBindVertexArray(0);
	// Czyszczenie ramki
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // ------------------------------
	// Geometria sceny
	matView = glm::mat4x4( 1.0 );
	matModel = glm::mat4x4(1.0);
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	Camera_Position = ExtractCameraPos(matView);

	// 1. Wlaczanie programu
	//glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);



		// 2. Wysylanie parametrow sceny

		// (a) pozycja kamery






	// 4. Wylaczanie programu
	for(Model m : models)
		DrawModel(m);
	glUseProgram( 0 );
	glBindVertexArray( 0 );
    glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}


// ---------------------------------------------------
void Initialize()
{


	unsigned char* image;

	stbi_set_flip_vertically_on_load(1);
  unsigned char* data = stbi_load(path, &width, &height,
                                 &channels, 0);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D,id);

    if(channels == 3) //RGB
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if(channels == 4) //RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

//if (!loadOBJ("flower.obj", OBJ_vertices, OBJ_uvs, OBJ_normals))
//	{
//		printf("Not loaded!\n");
//		exit(1);
//	}

	//glEnable(GL_TEXTURE_2D);

	//glGenTextures(1, &TextureID);
//glBindTexture(GL_TEXTURE_2D, TextureID);




//	image = SOIL_load_image("flower.png", &width, &height, 0, SOIL_LOAD_RGBA);
//	if (image == NULL)
//	{
	//	printf("Blad odczytu pliku graficznego!\n");
	//	exit(1);
//	}

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//SOIL_free_image_data(image);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //stbi_set_flip_vertically_on_load(1);


	// Programowanie potoku
	id = glCreateProgram();


    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glEnable( GL_DEPTH_TEST );

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );





	Scene.Load("scene.obj");
	Tree.Load("object.obj");





Program* program_flower = new Program("vertex.glsl", "fragment.glsl");
		Texture* flower_tex = new Texture("flower.png");


        for(int i=0; i<5; i++) {
		float min = -5.0f;
		float max = 5.0f;
		float x = min + rand()/(float)RAND_MAX*(max-min); //losowanie mi�dzy 2 liczbami
	    float z = min + rand()/(float)RAND_MAX*(max-min);
		float y_rotation = rand() / (float) RAND_MAX * (180.0f);
		Model flower(x, -0.6f, z, 0.0f, y_rotation, 0.0f, 0.3f, "item.obj", program_flower, flower_tex);
		Model flower_rotated(x, -0.6f, z, 0.0f, (y_rotation+90.0f), 0.0f, 0.3f, "item.obj", program_flower, flower_tex);
		models.push_back(flower);
		models.push_back(flower_rotated);




	// Ladowanie obiektow


}
}

// ---------------------------------------------------
void Animation(int frame)
{

	glutPostRedisplay();
    glutTimerFunc(10, Animation, 0);

}


// ---------------------------------------------------
int main( int argc, char *argv[] ) {
   // srand(time(NULL));
    // GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutInitWindowSize( 500, 500 );
    glutCreateWindow( "OpenGL" );

	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

	glutTimerFunc(10, Animation, 0);

    glutMainLoop();

    // Cleaning();
   // glDeleteProgram( idProgram );

    return 0;
}
