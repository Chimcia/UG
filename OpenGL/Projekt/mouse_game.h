#ifndef __MouseButton_Game
#define __MouseButton_Game

#define __ALTITUDE_ERROR	99999

// -------------------------------
void MouseButton_Game( int button, int state, int winX, int winY ) {
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
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    int mod;
   // glm::vec3 last_mouse;

    if(button == GLUT_RIGHT_BUTTON) {
                printf("RIGHT MOUSE KEY GAME MODE \n");
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

            last_mouse.x = point.x;
            last_mouse.y = point.y;
            last_mouse.z = point.z;

            printf("TEST MYSZY (%f,%f,%f) \n", last_mouse.x, last_mouse.y, last_mouse.z );
            }
        }

    }




#endif
