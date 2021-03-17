#ifndef __Keyboard_Setup
#define __Keyboard_Setup

#define __ALTITUDE_ERROR	99999

// -------------------------------
void Keyboard_Setup( unsigned char key, int x, int y ) {
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
        case 'e':
            //ExtendTable();
         //   glutMouseFunc(MouseUpdate);
            ExtendVector_E();
            break;
         case 'q':
            ShortenVector();
            break;
         case 'g':
            BecomePlayer();
        // case 'v':
          //  outColor = texture( tex0, inoutUV ) * vec4(color, 1.0);
         //   break;

    }

    glutPostRedisplay();
}

#endif
