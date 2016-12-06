/*
 * Game of Life - BITS Implementation
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gol_bits.h"

//#define ENABLE_VERBOSE_LOGGING

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
        *Target_p |= 1<<BitPos;
    }
    else
    {
        *Target_p &= ~(1<<BitPos);
    }
}


void
BITS_SetCellStateInCurrent(BitsGame_t* Game_p,
                           const int   Column,
                           const int   Row,
                           const int   State)
{
    uint_t* Target_p;
    int UintPos = (1 + Column) / (sizeof(uint_t) * 8);
    int BitPos  = (1 + Column) % (sizeof(uint_t) * 8);

    Target_p = Game_p->CurrentWorld_p + Game_p->NumberOfUintsPerRow * (1 + Row) + UintPos;
    if (State)
    {
        // Set Bit
        *Target_p |= 1<<BitPos;
    }
    else
    {
        *Target_p &= ~(1<<BitPos);
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

//#define BIT_CHECK(a,b) ((a) & (1<<(b)))

    if (*Target_p & (1<<BitPos))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void
BITS_EvolveWorld(BitsGame_t* Game_p)
{
#ifdef ENABLE_VERBOSE_LOGGING
    printf("\n-------------------- NEIGHBORS...\n");
#endif
    for (int Row = 0; Row < Game_p->Height; Row++)
    {
#ifdef ENABLE_VERBOSE_LOGGING
        printf("|");
#endif
        for (int Column = 0; Column < Game_p->Width; Column++)
        {
            int NewCellState = CalculateNewCellState(Game_p, Column, Row);
            BITS_SetCellState(Game_p, Column, Row, NewCellState);
        }
#ifdef ENABLE_VERBOSE_LOGGING
        printf("\n");
#endif
    }
#ifdef ENABLE_VERBOSE_LOGGING
    printf("\n--------------------\n");
#endif

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
    const int UintInBits = sizeof(uint_t) * 8;

    int CurrentState = BITS_GetCellState(Game_p, Column, Row);
    int NewState = 0;
    int Neighbors = 0;

    int UintPos = (1 + Column) / UintInBits;
    int BitPos  = (1 + Column) % UintInBits;
    uint_t* CurrentUint_p = Game_p->CurrentWorld_p + Game_p->NumberOfUintsPerRow * (1 + Row) + UintPos;
    uint_t* PrevRowUint_p = CurrentUint_p - Game_p->NumberOfUintsPerRow;
    uint_t* NextRowUint_p = CurrentUint_p + Game_p->NumberOfUintsPerRow;

    if ((BitPos == 0) && (UintPos > 0))
    {
        // First bit in uint_t (bit 0)
        // We take:
        //   * The two lower bits from CurrentUint_p
        //   * The one highest bit from CurrentUint_p - 1
        uint_t UpperUint   = ((*(PrevRowUint_p - 1)>>31)&0x01) | ((*PrevRowUint_p & 0x03)<<1);
        uint_t CurrentUint = ((*(CurrentUint_p - 1)>>31)&0x01) | ((*CurrentUint_p & 0x03)<<1);
        uint_t LowerUint   = ((*(NextRowUint_p - 1)>>31)&0x01) | ((*NextRowUint_p & 0x03)<<1);

        Neighbors += CALC_BITS_PER_3(UpperUint & 0x07);
        Neighbors += CALC_BITS_PER_3(CurrentUint & 0x05);
        Neighbors += CALC_BITS_PER_3(LowerUint & 0x07);
    }
    else if (BitPos == 31 && (UintPos != Game_p->NumberOfUintsPerRow))
    {
        // Last bit in uint_t (bit 31)
        // We take:
        //   * The two upper bits from CurrentUint_p (bit 30, 31)
        //   * The one lowest bit from CurrentUint_p + 1 (bit 0)
        uint_t UpperUint   = ((*(PrevRowUint_p + 1)&0x01)<<2) | ((*PrevRowUint_p>>30) & 0x03);
        uint_t CurrentUint = ((*(CurrentUint_p + 1)&0x01)<<2) | ((*CurrentUint_p>>30) & 0x03);
        uint_t LowerUint   = ((*(NextRowUint_p + 1)&0x01)<<2) | ((*NextRowUint_p>>30) & 0x03);

        Neighbors += CALC_BITS_PER_3(UpperUint & 0x07);
        Neighbors += CALC_BITS_PER_3(CurrentUint & 0x05);
        Neighbors += CALC_BITS_PER_3(LowerUint & 0x07);
    }
    else
    {
        // Somewhere in the middle...
        int Shift = BitPos - 1;
        Neighbors += CALC_BITS_PER_3((*(PrevRowUint_p) >> Shift) & 0x07);
        Neighbors += CALC_BITS_PER_3((*(CurrentUint_p) >> Shift) & 0x05);
        Neighbors += CALC_BITS_PER_3((*(NextRowUint_p) >> Shift) & 0x07);
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
#ifdef ENABLE_VERBOSE_LOGGING
    printf("%d|", Neighbors);
#endif
    return NewState;
}
