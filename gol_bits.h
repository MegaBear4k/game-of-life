/*
 * Game of Life - BITS Variant
 *
 */

#ifndef GOL_BITS_H_
#define GOL_BITS_H_


typedef unsigned int uint_t;


typedef struct
{
    int     Width;
    int     Height;
    int     NumberOfUintsPerRow;
    uint_t* CurrentWorld_p;
    uint_t* EvolvingWorld_p;
} BitsGame_t;


void
BITS_InitializeWorld(BitsGame_t* Game_p,
                     const int   Width,
                     const int   Height);


void
BITS_DestroyWorld(BitsGame_t* Game_p);


void
BITS_SetCellState(BitsGame_t* Game_p,
                  const int   Column,
                  const int   Row,
                  const int   State);


void
BITS_SetCellStateInCurrent(BitsGame_t* Game_p,
                           const int   Column,
                           const int   Row,
                           const int   State);


int
BITS_GetCellState(BitsGame_t* Game_p,
                  const int   Column,
                  const int   Row);


void
BITS_EvolveWorld(BitsGame_t* Game_p);


int
BITS_GetWorldWidth(BitsGame_t* Game_p);


int
BITS_GetWorldHeight(BitsGame_t* Game_p);



#endif // GOL_BITS_H_
