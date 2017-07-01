#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "Utility.h"


static int stage_width  = 0;
static int stage_height = 0;

//---------------------------------------------------------
// ’e
//---------------------------------------------------------

typedef struct 
{
    int enable;
    int x;
    int y;
    int speed_x;
    int speed_y;
    int move_limit_count;
} BulletStatus;

static BulletStatus *bullets = NULL;
static int bullets_count = 0;

static void InitBullets( void )
{
    bullets_count = 100;
    bullets = malloc( bullets_count * sizeof(BulletStatus) );

    int i;
    for( i = 0; i < bullets_count; i++ )
    {
        bullets[i].enable = 0;
    }
}


static void AddBullet( int x, int y, int speed_x, int speed_y )
{
    int i;
    for( i = 0; i < bullets_count; i++ )
    {
        if( bullets[i].enable == 0 ) 
        {
            bullets[i].enable  = 1;
            bullets[i].x       = x;
            bullets[i].y       = y;
            bullets[i].speed_x = speed_x;
            bullets[i].speed_y = speed_y;
            bullets[i].move_limit_count = 5;

            break;
        }
    }
}


static void MoveBullets( void )
{
    int i;
    for( i = 0; i < bullets_count; i++ )
    {
        if( bullets[i].enable == 0 ) continue;
        if( bullets[i].move_limit_count > 0 )
        {
            bullets[i].move_limit_count--;
            continue;
        }
        bullets[i].move_limit_count = 5;

        bullets[i].x += bullets[i].speed_x;
        bullets[i].y += bullets[i].speed_y;

        if( (bullets[i].x < 0) || (bullets[i].x > (stage_width -1)) ) bullets[i].enable = 0;
        if( (bullets[i].y < 0) || (bullets[i].y > (stage_height-1)) ) bullets[i].enable = 0;
    }
}


static void PrintBullets( void )
{
    int i;
    for( i = 0; i < bullets_count; i++ )
    {
        if( bullets[i].enable )
        {
            PutCharConsole( bullets[i].x, bullets[i].y, '|' );
        }
    }
}


//---------------------------------------------------------
// ƒvƒŒƒCƒ„[
//---------------------------------------------------------

static int player_x = 0;
static int player_y = 0;

static void InitPlayer( FILE *file ) 
{
    fread( &player_x, sizeof(int), 1, file );
    fread( &player_y, sizeof(int), 1, file );
}


static void MovePlayer( void )
{
    static int move_limit_count = 0;

    if( move_limit_count > 0)
    {
        move_limit_count--;
        return;
    }

    int key_left  = GetKeyStatus(VK_LEFT);
    int key_right = GetKeyStatus(VK_RIGHT);

    if( (key_left == InputStatePush) || (key_left == InputStatePushing) )
    {
        if( player_x > 0 )
        {
            player_x--;
            move_limit_count = 5;
        }
    }

    if( (key_right == InputStatePush) || (key_right == InputStatePushing) )
    {
        if( player_x < (stage_width-1) )
        {
            player_x++;
            move_limit_count = 5;
        }
    }
}


static void FirePlayer( void )
{
    int key_space  = GetKeyStatus(VK_SPACE);

    if( key_space == InputStatePush )
    {
        AddBullet( player_x, player_y-1, 0, -1 );
    }
}


static void PrintPlayer( void )
{
    PutCharConsole( player_x, player_y, '@' );
}


//---------------------------------------------------------
// “G
//---------------------------------------------------------

typedef struct 
{
    int enable;
    int x;
    int y;
} EnemyStatus;

static EnemyStatus *enemys = NULL;
static int enemys_count = 0;

static int enemys_speed_x = 0;
static int enemys_speed_y = 0;

static void InitEnemys( FILE *file )
{
    fread( &enemys_count, sizeof(int), 1, file );
    enemys = malloc( enemys_count * sizeof(EnemyStatus) );

    int i;
    for( i = 0; i < enemys_count; i++ )
    {
        enemys[i].enable = 1;
        fread( &enemys[i].x, sizeof(int), 1, file );
        fread( &enemys[i].y, sizeof(int), 1, file );
    }

    enemys_speed_x = 1;
    enemys_speed_y = 0;
}


static void MoveEnemys( void )
{
    static int move_limit_count = 10;

    if( move_limit_count > 0)
    {
        move_limit_count--;
        return;
    }
    move_limit_count = 10;

    // ˆÚ“®
    int i;
    for( i = 0; i < enemys_count; i++ )
    {
        if( enemys[i].enable )
        {
            enemys[i].x += enemys_speed_x;
            enemys[i].y += enemys_speed_y;
        }
    }

    // ’[ŒŸõ
    int edge_left  = 0;
    int edge_right = 0;
    for( i = 0; i < enemys_count; i++ )
    {
        if( enemys[i].enable )
        {
            if( enemys[i].x <= 0 )
            {
                edge_left = 1;
                break;
            }
            if( enemys[i].x >= (stage_width-1) )
            {
                edge_right = 1;
                break;
            }
        }
    }

    if( edge_left || edge_right )
    {
        if( enemys_speed_y == 0 )
        {
            enemys_speed_x = 0;
            enemys_speed_y = 1;
        }
        else
        {
            enemys_speed_x = ( edge_left ) ? 1 : -1;
            enemys_speed_y = 0;
        }
    }
}


