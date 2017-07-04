/**
 * @file  main.c
 */

/**
 * @mainpage シンプルなインベーダゲーム
 *
 * @par ゲーム内容
 * 普通のインベーダゲーム
 * [SPACE] 弾の発射
 * [←→]  移動
 * 
 * @par コンセプト
 * C言語初心者の理解の助けになるようなプログラム作成。
 * できるだけシンプルに！
 * だけども改修し易い構造に!
 *
 * @par ステージの管理
 * 外部ファイル[stage.dat]にステージのデータを保持
 *   - ステージのサイズ
 *   - プレイヤーの初期位置
 *   - 敵の情報
 *   - 壁の情報
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "game_object.h"
#include "Utility.h"


static int stage_width  = 0;   ///< ステージの幅
static int stage_height = 0;   ///< ステージの高さ

/**
 * @enum  game_status_t
 * @brief ゲームの動作状態
 */
typedef enum
{
    game_status_title = 0,   ///< タイトル画面
    game_status_playing,     ///< プレイ画面
    game_status_gameover     ///< ゲームオーバー画面
} game_status_t;

typedef struct
{
    int count;
    game_object_t *list;
} object_list_t;


//---------------------------------------------------------
// 弾
//---------------------------------------------------------

/**
 * @brief 弾の初期化
 */
static void InitBullets( object_list_t *list, int max )
{
    list->count = max;
    list->list  = malloc( sizeof(game_object_t) * list->count );

    int i;
    for( i = 0; i < list->count; i++ )
    {
        list->list[i].alive = 0;
    }
}


/**
 * @brief
 */
static void AddBullet( object_list_t *list, int x, int y, direct_t move_direct )
{
    int i;
    for( i = 0; i < list->count; i++ )
    {
        if( list->list[i].alive ) continue;

        list->list[i].alive = 1;
        list->list[i].x     = x;
        list->list[i].y     = y;
        list->list[i].move_direct   = move_direct;
        list->list[i].move_interval = 5;
        list->list[i].move_count    = 0;
        break;
    }
}


static void MoveBullets( object_list_t *list )
{
    int i;
    for( i = 0; i < list->count; i++ )
    {
        if( list->list[i].alive == 0 ) continue;

        MoveObject( &list->list[i] );

        if( IsOutside( &list->list[i], stage_width, stage_height ) )
        {
            list->list[i].alive = 0;
        }
    }
}


static void PrintBullets( object_list_t *list )
{
    int i;
    for( i = 0; i < list->count; i++ )
    {
        if( list->list[i].alive == 0 ) continue;

        PutCharConsole( list->list[i].x, list->list[i].y, '|' );
    }
}


//---------------------------------------------------------
// プレイヤー
//---------------------------------------------------------

static void InitPlayer( FILE *file, object_list_t *list ) 
{
    list->count                 = 1;
    list->list                  = malloc( sizeof(game_object_t) * list->count );
    list->list[0].alive         = 1;
    list->list[0].move_direct   = direct_none;
    list->list[0].move_count    = 0;
    list->list[0].move_interval = 5;

    fread( &list->list[0].x, sizeof(int), 1, file );
    fread( &list->list[0].y, sizeof(int), 1, file );
}


static void MovePlayer( object_list_t *list, int key_left, int key_right )
{
    list->list[0].move_direct = direct_none;
    if( (key_left  == InputStatePush) || (key_left  == InputStatePushing) ) list->list[0].move_direct = direct_left;
    if( (key_right == InputStatePush) || (key_right == InputStatePushing) ) list->list[0].move_direct = direct_right;

    MoveObject( &list->list[0] );

    int outside = IsOutside( &list->list[0], stage_width, stage_height );
    if     ( outside == OUTSIDE_LEFT  ) list->list[0].x = 0;
    else if( outside == OUTSIDE_RIGHT ) list->list[0].x = stage_width - 1;
}


static void FirePlayer( object_list_t *list_player, int key_space, object_list_t *list_bullet )
{
    if( key_space == InputStatePush )
    {
        AddBullet( list_bullet, list_player->list[0].x,list_player->list[0].y - 1, direct_up );
    }
}


static void PrintPlayer( object_list_t *list_player )
{
    PutCharConsole( list_player->list[0].x, list_player->list[0].y, '@' );
}


//---------------------------------------------------------
// 敵
//---------------------------------------------------------

static direct_t enemy_move_direct   = direct_none;
static int      enemy_move_interval = 0;
static int      enemy_move_count    = 0;

static void InitEnemys( FILE *file, object_list_t *list )
{
    fread( &list->count, sizeof(int), 1, file );
    list->list = malloc( sizeof(game_object_t) * list->count );

    int i;
    for( i = 0; i < list->count; i++ )
    {
        list->list[i].alive = 1;
        fread( &list->list[i].x, sizeof(int), 1, file );
        fread( &list->list[i].y, sizeof(int), 1, file );
    }

    enemy_move_direct   = direct_right;
    enemy_move_interval = 10;
    enemy_move_count    = 0;
}


