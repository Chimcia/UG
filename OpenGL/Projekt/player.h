#ifndef __CPLAYER
#define __CPLAYER


#define __ALTITUDE_ERROR	99999

// -------------------------------
class CPlayer {

public:

    glm::vec3 Position;
    glm::vec3 point;
   // glm::vec3 last_mouse;
    float radius;

    CGround *Ground = NULL;

    GLuint VAO;
    int VBO_Size;

    glm::mat4x4 matModel;

    int CollisionID = 0;

    CPlayer() {
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

    // zmiana pozycji na scenie gdy nie zachodzi kolizja z obiektem other
    void MoveXZ(float _x, float _z, const CPlayer &_other) {
        glm::vec3 OldPosition = Position;

        //printf("%d", _x);
        //float _y = 0.2f;
        Position += glm::vec3(_x, 0.0, _z);
        if(this->isCollision(_other)) {
            Position = OldPosition;
        }
        matModel = glm::translate(glm::mat4(1.0), Position);
        UpdateLatitute();

    }

    // sprawdzanie kolizji z innym obiektem
    bool isCollision(const CPlayer &_other) {
        float distance = glm::distance(this->Position, _other.Position);
        if (distance < this->radius + _other.radius) {
            return true;
        }
        return false;
    }

};




#endif
