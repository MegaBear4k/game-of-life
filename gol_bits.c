/*
 * Game of Life - BITS Implementation
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gol_bits.h"


#define CALC_BITS_PER_3(X)   ((0x0E994 >> ((X) << 1)) & 0x03)


static int
CalculateNewCellState(BitsGame_t* Game_p,
                      const int   Column,
                      const int   Row);


void
BITS_InitializeWorld(BitsGame_t* Game_p,
                     const int   Width,
                     const int   Height)
{
    /*
     * The world is made one bit wider than the given Width and Height.
     * Hence, we make room for (1 + Width + 1) * ( 1 + Height + 1) bits.
     *
     * (UintInBits - 1) is for rounding up.
     */
    const int UintInBits = sizeof(uint_t) * 8;
    int WidthInUints  = (Width + 2 + (UintInBits - 1)) / UintInBits;  // Number of Uints per row
    int NumberOfUints = (Height + 2) * WidthInUints;

    printf("We need %d uints for Width. Total: %d uints.\n", WidthInUints, NumberOfUints);
    Game_p->NumberOfUintsPerRow = WidthInUints;
    Game_p->Width  = Width;
    Game_p->Height = Height;
    Game_p->CurrentWorld_p = malloc(NumberOfUints * sizeof(uint_t));
    Game_p->EvolvingWorld_p = malloc(NumberOfUints * sizeof(uint_t));
    memset(Game_p->CurrentWorld_p, 0, NumberOfUints * sizeof(uint_t));

    // Set a Glider pattern, ugly way...
    memset(Game_p->EvolvingWorld_p, 0, NumberOfUints * sizeof(uint_t));
    BITS_SetCellState(Game_p, 1, 2, 1);
    BITS_SetCellState(Game_p, 3, 1, 1);
    BITS_SetCellState(Game_p, 3, 2, 1);
    BITS_SetCellState(Game_p, 3, 3, 1);
    BITS_SetCellState(Game_p, 2, 3, 1);
    uint_t* Temp_p = Game_p->EvolvingWorld_p;
    Game_p->EvolvingWorld_p = Game_p->CurrentWorld_p;
    Game_p->CurrentWorld_p = Temp_p;

//    for (int i = 0; i <= 32; i++)
//    {
//        int n;
//        n = 1<<i;
//        printf("%2d => 0x%04x\n", i, n);
//    }
}


void
BITS_DestroyWorld(BitsGame_t* Game_p)
{
    free(Game_p->CurrentWorld_p);
    free(Game_p->EvolvingWorld_p);
}


void
BITS_SetCellState(BitsGame_t* Game_p,
                  const int   Column,
                  const int   Row,
                  const int   State)
{
    uint_t* Target_p;
    int UintPos = (1 + Column) / (sizeof(uint_t) * 8);
    int BitPos  = (1 + Column) % (sizeof(uint_t) * 8);
//    uint_t NewValue = (State & 0x01) << (32 - BitPos);

    /*
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))
number |= 1 << x;
number &= ~(1 << x);
bit = (number >> x) & 1;
     */

    Target_p = Game_p->EvolvingWorld_p + Game_p->NumberOfUintsPerRow * (1 + Row) + UintPos;
    if (State)
    {
        // Set Bit
        *Target_p |= 1<<(32 - BitPos - 1);
    }
    else
    {
        // Clear Bit
        *Target_p &= ~(1<<(32 - BitPos - 1));
    }
}


int
BITS_GetCellState(BitsGame_t* Game_p,
                  const int   Column,
                  const int   Row)
{
    uint_t* Target_p;
    int UintPos = (1 + Column) / (sizeof(uint_t) * 8);
    int BitPos  = (1 + Column) % (sizeof(uint_t) * 8);

    Target_p = Game_p->CurrentWorld_p + Game_p->NumberOfUintsPerRow * (1 + Row) + UintPos;
//    printf("Col=%d Row=%d => 0x%04x\n", Column, Row, *Target_p);
//    Target_p = Game_p->CurrentWorld_p + Game_p->NumberOfUintsPerRow * (1 + Column) + RowOffset;
//    printf("Col=%d Row=%d => 0x%08x ... BitPos=%d Checker=0x%08x Result=%d\n",
//            Column, Row, *Target_p,
//            BitPos,
//            1<<(32 - BitPos - 1),
//            (*Target_p & (1<<(32 - BitPos - 1))));

//#define BIT_CHECK(a,b) ((a) & (1<<(b)))

    return (*Target_p & (1<<(32 - BitPos - 1))) > 0;
//    return (*Target_p)>>(32 - BitPos);
}


