#ifndef __Menu
#define __Menu

#define __ALTITUDE_ERROR	99999

// -------------------------------
void Menu( int value )
{
    switch( value )
    {
	case 1:
		printf("Opcja nr jeden\n")	;
		break;

    case 'KONIEC':
        glutLeaveMainLoop();
        break;

    case 'FULLSCREEN':
        glutFullScreen();


        break;
    case 'WINDOWED':
        glutReshapeWindow(500, 500);
        break;
    case 'SETUP':
        glutMouseFunc( MouseButton_Setup );
        glutKeyboardFunc( Keyboard_Setup );
        Mode=0;
        break;
    case 'GAME':
        for(int i=0; i<vectors_of_objects.size(); i++){
        if(vectors_of_objects[i].CollisionID == 1){
            vectors_of_objects[i].CollisionID = 0;
            }
            }
        glutMouseFunc( MouseButton_Game );
        glutKeyboardFunc( Keyboard_Game );
        Mode=1;
        break;


        // GL_TEXTURE_MAG
    }

    printf("Wywolano funkcje Menu(%d)\n", value);
}

#endif
