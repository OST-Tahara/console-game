#include <Windows.h>
#include <stdio.h>
#include "Utility.h"


//---------------------------------------------------------
// キー操作
//---------------------------------------------------------

typedef struct {
    int        code;
    InputState state;
} KeyStatus;

KeyStatus key_status_list[] = {{ VK_SPACE, InputStateLeft},
                               { VK_LEFT,  InputStateLeft},
                               { VK_UP,    InputStateLeft},
                               { VK_RIGHT, InputStateLeft},
                               { VK_DOWN,  InputStateLeft}};

#define KEY_STATE_LIST_COUNT (sizeof(key_status_list) / sizeof(key_status_list[0]))


InputState GetKeyStatus( int keycode )
{
    int i;
    for( i = 0; i < KEY_STATE_LIST_COUNT; i++ ) 
    {
        if( key_status_list[i].code == keycode )
        { 
            return key_status_list[i].state;
        }
    }

    return InputStateLefting;
}


void UpdateKeyStatus( void )
{
    int i;
    for( i = 0; i < KEY_STATE_LIST_COUNT; i++ ) 
    {
        if( GetKeyState( key_status_list[i].code ) < 0 )  // push
        {
            if( (key_status_list[i].state == InputStatePush) || (key_status_list[i].state == InputStatePushing) )
            {
                key_status_list[i].state = InputStatePushing;
            }
            else 
            {
                key_status_list[i].state = InputStatePush;
            }
        }
        else                                             // left
        {
            if( (key_status_list[i].state == InputStateLeft) || (key_status_list[i].state == InputStateLefting) )
            {
                key_status_list[i].state = InputStateLefting;
            }
            else 
            {
                key_status_list[i].state = InputStateLeft;
            }
        }
    }
}


//---------------------------------------------------------
// コンソール表示
//---------------------------------------------------------

#define CONSOLE_WIDTH  30
#define CONSOLE_HEIGHT 20

static int  console_width   = 0;
static int  console_height  = 0;
static char *console_buffer = NULL;


void InitConsole( int width, int height ) 
{
    console_width  = width;
    console_height = height;

    size_t buffer_size = console_width * console_height;
    console_buffer = malloc( buffer_size );

    ClrConsole();
}


void ClrConsole( void )
{
    size_t buffer_size = console_width * console_height;
    memset( console_buffer, ' ', buffer_size );
}


void PutCharConsole( int x, int y, char str )
{
    if( (x >= 0) && (x < console_width) && (y >= 0) && (y < console_height) )
    {
        int index = x + y * console_width;
        console_buffer[index] = str;
    }
}


void UpdateConsole( void ) 
{
    HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE );
    COORD  origin = { 0, 0 };
    SetConsoleCursorPosition( handle, origin );

    SetConsoleTextAttribute( handle, BACKGROUND_INTENSITY );

    int y;
    for( y = 0; y < console_height; y++ )
    {
        int index_y = y * console_width;

        int x;
        for( x = 0; x < console_width; x++ )
        {
            putchar( console_buffer[index_y + x] );
        }
        putchar( '\n' );
    }
}






