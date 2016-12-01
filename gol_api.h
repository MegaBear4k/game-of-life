/*
 * Game of Life API
 */


#define DEFAULT_WORLD_WIDTH       39
#define DEFAULT_WORLD_HEIGHT      20

#define DEFAULT_NUM_GENERATIONS   50


#define CELL_ALIVE  1
#define CELL_DEAD   0


typedef enum
{
    GOL_VARIANT_REFERENCE,
    GOL_VARIANT_ARRAY,
    GOL_VARIANT_BITS,

    GOL_VARIANT_LAST_ENTRY
} GOL_Variant_t;


typedef void* GOL_Game_t;


GOL_Game_t
GOL_InitializeWorld(const GOL_Variant_t Variant,
                    const int           Width,
                    const int           Height);


GOL_Game_t
GOL_InitializeWorldFromFile(const GOL_Variant_t Variant,
                            const int           Width,
                            const int           Height,
                            const char* const   Filename_p);


void
GOL_DestroyWorld(GOL_Game_t* Game_p);


void
GOL_EvolveWorld(const GOL_Game_t Game);


// Prints and exit():s if worlds differ
void
GOL_CompareWorlds(const GOL_Game_t Game1, const GOL_Game_t Game2);


void
GOL_OutputWorld(const GOL_Game_t Game);


void
GOL_SaveWorldToFile(const GOL_Game_t Game, const char* const Filename_p);


int
GOL_GetWorldWidth(const GOL_Game_t Game);


int
GOL_GetWorldHeight(const GOL_Game_t Game);


int
GOL_GetCellState(const GOL_Game_t Game, const int x, const int y);
