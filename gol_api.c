/*
 * Game of Life - API Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "gol_api.h"
#include "gol_ref.h"
#include "gol_array.h"
#include "gol_bits.h"


/* character representations of cell states */
#define CHAR_ALIVE '*'
#define CHAR_DEAD ' '


typedef struct
{
    GOL_Variant_t Variant;
    union
    {
        RefGame_t   RefGame;
        ArrayGame_t ArrayGame;
        BitsGame_t  BitsGame;
    } Data;
} GameOfLife_t;


static int
GetCellState(const GOL_Game_t Game, const int Column, const int Row);

static void
SetCellStateInCurrent(const GOL_Game_t Game, const int Column, const int Row, const int State);


GOL_Game_t
GOL_InitializeWorld(const GOL_Variant_t Variant,
                    const int           Width,
                    const int           Height,
                    const int           UseDefaultPattern)
{
    GameOfLife_t* Game_p = NULL;
    char* VariantName_p = NULL;

    switch (Variant)
    {
    case GOL_VARIANT_REFERENCE:
        Game_p = malloc(sizeof(GameOfLife_t));
        Game_p->Variant = GOL_VARIANT_REFERENCE;
        initialize_world(&Game_p->Data.RefGame);
        VariantName_p = "REFERENCE";
        break;

    case GOL_VARIANT_ARRAY:
        Game_p = malloc(sizeof(GameOfLife_t));
        Game_p->Variant = GOL_VARIANT_ARRAY;
        ARRAY_InitializeWorld(&Game_p->Data.ArrayGame, Width, Height);
        VariantName_p = "ARRAY";
        break;

    case GOL_VARIANT_BITS:
        Game_p = malloc(sizeof(GameOfLife_t));
        Game_p->Variant = GOL_VARIANT_BITS;
        BITS_InitializeWorld(&Game_p->Data.BitsGame, Width, Height);
        VariantName_p = "BITS";
        break;

    default:
        printf("Invalid implementation variant: %d\n", Variant);
    }

    if (Game_p != NULL)
    {
        printf("Initialized **%s** world\n", VariantName_p);

        if (UseDefaultPattern)
        {
            // Insert the Pattern "Glider"
            SetCellStateInCurrent(Game_p, 1, 2, ALIVE);
            SetCellStateInCurrent(Game_p, 3, 1, ALIVE);
            SetCellStateInCurrent(Game_p, 3, 2, ALIVE);
            SetCellStateInCurrent(Game_p, 3, 3, ALIVE);
            SetCellStateInCurrent(Game_p, 2, 3, ALIVE);
        }
    }

    return Game_p;
}


GOL_Game_t
GOL_InitializeWorldFromFile(const GOL_Variant_t Variant,
                            const int           Width,
                            const int           Height,
                            const char* const   Filename_p)
{
    GameOfLife_t* Game_p = NULL;

    Game_p = GOL_InitializeWorld(Variant, Width, Height, 0);
    if (Game_p != NULL)
    {
        FILE * pfile;
        int i = 0, j = 0, len;
        char strread[256];

        if ((pfile = fopen(Filename_p, "r")) == NULL) {
                fprintf(stderr,"Error: unable to read \"%s\" (error #%d).\n",
                        Filename_p, errno);
                abort();
        }

        while (j < Height && !feof(pfile) &&
                        fgets(strread, 256, pfile) != NULL) {
                /* jth line read */
                len = strlen(strread);
                if (len > Width)
                        len = Width; /* ignore extra chars */
                for (i = 0; i < len; i++)
                    SetCellStateInCurrent(Game_p, i, j, strread[i] == CHAR_ALIVE ? ALIVE : DEAD);
                j++; /* next line */
        }
        fclose(pfile);

    }
    return Game_p;
}


void
GOL_DestroyWorld(GOL_Game_t* Game_p)
{
    GameOfLife_t** Game_pp = (GameOfLife_t**)Game_p;
    switch ((*Game_pp)->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        // No action
        break;

    case GOL_VARIANT_ARRAY:
        ARRAY_DestroyWorld(&(*Game_pp)->Data.ArrayGame);
        break;

    case GOL_VARIANT_BITS:
        BITS_DestroyWorld(&(*Game_pp)->Data.BitsGame);
        break;

    default:
        printf("Invalid implementation variant: %d\n", (*Game_pp)->Variant);
    }

    free(*Game_pp);
    *Game_pp = NULL;
}


void
GOL_EvolveWorld(const GOL_Game_t Game)
{
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;
    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        next_generation(&Game_p->Data.RefGame);
        break;

    case GOL_VARIANT_ARRAY:
        ARRAY_EvolveWorld(&Game_p->Data.ArrayGame);
        break;

    case GOL_VARIANT_BITS:
        BITS_EvolveWorld(&Game_p->Data.BitsGame);
        break;

    default:
        printf("Invalid implementation variant: %d\n", Game_p->Variant);
    }
}


void
GOL_CompareWorlds(const GOL_Game_t Game1, const GOL_Game_t Game2)
{
    int Game1_Width;
    int Game1_Height;
    int Game2_Width;
    int Game2_Height;
    int MaxWidth;
    int MaxHeight;

    Game1_Width = GOL_GetWorldWidth(Game1);
    Game2_Width = GOL_GetWorldWidth(Game2);

    Game1_Height = GOL_GetWorldHeight(Game1);
    Game2_Height = GOL_GetWorldHeight(Game2);

    MaxWidth = (Game1_Width > Game2_Width) ? Game1_Width : Game2_Width;
    MaxHeight = (Game1_Height > Game2_Height) ? Game1_Height : Game2_Height;

    for (int x = 0; x < MaxWidth; x++)
    {
        for (int y = 0; y < MaxHeight; y++)
        {
            if (GetCellState(Game1, x, y) != GetCellState(Game2, x, y))
            {
                printf("ERROR! Worlds are not equal! X=%d Y= %d\n", x, y);
                printf("GAME1\n");
                GOL_OutputWorld(Game1);
                printf("GAME2\n");
                GOL_OutputWorld(Game2);
                exit(-1);
            }
        }
    }
}


