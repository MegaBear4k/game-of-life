/*
 * Game of Life - MAIN file
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gol_api.h"


typedef enum
{
    GOL_DISPLAY_NONE,
    GOL_DISPLAY_ANIMATE,
    GOL_DISPLAY_SHOW_FINAL,

    GOL_DISPLAY_LAST_ENTRY
} GOL_Display_t;


int
main(int argc, char* argv[])
{
    GOL_Variant_t Variant = GOL_VARIANT_REFERENCE;
    GOL_Display_t Display = GOL_DISPLAY_ANIMATE;
    int Width             = DEFAULT_WORLD_WIDTH;
    int Height            = DEFAULT_WORLD_HEIGHT;
    int NumGenerations    = DEFAULT_NUM_GENERATIONS;
    char* Filename_p      = NULL;
    int DoCompare         = 0;
    int Success           = 1;

    if (argc % 2 == 0)
    {
        // We expect an uneven number of arguments
        Success = 0;
    }

    if ((argc > 1) && Success)
    {
        for (int i = 1; i < argc; i += 2)
        {
            char* Option_p = argv[i];
            char* Value_p  = argv[i + 1];

            if (!strcmp(Option_p, "--width"))
            {
                Width = atoi(Value_p);
            }
            else if (!strcmp(Option_p, "--height"))
            {
                Height = atoi(Value_p);
            }
            else if (!strcmp(Option_p, "--count"))
            {
                NumGenerations = atoi(Value_p);
            }
            else if (!strcmp(Option_p, "--file"))
            {
                Filename_p = Value_p;
            }
            else if (!strcmp(Option_p, "--compare"))
            {
                DoCompare = atoi(Value_p) ? 1 : 0;
            }
            else if (!strcmp(Option_p, "--variant"))
            {
                int NewVariant = atoi(Value_p);
                if (NewVariant < GOL_VARIANT_LAST_ENTRY)
                {
                    Variant = NewVariant;
                }
            }
            else if (!strcmp(Option_p, "--display"))
            {
                int NewDisplay = atoi(Value_p);
                if (NewDisplay < GOL_DISPLAY_LAST_ENTRY)
                {
                    Display = NewDisplay;
                }
            }
            else
            {
                // Unknown option!
                printf("\nInvalid option: %s\n\n", Option_p);
                Success = 0;
                break;
            }
        }
    }

    if (Success)
    {
        GOL_Game_t TheGame;
        GOL_Game_t RefGame;
        clock_t StartTime;
        clock_t EndTime;

        printf("Game of Life!\n\n"
               "Params... Width=%d Height=%d NumGenerations=%d "
                "File=%s Compare=%d Variant=%d\n\n",
                Width, Height, NumGenerations,
                Filename_p, DoCompare, Variant);

        if (Filename_p != NULL)
        {
            TheGame = GOL_InitializeWorldFromFile(Variant,
                                                  Width,
                                                  Height,
                                                  Filename_p);
//                        GOL_OutputWorld(TheGame);
//                        exit(0);

            if (DoCompare)
            {
                RefGame = GOL_InitializeWorldFromFile(GOL_VARIANT_REFERENCE,
                                                      Width,
                                                      Height,
                                                      Filename_p);
            }
        }
        else
        {
            TheGame = GOL_InitializeWorld(Variant, Width, Height, 1);
            if (DoCompare)
            {
                RefGame = GOL_InitializeWorld(GOL_VARIANT_REFERENCE,
                                              Width, Height, 1);
            }
        }

        if (TheGame == 0)
        {
            return -1;
        }

        StartTime = clock();
        for (int i = 0; i < NumGenerations; i++)
        {
            GOL_EvolveWorld(TheGame);
            if (DoCompare)
            {
                GOL_EvolveWorld(RefGame);
            }

            if (Display == GOL_DISPLAY_ANIMATE)
            {
                system("clear");
                GOL_OutputWorld(TheGame);
                system("sleep 0.1");
            }
            if (DoCompare)
            {
                GOL_CompareWorlds(TheGame, RefGame);
            }
        }
        EndTime = clock();

        if (Display == GOL_DISPLAY_SHOW_FINAL)
        {
            GOL_OutputWorld(TheGame);
        }

        if (Filename_p != NULL)
        {
            GOL_SaveWorldToFile(TheGame, "final_world.txt");
        }

        GOL_DestroyWorld(&TheGame);
        if (DoCompare)
        {
            GOL_DestroyWorld(&RefGame);
        }
        printf("Done! Made %d evolutions in %f seconds\n",
               NumGenerations,
               (((double)(EndTime - StartTime)) / CLOCKS_PER_SEC));
    }
    else
    {
        printf("\n"
               "Usage: %s [--width X]\n"
               "          [--height Y]\n"
               "          [--count NUMBER_OF_GENERATIONS]\n"
               "          [--file  WORLD_FILE]\n"
               "          [--compare BOOL]\n"
               "          [--variant N]\n"
               "          [--display M]\n"
               "\n"
               "Where variants are: 0 - Ref, 1 - Array, 2 - Bits\n"
                "\n"
                "Where displays are: 0 - None,  1 - Animate, 2 - Final evolvement\n"
                "\n"
               "Default values are: X=%d Y=%d NUMBER_OF_GENERATIONS=%d\n"
                "                   WORLD_FILE=N/A (Glider Pattern)\n"
                "                   COMPARE=NO VARIANT=REF\n"
                "                   DISPLAY=ANIMATE\n"
                "\n",
                argv[0],
                DEFAULT_WORLD_WIDTH, DEFAULT_WORLD_HEIGHT, DEFAULT_NUM_GENERATIONS);
    }
    return 0;
}