void
BITS_EvolveWorld(BitsGame_t* Game_p)
{
    for (int Column = 0; Column < Game_p->Width; Column++)
    {
        for (int Row = 0; Row < Game_p->Height; Row++)
        {
            int NewCellState = CalculateNewCellState(Game_p, Column, Row);
            BITS_SetCellState(Game_p, Column, Row, NewCellState);
        }
    }

    uint_t* TempWorld_p = Game_p->CurrentWorld_p;
    Game_p->CurrentWorld_p = Game_p->EvolvingWorld_p;
    Game_p->EvolvingWorld_p = TempWorld_p;
}


int
BITS_GetWorldWidth(BitsGame_t* Game_p)
{
    return Game_p->Width;
}


int
BITS_GetWorldHeight(BitsGame_t* Game_p)
{
    return Game_p->Height;
}


static int
CalculateNewCellState(BitsGame_t* Game_p,
                      const int   Column,
                      const int   Row)
{
    int CurrentState = BITS_GetCellState(Game_p, Column, Row);
    int NewState = 0;
    int Neighbors = 0;

    int UintPos = (1 + Column) / (sizeof(uint_t) * 8);
    int BitPos  = (1 + Column) % (sizeof(uint_t) * 8);
    uint_t* CurrentUint_p = Game_p->CurrentWorld_p + Game_p->NumberOfUintsPerRow * (1 + Row) + UintPos;

    if ((BitPos == 1) && (UintPos > 0))
    {
        // First bit in uint_t (bit 32)
//        uint_t FirstBit = *(CurrentUint_p - 1) >> 28 & 0x04;
//        uint_t TheRest = (*CurrentUint_p>>30 &0x03) | FirstBit;

        uint_t UpperUint   = (((*(CurrentUint_p - 1 - Game_p->NumberOfUintsPerRow)>>28)&0x04) | (*(CurrentUint_p - Game_p->NumberOfUintsPerRow)>>30 & 0x03));
        uint_t CurrentUint = (((*(CurrentUint_p - 1)>>28)&0x04) | (*CurrentUint_p>>30 & 0x03));
        uint_t LowerUint   = (((*(CurrentUint_p - 1 + Game_p->NumberOfUintsPerRow)>>28)&0x04) | (*(CurrentUint_p + Game_p->NumberOfUintsPerRow)>>30 & 0x03));

        Neighbors += CALC_BITS_PER_3(UpperUint & 0x07);
        Neighbors += CALC_BITS_PER_3(CurrentUint & 0x05);
        Neighbors += CALC_BITS_PER_3(LowerUint & 0x07);
    }
    else if (BitPos == 0 && (UintPos != Game_p->NumberOfUintsPerRow))
    {
        // Last bit in uint_t (bit 1)
//        uint_t NextBit = (*(CurrentUint_p + 1)>>31)&0x01;
//        uint_t TheRest = ((*CurrentUint_p)<<1)&0x06 | NextBit;

        uint_t UpperUint   = (((*(CurrentUint_p + 1 - Game_p->NumberOfUintsPerRow)>>31)&0x01) | (*(CurrentUint_p - Game_p->NumberOfUintsPerRow)<<1 & 0x06));
        uint_t CurrentUint = (((*(CurrentUint_p + 1)>>31)&0x01) | (*CurrentUint_p<<1 & 0x06));
        uint_t LowerUint   = (((*(CurrentUint_p + 1 + Game_p->NumberOfUintsPerRow)>>31)&0x01) | (*(CurrentUint_p + Game_p->NumberOfUintsPerRow)<<1 & 0x06));

        Neighbors += CALC_BITS_PER_3(UpperUint & 0x07);
        Neighbors += CALC_BITS_PER_3(CurrentUint & 0x05);
        Neighbors += CALC_BITS_PER_3(LowerUint & 0x07);
    }
    else
    {
        // Somewhere in the middle...
        int Shift = sizeof(uint_t) * 8 - 1 - BitPos - 1;
        Neighbors += CALC_BITS_PER_3((*(CurrentUint_p - Game_p->NumberOfUintsPerRow) >> Shift) & 0x07);
        Neighbors += CALC_BITS_PER_3((*(CurrentUint_p) >> Shift) & 0x05);
        Neighbors += CALC_BITS_PER_3((*(CurrentUint_p + Game_p->NumberOfUintsPerRow) >> Shift) & 0x07);
    }

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
