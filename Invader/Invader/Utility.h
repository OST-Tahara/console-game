#ifndef _UTILITY_H
#define _UTILITY_H

//---------------------------------------------------------
// キー操作
//---------------------------------------------------------

typedef enum {
    InputStateLeft = 0,
    InputStateLefting,
    InputStatePush,
    InputStatePushing
} InputState;

InputState GetKeyStatus( int keycode );
void UpdateKeyStatus( void );


//---------------------------------------------------------
// コンソール表示
//---------------------------------------------------------

void InitConsole( int width, int height );
void ClrConsole( void );
void PutCharConsole( int x, int y, char str );
void UpdateConsole( void );


//---------------------------------------------------------
// その他
//---------------------------------------------------------


#endif