static void MoveEnemys( object_list_t *list )
{
    // 移動
    int i, move_i = 0;
    for( i = 0; i < list->count; i++ )
    {
        list->list[i].move_direct   = enemy_move_direct;
        list->list[i].move_interval = enemy_move_interval;
        list->list[i].move_count    = enemy_move_count;

        if( MoveObject( &list->list[i] ) ) move_i = i;
    }

    enemy_move_direct   = list->list[move_i].move_direct;
    enemy_move_interval = list->list[move_i].move_interval;
    enemy_move_count    = list->list[move_i].move_count;

    // 端検索
    int edge = 0;
    for( i = 0; i < list->count; i++ )
    {
        edge = IsEdge( &list->list[i], stage_width, stage_height );
        if( (edge & EDGE_LEFT) || (edge & EDGE_RIGHT) )
        {
            if( enemy_move_direct == direct_down )
            {
                if( edge & EDGE_LEFT ) enemy_move_direct = direct_right;
                else                   enemy_move_direct = direct_left;
            }
            else
            {
                enemy_move_direct = direct_down;
            }

            break;
        }
    }
}


static void FireEnemys( object_list_t *list_player, object_list_t *list_bullet )
{
    int i;
    for( i = 0; i < list_player->count; i++ )
    {
        if( list_player->list[i].alive == 0 ) continue;

        if( rand() % 1000 == 0 )
        {
            AddBullet( list_bullet, list_player->list[i].x, list_player->list[i].y+1, direct_down );
        }
    }
}


static void PrintEnemys( object_list_t *list )
{
    int i;
    for( i = 0; i < list->count; i++ )
    {
        if( list->list[i].alive == 0 ) continue;

        PutCharConsole( list->list[i].x, list->list[i].y, 'Q' );
    }
}


//---------------------------------------------------------
// 壁
//---------------------------------------------------------

static void InitWalls( FILE *file, object_list_t *list )
{
    fread( &list->count, sizeof(int), 1, file );
    list->list = malloc( sizeof(game_object_t) * list->count );

    int i;
    for( i = 0; i < list->count; i++ )
    {
        list->list[i].alive = 1;
        fread( &list->list[i].x, sizeof(int), 1, file );
        fread( &list->list[i].y, sizeof(int), 1, file );
    }
}


static void PrintWalls( object_list_t *list )
{
    int i;
    for( i = 0; i < list->count; i++ )
    {
        if( list->list[i].alive == 0 ) continue;

        PutCharConsole( list->list[i].x, list->list[i].y, '#' );
    }
}


//---------------------------------------------------------
// ゲーム制御
//---------------------------------------------------------

static object_list_t list_bullet_enemy;
static object_list_t list_bullet_player;
static object_list_t list_player;
static object_list_t list_enemy;
static object_list_t list_wall;

static void Collision( object_list_t *obj1, object_list_t *obj2 )
{
    int i, j;

    for( i = 0; i < obj1->count; i++ )
    {
        if( obj1->list[i].alive == 0 ) continue;

        for( j = 0; j < obj2->count; j++ )
        {
            if( obj2->list[j].alive == 0 ) continue;

            if( (obj1->list[i].x == obj2->list[j].x) && (obj1->list[i].y == obj2->list[j].y) )
            {
                obj1->list[i].alive = 0;
                obj2->list[j].alive = 0;
            }
        }
    }
}


int main()
{
    FILE *stage_file = fopen( "stage.dat", "rb" );
    // @todo ファイルオープン失敗処理
    
    fread( &stage_width,  sizeof(int), 1, stage_file );
    fread( &stage_height, sizeof(int), 1, stage_file );

    InitConsole( stage_width, stage_height );
    InitPlayer ( stage_file, &list_player  );
    InitEnemys ( stage_file, &list_enemy   );
    InitWalls  ( stage_file, &list_wall    );
    InitBullets( &list_bullet_player, 100  );
    InitBullets( &list_bullet_enemy,  100  );

    fclose( stage_file );

    srand( (unsigned int)time(NULL) );

    while( 1 )
    {
        UpdateKeyStatus();
        ClrConsole();

        MovePlayer ( &list_player, GetKeyStatus(VK_LEFT), GetKeyStatus(VK_RIGHT) );
        MoveEnemys ( &list_enemy         );
        MoveBullets( &list_bullet_enemy  );
        MoveBullets( &list_bullet_player );

        FirePlayer( &list_player, GetKeyStatus(VK_SPACE), &list_bullet_player );
        FireEnemys( &list_enemy, &list_bullet_enemy );

        Collision( &list_enemy,         &list_bullet_player );
        Collision( &list_enemy,         &list_wall          );
        Collision( &list_wall,          &list_bullet_enemy  );
        Collision( &list_wall,          &list_bullet_player );
        Collision( &list_bullet_player, &list_bullet_enemy  );
        Collision( &list_player,        &list_bullet_enemy  );
        Collision( &list_enemy,         &list_player        );

        PrintWalls  ( &list_wall          );
        PrintBullets( &list_bullet_player );
        PrintBullets( &list_bullet_enemy  );
        PrintPlayer ( &list_player        );
        PrintEnemys ( &list_enemy         );

        UpdateConsole();
        Sleep( 1000 / 50 );
    }

    return 0;
}


