/*
 * Game of Life - ARRAY Variant
 *
 */

#ifndef GOL_ARRAY_H_
#define GOL_ARRAY_H_


typedef unsigned char byte_t;


typedef struct
{
    int     Width;
    int     Height;
    byte_t* CurrentWorld_p;
    byte_t* EvolvingWorld_p;
} ArrayGame_t;


void
ARRAY_InitializeWorld(ArrayGame_t* Game_p,
                      const int    Width,
                      const int    Height);


void
ARRAY_DestroyWorld(ArrayGame_t* Game_p);


void
ARRAY_SetCellState(ArrayGame_t* Game_p,
                   const int    Column,
                   const int    Row,
                   const int    State);


void
ARRAY_SetCellStateInCurrent(ArrayGame_t* Game_p,
                            const int    Column,
                            const int    Row,
                            const int    State);


int
ARRAY_GetCellState(ArrayGame_t* Game_p,
                   const int    Column,
                   const int    Row);


void
ARRAY_EvolveWorld(ArrayGame_t* Game_p);


int
ARRAY_GetWorldWidth(ArrayGame_t* Game_p);


int
ARRAY_GetWorldHeight(ArrayGame_t* Game_p);



#endif // GOL_ARRAY_H_