void
GOL_OutputWorld(const GOL_Game_t Game)
{
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;
    int Width;
    int Height;

    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        Width = get_world_width();
        Height = get_world_height();
        break;

    case GOL_VARIANT_ARRAY:
        Width  = Game_p->Data.ArrayGame.Width;
        Height = Game_p->Data.ArrayGame.Height;
        break;

    case GOL_VARIANT_BITS:
        Width  = Game_p->Data.BitsGame.Width;
        Height = Game_p->Data.BitsGame.Height;
        break;

    default:
        printf("Invalid implementation variant: %d\n", Game_p->Variant);
        return;
    }

    {
//        char worldstr[2*WORLDWIDTH+2];
        char* worldstr = malloc(2*Width+2);
        int i, j;

        worldstr[2*Width+1] = '\0';
        worldstr[0] = '+';
        for (i = 1; i < 2*Width; i++)
            worldstr[i] = '-';
        worldstr[2*Width] = '+';
        puts(worldstr);
        for (i = 0; i <= 2*Width; i+=2)
            worldstr[i] = '|';
        for (i = 0; i < Height; i++) {
            for (j = 0; j < Width; j++)
                worldstr[2*j+1] = GetCellState(Game_p, j, i) == ALIVE ? CHAR_ALIVE : CHAR_DEAD;
            puts(worldstr);
        }
        worldstr[0] = '+';
        for (i = 1; i < 2*Width; i++)
            worldstr[i] = '-';
        worldstr[2*Width] = '+';
        puts(worldstr);
        free(worldstr);
    }
}


void
GOL_SaveWorldToFile(const GOL_Game_t Game, const char* const Filename_p)
{
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;
    int Width;
    int Height;

    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        Width = get_world_width();
        Height = get_world_height();
        break;

    case GOL_VARIANT_ARRAY:
        Width  = Game_p->Data.ArrayGame.Width;
        Height = Game_p->Data.ArrayGame.Height;
        break;

    case GOL_VARIANT_BITS:
        Width  = Game_p->Data.BitsGame.Width;
        Height = Game_p->Data.BitsGame.Height;
        break;

    default:
        printf("Invalid implementation variant: %d\n", Game_p->Variant);
        return;
    }

    FILE * pfile;
    int i, j;
    char* strwrite = malloc(Width + 1);

    if ((pfile = fopen(Filename_p, "w")) == NULL) {
        fprintf(stderr,"Error: unable to open \"%s\" for writing (error #%d).\n",
                Filename_p, errno);
        abort();
    }

    strwrite[Width] = '\0'; /* null terminator */
    for (j = 0; j < Height; j++) {
        for (i = 0; i < Width; i++)
            strwrite[i] = GetCellState(Game, i, j) == ALIVE ? CHAR_ALIVE : CHAR_DEAD;
        fprintf(pfile,"%s\n",strwrite);
    }

    free(strwrite);
    fclose(pfile);

}


static int
GetCellState(const GOL_Game_t Game, const int Column, const int Row)
{
    int State = CELL_DEAD;
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;

    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        State = get_cell_state(&Game_p->Data.RefGame, Column, Row);
        break;

    case GOL_VARIANT_ARRAY:
        State = ARRAY_GetCellState(&Game_p->Data.ArrayGame, Column, Row);
        break;

    case GOL_VARIANT_BITS:
        State = BITS_GetCellState(&Game_p->Data.BitsGame, Column, Row);
        break;

    default:
        break;
    }
    return State;
}


// Sets a cell state in the current world. Used for loading files only.
static void
SetCellStateInCurrent(const GOL_Game_t Game, const int Column, const int Row, const int State)
{
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;

    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        set_cell_state_in_current(&Game_p->Data.RefGame, Column, Row, State);
        break;

    case GOL_VARIANT_ARRAY:
        ARRAY_SetCellStateInCurrent(&Game_p->Data.ArrayGame, Column, Row, State);
        break;

    case GOL_VARIANT_BITS:
        BITS_SetCellStateInCurrent(&Game_p->Data.BitsGame, Column, Row, State);
        break;

    default:
        break;
    }
}


// XXX: Expose in the API (or not?)
int
GOL_GetWorldWidth(const GOL_Game_t Game)
{
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;
    int Width = 0;
    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        Width = get_world_width();
        break;

    case GOL_VARIANT_ARRAY:
        Width = ARRAY_GetWorldWidth(&Game_p->Data.ArrayGame);
        break;

    case GOL_VARIANT_BITS:
        Width = BITS_GetWorldWidth(&Game_p->Data.BitsGame);
        break;

    default:
        break;
    }
    return Width;
}


// XXX: Expose in the API (or not?)
int
GOL_GetWorldHeight(const GOL_Game_t Game)
{
    GameOfLife_t* Game_p = (GameOfLife_t*)Game;
    int Height = 0;
    switch (Game_p->Variant)
    {
    case GOL_VARIANT_REFERENCE:
        Height = get_world_height();
        break;

    case GOL_VARIANT_ARRAY:
        Height = ARRAY_GetWorldHeight(&Game_p->Data.ArrayGame);
        break;

    case GOL_VARIANT_BITS:
        Height = BITS_GetWorldHeight(&Game_p->Data.BitsGame);
        break;

    default:
        break;
    }
    return Height;
}
