/*
 * Game of Life - ARRAY Implementation
 *
 */

#include <string.h>
#include <stdlib.h>

#include "gol_array.h"


//#define POS_OFFSET(Column, Row, Width)   (((Row) * ((Width) + 2)) + (1 + (Column)))

#define POS_OFFSET(Column, Row, Width)   (((1 + (Row)) * ((Width) + 2)) + (1 + (Column)))
//int Pos = (1 + Row) * (Game_p->Width + 2) + (1 + Column);


static int
CalculateNewCellState(ArrayGame_t* Game_p,
                      const int    Column,
                      const int    Row);

void
ARRAY_InitializeWorld(ArrayGame_t* Game_p,
                      const int    Width,
                      const int    Height)
{
    int NumberOfBytes = (Width + 2) * (Height + 2);
    Game_p->CurrentWorld_p  = malloc(NumberOfBytes);
    Game_p->EvolvingWorld_p = malloc(NumberOfBytes);
    Game_p->Width  = Width;
    Game_p->Height = Height;

    memset(Game_p->CurrentWorld_p, 0, NumberOfBytes);
    /* pattern "glider" */
    *(Game_p->CurrentWorld_p + POS_OFFSET(1, 2, Width)) = 1;
    *(Game_p->CurrentWorld_p + POS_OFFSET(3, 1, Width)) = 1;
    *(Game_p->CurrentWorld_p + POS_OFFSET(3, 2, Width)) = 1;
    *(Game_p->CurrentWorld_p + POS_OFFSET(3, 3, Width)) = 1;
    *(Game_p->CurrentWorld_p + POS_OFFSET(2, 3, Width)) = 1;
}


void
ARRAY_DestroyWorld(ArrayGame_t* Game_p)
{
    free(Game_p->CurrentWorld_p);
    free(Game_p->EvolvingWorld_p);
}


void
ARRAY_SetCellState(ArrayGame_t* Game_p,
                   const int    Column,
                   const int    Row,
                   const int    State)
{
    int Pos = (1 + Row) * (Game_p->Width + 2) + (1 + Column);
    *(Game_p->EvolvingWorld_p + Pos) = State;
}


int
ARRAY_GetCellState(ArrayGame_t* Game_p,
                   const int    Column,
                   const int    Row)
{
    int Pos = (1 + Row) * (Game_p->Width + 2) + (1 + Column);
    return *(Game_p->CurrentWorld_p + Pos);
}


void
ARRAY_EvolveWorld(ArrayGame_t* Game_p)
{
    for (int Column = 0; Column < Game_p->Width; Column++)
    {
        for (int Row = 0; Row < Game_p->Height; Row++)
        {
            int NewCellState = CalculateNewCellState(Game_p, Column, Row);
            ARRAY_SetCellState(Game_p, Column, Row, NewCellState);
        }
    }

    byte_t* TempWorld_p = Game_p->CurrentWorld_p;
    Game_p->CurrentWorld_p = Game_p->EvolvingWorld_p;
    Game_p->EvolvingWorld_p = TempWorld_p;
}


int
ARRAY_GetWorldWidth(ArrayGame_t* Game_p)
{
    return Game_p->Width;
}


int
ARRAY_GetWorldHeight(ArrayGame_t* Game_p)
{
    return Game_p->Height;
}


static int
CalculateNewCellState(ArrayGame_t* Game_p,
                      const int    Column,
                      const int    Row)
{

//    int CurrentState = ARRAY_GetCellState(Game_p, Column, Row);
    int CurrentState = *(Game_p->CurrentWorld_p + POS_OFFSET(Column, Row, Game_p->Width));
    int NewState = 0;
    int Neighbors;

    Neighbors = *(Game_p->CurrentWorld_p + POS_OFFSET(Column - 1, Row - 1, Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column,     Row - 1, Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column + 1, Row - 1, Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column - 1, Row,     Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column + 1, Row,     Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column - 1, Row + 1, Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column,     Row + 1, Game_p->Width)) +
                *(Game_p->CurrentWorld_p + POS_OFFSET(Column + 1, Row + 1, Game_p->Width));

    if (CurrentState)
    {
        // Cell is ALIVE
        NewState = (Neighbors == 2 || Neighbors == 3);
    }
    else
    {
        // Cell is DEAD
        NewState = (Neighbors == 3);
    }
    return NewState;
}
