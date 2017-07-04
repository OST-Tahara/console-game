/**
 * @file  game_object.h
 * @brief �Q�[���̃I�u�W�F�N�g���ʏ����E��`
 */


#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H


/**
 * @enum  direct_t
 * @brief �ړ�����
 */
typedef enum
{
    direct_none = 0,
    direct_left,       ///< ��
    direct_right,      ///< �E
    direct_up,         ///< ��
    direct_down        ///< ��
} direct_t;


typedef struct
{
    int alive;
    int x;
    int y;

    direct_t move_direct;
    int      move_interval;
    int      move_count;
} game_object_t;


#define OUTSIDE_LEFT   (0x1)
#define OUTSIDE_RIGHT  (0x2)
#define OUTSIDE_TOP    (0x4)
#define OUTSIDE_BOTTOM (0x8)

#define EDGE_LEFT      (0x1)
#define EDGE_RIGHT     (0x2)
#define EDGE_TOP       (0x4)
#define EDGE_BOTTOM    (0x8)


int MoveObject( game_object_t *obj );
int IsOutside( game_object_t *obj, int stage_width, int stage_height );
int IsEdge( game_object_t *obj, int stage_width, int stage_height );


#endif