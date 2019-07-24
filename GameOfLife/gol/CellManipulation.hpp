#pragma once

namespace gol
{

#define FLAG_ALIVE 0x1         //> Cell's current alive state.
#define FLAG_ALIVE_NEXTGEN 0x2 //> Cell's alive state after current generation.

// Reset cell to default state.
#define RESET_CELL(c) ((c) = 0x00)

// Check if cell is alive.
#define IS_CELL_ALIVE(c) (((c) & FLAG_ALIVE) != 0)

// Check if cell will be alive next generation.
#define IS_CELL_ALIVE_NEXTGEN(c) (((c) & FLAG_ALIVE_NEXTGEN) != 0)

// Set cell's alive state.
#define SET_CELL_ALIVE(c,bAlive) ((c) = ((bAlive) ? ((c) | FLAG_ALIVE) : ((c) & ~FLAG_ALIVE)))

// Set cell's alive state for next generation.
#define SET_CELL_ALIVE_NEXTGEN(c,bAlive) ((c) = ((bAlive) ? ((c) | FLAG_ALIVE_NEXTGEN) : ((c) & ~FLAG_ALIVE_NEXTGEN)))

}
