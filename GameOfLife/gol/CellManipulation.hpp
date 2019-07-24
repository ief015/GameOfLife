#pragma once

// 
// MIT License
// 
// Copyright(c) 2019 Nathan Cousins
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// 
// gol/CellManipulation.hpp
// Author: Nathan Cousins
// 
// GOL_* preprocessor defines, mainly for internal use.
// 
// Defines a number of utility preprocessor defines for reading and
// manipulating cell states.
// 

namespace gol
{

#define GOL_FLAG_ALIVE 0x1         //> Cell's current alive state.
#define GOL_FLAG_ALIVE_NEXTGEN 0x2 //> Cell's alive state after current generation.

// Reset cell to default state.
#define GOL_RESET_CELL(c) ((c) = 0x00)

// Check if cell is alive.
#define GOL_IS_CELL_ALIVE(c) (((c) & GOL_FLAG_ALIVE) != 0)

// Check if cell will be alive next generation.
#define GOL_IS_CELL_ALIVE_NEXTGEN(c) (((c) & GOL_FLAG_ALIVE_NEXTGEN) != 0)

// Set cell's alive state.
#define GOL_SET_CELL_ALIVE(c,bAlive) ((c) = ((bAlive) ? ((c) | GOL_FLAG_ALIVE) : ((c) & ~GOL_FLAG_ALIVE)))

// Set cell's alive state for next generation.
#define GOL_SET_CELL_ALIVE_NEXTGEN(c,bAlive) ((c) = ((bAlive) ? ((c) | GOL_FLAG_ALIVE_NEXTGEN) : ((c) & ~GOL_FLAG_ALIVE_NEXTGEN)))

}