static void FireEnemys( void )
{
    int i;
    for( i = 0; i < enemys_count; i++ )
    {
        if( enemys[i].enable )
        {
            if( rand() % 1000 == 0 )
            {
                AddBullet( enemys[i].x, enemys[i].y+1, 0, 1 );
            }
        }
    }
}


static void PrintEnemys( void )
{
    int i;
    for( i = 0; i < enemys_count; i++ )
    {
        if( enemys[i].enable )
        {
            PutCharConsole( enemys[i].x, enemys[i].y, 'Q' );
        }
    }
}


//---------------------------------------------------------
// •Ç
//---------------------------------------------------------

typedef struct 
{
    int enable;
    int x;
    int y;
} WallStatus;

static WallStatus *walls = NULL;
static int walls_count = 0;


static void InitWalls( FILE *file )
{
    fread( &walls_count, sizeof(int), 1, file );
    walls = malloc( walls_count * sizeof(WallStatus) );

    int i;
    for( i = 0; i < walls_count; i++ )
    {
        walls[i].enable = 1;
        fread( &walls[i].x, sizeof(int), 1, file );
        fread( &walls[i].y, sizeof(int), 1, file );
    }
}


static void PrintWalls( void )
{
    int i;
    for( i = 0; i < walls_count; i++ )
    {
        if( walls[i].enable )
        {
            PutCharConsole( walls[i].x, walls[i].y, '#' );
        }
    }
}


//---------------------------------------------------------
// ƒQ[ƒ€§Œä
//---------------------------------------------------------

static void Collision( void )
{
    int bullet_i, enemy_i, wall_i;

    // ’e <-> “G
    for( bullet_i = 0; bullet_i < bullets_count; bullet_i++ )
    {
        if( bullets[bullet_i].enable == 0 ) continue;

        int b_x = bullets[bullet_i].x;
        int b_y = bullets[bullet_i].y;

        for( enemy_i = 0; enemy_i < enemys_count; enemy_i++ )
        {
            if( enemys[enemy_i].enable == 0 ) continue;

            int e_x = enemys[enemy_i].x;
            int e_y = enemys[enemy_i].y;

            if( (b_x == e_x) && (b_y == e_y) )
            {
                bullets[bullet_i].enable = 0;
                enemys[enemy_i].enable   = 0;
            }
        }
    }

    // ’e <-> Ž©‹@
    for( bullet_i = 0; bullet_i < bullets_count; bullet_i++ )
    {
        if( bullets[bullet_i].enable == 0 ) continue;

        int b_x = bullets[bullet_i].x;
        int b_y = bullets[bullet_i].y;

        if( (b_x == player_x) && (b_y == player_y) )
        {
            // @todo
        }
    }

    // ’e <-> •Ç
    for( bullet_i = 0; bullet_i < bullets_count; bullet_i++ )
    {
        if( bullets[bullet_i].enable == 0 ) continue;

        int b_x = bullets[bullet_i].x;
        int b_y = bullets[bullet_i].y;

        for( wall_i = 0; wall_i < walls_count; wall_i++ )
        {
            if( walls[wall_i].enable == 0 ) continue;

            int w_x = walls[wall_i].x;
            int w_y = walls[wall_i].y;

            if( (b_x == w_x) && (b_y == w_y) )
            {
                bullets[bullet_i].enable = 0;
                walls[wall_i].enable     = 0;
            }
        }
    }

    // “G <-> Ž©‹@
    for( enemy_i = 0; enemy_i < enemys_count; enemy_i++ )
    {
        if( enemys[enemy_i].enable == 0 ) continue;

        int e_x = enemys[enemy_i].x;
        int e_y = enemys[enemy_i].y;

        if( (e_x == player_x) && (e_y == player_y) )
        {
            // @todo
        }
    }

    // “G <-> •Ç
    for( enemy_i = 0; enemy_i < enemys_count; enemy_i++ )
    {
        if( enemys[enemy_i].enable == 0 ) continue;

        int e_x = enemys[enemy_i].x;
        int e_y = enemys[enemy_i].y;

        for( wall_i = 0; wall_i < walls_count; wall_i++ )
        {
            if( walls[wall_i].enable == 0 ) continue;

            int w_x = walls[wall_i].x;
            int w_y = walls[wall_i].y;

            if( (e_x == w_x) && (e_y == w_y) )
            {
                enemys[enemy_i].enable = 0;
                walls[wall_i].enable   = 0;
            }
        }
    }

}

int main()
{
    FILE *stage_file = fopen( "stage.dat", "rb" );
    
    fread( &stage_width,  sizeof(int), 1, stage_file );
    fread( &stage_height, sizeof(int), 1, stage_file );

    InitConsole( stage_width, stage_height );
    InitPlayer ( stage_file );
    InitEnemys ( stage_file );
    InitWalls  ( stage_file );
    InitBullets();

    fclose( stage_file );

    srand(time(NULL));

    while( 1 )
    {
        UpdateKeyStatus();
        ClrConsole();

        MovePlayer();
        MoveEnemys();
        MoveBullets();

        FirePlayer();
        FireEnemys();

        Collision();

        PrintWalls();
        PrintBullets();
        PrintPlayer();
        PrintEnemys();

        UpdateConsole();
        Sleep( 1000 / 50 );
    }

    return 0;
}


