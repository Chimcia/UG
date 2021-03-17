#include <vector>
#include "texture.hpp"

class Model {
    public: //aby metody były publiczne
        GLuint idProgram;
        GLuint idVAO;
        GLuint idVBO_vertices;
        GLuint idVBO_uvs;
        GLuint idVBO_normals;
        GLfloat x, y, z;
        GLfloat rotation_x, rotation_y, rotation_z;
        GLfloat scale;
        int vertex_count;
        Program* program; //jeden program może rysować wiele róznych modeli
        Texture* texture;

        Model(
            GLfloat x,
            GLfloat y,
            GLfloat z,
            GLfloat rotation_x,
            GLfloat rotation_y,
            GLfloat rotation_z,
            GLfloat scale,
            const char* path,
            Program* program,
            Texture* texture
        ); //konstruktor


};

Model::Model(GLfloat x, GLfloat y, GLfloat z,
             GLfloat rotation_x, GLfloat rotation_y, GLfloat rotation_z,
             GLfloat scale,
             const char* path, Program* program, Texture* texture) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->rotation_x = rotation_x;
    this->rotation_y = rotation_y;
    this->rotation_z = rotation_z;
    this->scale = scale;
    this->program = program;
    this->texture = texture;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    loadOBJ(path, vertices, uvs, normals); //funkcja wpisuje OBJ do tablic
    this->vertex_count = vertices.size();

// --- TWORZENIE BUFORÓW
    glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	glGenBuffers(1, &idVBO_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers(1, &idVBO_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_uvs);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

    glGenBuffers(1, &idVBO_normals);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 2 );

	glBindVertexArray( 0 );
};

