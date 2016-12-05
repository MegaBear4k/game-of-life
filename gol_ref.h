/*
 * lifegame.h
 *
 *  Created on: Jan 7, 2010
 *      Author: Daniel Weller
 */

/* Warning: Do NOT modify the contents of this file.
   All your code should be in lab1a.c. */

#ifndef LIFEGAME_H_
#define LIFEGAME_H_

/* state constants */
#define DEAD 0
#define ALIVE 1

/* hard-coded world size */
#define WORLDWIDTH 39
#define WORLDHEIGHT 20


typedef struct
{
    int world[WORLDWIDTH][WORLDHEIGHT];      /* current cell states of the world */
    int nextstates[WORLDWIDTH][WORLDHEIGHT]; /* next generation cell states */
} RefGame_t;


/* initialize_world -- set up world, all cells initialized
   to DEAD or ALIVE; all cells in next generation are
   initialized to DEAD */
void initialize_world(RefGame_t* Game_p);

/* returns the width (x) and height (y) of the world */
int get_world_width(void);
int get_world_height(void);

/* returns the state (DEAD or ALIVE) of the cell at (x,y);
   coordinates go from x = 0,...,width-1 and
   y = 0,...,height-1; returns DEAD for cells outside this
   range */
int get_cell_state(RefGame_t* Game_p, int x, int y);

/* sets the state (DEAD or ALIVE) of the cell at (x,y) in
   the next generation; range of coordinates same as in
   get_cell_state(); calls abort() if invalid coordinates
   are specified */
void set_cell_state(RefGame_t* Game_p, int x, int y, int state);

/* As above, but sets the value in the current world, not the evolving. */
void set_cell_state_in_current(RefGame_t* Game_p, int x, int y, int state);


/* updates world state to next generation and resets all
   next generation states to DEAD */
void finalize_evolution(RefGame_t* Game_p);

/* outputs the current world state to the console */
void output_world(RefGame_t* Game_p);

/* functions to implement for Part B */
void initialize_world_from_file(RefGame_t* Game_p, const char * filename);
void save_world_to_file(RefGame_t* Game_p, const char* filename);

void next_generation(RefGame_t* Game_p);

#endif /* LIFEGAME_H_ */
