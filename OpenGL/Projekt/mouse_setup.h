#ifndef __MouseButton_Setup
#define __MouseButton_Setup

#define __ALTITUDE_ERROR	99999

// -------------------------------
void MouseButton_Setup( int button, int state, int winX, int winY ) {
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
                printf("RIGHT MOUSE KEY SETUP MODE \n");
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



            for (int i=0; i<NUMBER_OF_OBJECTS; ++i)
            {
                float dist = glm::distance(vectors_of_objects[i].Position, point);
                if (dist < vectors_of_objects[i].radius )
                {
                    if(vectors_of_objects[i].CollisionID == 0){
                            vectors_of_objects[i].CollisionID = 1;
                            selected_number+=1;
                         }

                    printf("Selected object id=%d , selected number selected_number=%d WYBRANO SINGLA \n", i, selected_number);
                    if(vectors_of_objects[i].CollisionID == 2){

                               vectors_of_objects[i].CollisionID = 0;
                               number_of_players=0;
                               printf("automatyczne usuwanie \n");
                               printf("test zaznaczonych selected_number=%d \n", selected_number);
                         }

                }
                else if (mod == (GLUT_ACTIVE_SHIFT)) {
                    //Jesli dotarlo do tego miejsca to nie wazne w co klikniesz nie zmieni to juz zaznaczonych kulek
                }
                    else{
                         if(vectors_of_objects[i].CollisionID == 1){

                               vectors_of_objects[i].CollisionID = 0;
                               selected_number-=1;
                               printf("automatyczne usuwanie \n");
                               printf("test zaznaczonych selected_number=%d \n", selected_number);
                         }

                    }
                    }
                }

            }

           }







#endif
