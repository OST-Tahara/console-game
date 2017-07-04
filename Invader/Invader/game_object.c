/**
 * @file  game_object.c
 * @brief 
 */

#include "game_object.h"


int MoveObject( game_object_t *obj )
{
    if( obj->alive == 0 ) return 0;

    if( obj->move_interval > obj->move_count )
    {
        obj->move_count++;
        return 0;
    }

    if     ( obj->move_direct == direct_left  ) { obj->x--; obj->move_count = 0; } 
    else if( obj->move_direct == direct_right ) { obj->x++; obj->move_count = 0; } 
    else if( obj->move_direct == direct_up    ) { obj->y--; obj->move_count = 0; } 
    else if( obj->move_direct == direct_down  ) { obj->y++; obj->move_count = 0; } 
    else return 0;

    return 1;
}


int IsOutside( game_object_t *obj, int stage_width, int stage_height )
{
    if( obj->alive == 0 ) return 0;

    if( obj->x < 0                ) return OUTSIDE_LEFT;
    if( obj->x > (stage_width-1)  ) return OUTSIDE_RIGHT;
    if( obj->y < 0                ) return OUTSIDE_TOP;
    if( obj->y > (stage_height-1) ) return OUTSIDE_BOTTOM;

    return 0;
}


int IsEdge( game_object_t *obj, int stage_width, int stage_height )
{
    if( obj->alive == 0 ) return 0;

    if( obj->x == 0                ) return EDGE_LEFT;
    if( obj->x == (stage_width-1)  ) return EDGE_RIGHT;
    if( obj->y == 0                ) return EDGE_TOP;
    if( obj->y == (stage_height-1) ) return EDGE_BOTTOM;

    return 0;
}

