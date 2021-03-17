#ifndef __Keyboard_Game
#define __Keyboard_Game
//Prawym i unpack
#define __ALTITUDE_ERROR	99999

// -------------------------------
void Keyboard_Game( unsigned char key, int x, int y ) {
    float move_vec = 0.2;




    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

        case 'w':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 2){
                    vectors_of_objects[i].MoveXZ(move_vec, 0.0f);
                    vectors_of_objects[i].Grow();
                }
            }
            break;
         case 's':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 2){
                    vectors_of_objects[i].MoveXZ(-move_vec, 0.0f);
                    vectors_of_objects[i].Grow();                }
            }
            break;

         case 'd':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 2){
                    vectors_of_objects[i].MoveXZ(0.0f, move_vec);
                    vectors_of_objects[i].Grow();                }
            }
            break;
         case 'a':
            for(int i = 0; i<NUMBER_OF_OBJECTS; i++){
                if (vectors_of_objects[i].CollisionID == 2){
                    vectors_of_objects[i].MoveXZ(0.0f, -move_vec);
                    vectors_of_objects[i].Grow();                }
            }
            break;
        case 'x':
            ShortenVector();
            break;

    }

	for(int i = 0; i < NUMBER_OF_OBJECTS; i++){
		if(vectors_of_objects[i].CollisionID == 2){
			for(int l = 0; l < NUMBER_OF_OBJECTS; l++){
				if(vectors_of_objects[i].isCollision(vectors_of_objects[l]) && l != i){
					vectors_of_objects.erase(vectors_of_objects.begin() + l);
					i -= 1;
					l -= 1;
					NUMBER_OF_OBJECTS -= 1;
					Consume(0.2);
				//	UpdateRadius(0.2);
					collected_orbs+=1;

				}
			}
		}
	}

    glutPostRedisplay();
}

#endif
