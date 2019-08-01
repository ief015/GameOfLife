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
// gol/Chunk.cpp
// Author: Nathan Cousins
// 
// Implements class gol::Chunk
// 

#include "Chunk.hpp"
#include "Simulation.hpp"
#include "CellManipulation.hpp"

#include <iostream>

using namespace gol;


unsigned int Chunk::NEXT_UNIQUE_ID = 0;


//////////////////////////////////////////////////////////////////////
Chunk::Chunk(Simulation* sim, int col, int row)
	: m_sim(sim)
	, m_uid(NEXT_UNIQUE_ID++)
	, m_cells(nullptr)
	, m_aliveCells(0)
	, m_column(col)
	, m_row(row)
	, m_sleepMode(Sleeping)
	, m_inactivity(0)
	, m_north(nullptr)
	, m_east(nullptr)
	, m_south(nullptr)
	, m_west(nullptr)
	, m_births(0)
	, m_deaths(0)
	, m_borderChanged(false)
	, m_cellCoordsInvalid(false)
{
	if (sim == nullptr)
		return;

	// Build cell graph
	if ((m_cells = new char[CHUNK_SIZE * CHUNK_SIZE]) == nullptr)
	{
		std::cerr << "chunk could not allocate cell graph! out of memory? ("
		          << "uid=" << m_uid << ", "
		          << "column=" << m_column << ", "
		          << "row=" << m_row << ")"
		          << std::endl;
	}

	// Zero all cells
	this->clear();

	m_cellCoords.reserve(CHUNK_SIZE);

	// Find and update neighbours of ourself
	if (m_north = sim->getChunk(col, row - 1))
		m_north->m_south = this;
	if (m_east = sim->getChunk(col + 1, row))
		m_east->m_west   = this;
	if (m_south = sim->getChunk(col, row + 1))
		m_south->m_north = this;
	if (m_west = sim->getChunk(col - 1, row))
		m_west->m_east   = this;
}


//////////////////////////////////////////////////////////////////////
Chunk::~Chunk()
{
	// Update neighbours of ourself
	if (m_north)
		m_north->m_south = nullptr;
	if (m_east)
		m_east->m_west   = nullptr;
	if (m_south)
		m_south->m_north = nullptr;
	if (m_west)
		m_west->m_east   = nullptr;

	// Destroy cell graph
	if (m_cells != nullptr)
	{
		delete[] m_cells;
		m_cells = nullptr;
	}
}


//////////////////////////////////////////////////////////////////////
void Chunk::clear()
{
	if (m_cells == nullptr)
		return;

	for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
		GOL_RESET_CELL(m_cells[i]);

	m_aliveCells = 0;
}


//////////////////////////////////////////////////////////////////////
void Chunk::updateCellStates()
{
	if (!this->isValid())
		return;

	int births = 0;
	int deaths = 0;

	// Determines if border cells have changed since last update
	bool borderChanged = false;

	if (m_sleepMode != Sleeping)
	{
		const Ruleset& ruleset = this->getSimulation()->getRuleset();
		size_t idx = 0;

		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			// True if cell borders east/west
			bool yEdge = (y == 0 || y == CHUNK_SIZE - 1);

			for (int x = 0; x < CHUNK_SIZE; x++, idx++)
			{
				// True if cell borders north/south
				bool xEdge = (x == 0 || x == CHUNK_SIZE - 1);

				char& cell = m_cells[idx];
				int neighbours;

				if (xEdge || yEdge)
				{
					// Cell borders another chunk...
					// countActiveNeighbourCells() checks neighbouring chunks
					neighbours = this->countActiveNeighbourCells(x, y);
				}
				else
				{
					// Performance optimization
					// Only process border cells when this chunk has not changed since last update
					if (m_sleepMode == BorderOnly)
						continue;

					// Cell does not border another chunk...
					// No need for edge-checking overhead for neighbouring chunks
					neighbours = (GOL_IS_CELL_ALIVE(m_cells[idx -  1 - CHUNK_SIZE]) ? 1 : 0) + //> x-1 , y-1
					             (GOL_IS_CELL_ALIVE(m_cells[idx -  1])              ? 1 : 0) + //> x-1 , y  
					             (GOL_IS_CELL_ALIVE(m_cells[idx -  1 + CHUNK_SIZE]) ? 1 : 0) + //> x-1 , y+1
					             (GOL_IS_CELL_ALIVE(m_cells[idx -      CHUNK_SIZE]) ? 1 : 0) + //> x   , y-1
					             (GOL_IS_CELL_ALIVE(m_cells[idx +      CHUNK_SIZE]) ? 1 : 0) + //> x   , y+1
					             (GOL_IS_CELL_ALIVE(m_cells[idx +  1 - CHUNK_SIZE]) ? 1 : 0) + //> x+1 , y-1
					             (GOL_IS_CELL_ALIVE(m_cells[idx +  1])              ? 1 : 0) + //> x+1 , y  
					             (GOL_IS_CELL_ALIVE(m_cells[idx +  1 + CHUNK_SIZE]) ? 1 : 0);  //> x+1 , y+1
				}

				if (GOL_IS_CELL_ALIVE(cell))
				{
					// Cell is alive, check if we stay alive
					if (!ruleset.testSurvival(neighbours))
					{
						// Died
						GOL_SET_CELL_ALIVE_NEXTGEN(cell, false);
						deaths++;

						// Set borderChanged true if a border cell changed
						borderChanged = borderChanged || xEdge || yEdge;
					}
				}
				else
				{
					// Cell is dead, check for birth
					if (ruleset.testBirth(neighbours))
					{
						// Born
						GOL_SET_CELL_ALIVE_NEXTGEN(cell, true);
						births++;

						// Set borderChanged true if a border cell changed
						borderChanged = borderChanged || xEdge || yEdge;
					}
				}
			} // for x
		} // for y
	} // if (m_sleepMode != Sleeping)

	m_borderChanged = borderChanged;

	// Update population deltas
	m_births = births;
	m_deaths = deaths;
}

//////////////////////////////////////////////////////////////////////
void Chunk::applyCellStates()
{
	if (!this->isValid())
		return;

	if (m_births > 0 || m_deaths > 0)
	{
		// Population has changed...
		m_aliveCells += m_births;
		m_aliveCells -= m_deaths;

		m_cellCoords.clear();

		// Update all cells to their next generation states
		size_t idx = 0;
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++, idx++)
			{
				char& cell = m_cells[idx];
				bool alive = GOL_IS_CELL_ALIVE_NEXTGEN(cell);
				GOL_SET_CELL_ALIVE(cell, alive);
				if (alive)
					m_cellCoords.emplace_back(x, y);
			}
		}

		m_cellCoordsInvalid = false;

		// Fully awake for next step
		m_sleepMode = Awake;
	}
	else
	{
		// Population has not changed...

		// Keep an eye out for population changes on neighbour chunk borders...
		// If neighbouring border cells have changed, set ourself to check only border cells next step
		// Otherwise, fully sleep
		Chunk* c;
		if (m_north && m_north->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if (m_east && m_east->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if (m_south && m_south->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if (m_west && m_west->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if ((c = this->getNeighbour(NorthEast)) && c->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if ((c = this->getNeighbour(NorthWest)) && c->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if ((c = this->getNeighbour(SouthEast)) && c->m_borderChanged)
			m_sleepMode = BorderOnly;
		else if ((c = this->getNeighbour(SouthWest)) && c->m_borderChanged)
			m_sleepMode = BorderOnly;
		else
			m_sleepMode = Sleeping;
	}

	this->checkInactivity();
}


//////////////////////////////////////////////////////////////////////
void Chunk::setCell(int x, int y, bool alive)
{
	if (m_cells == nullptr)
		return;

	if (x < 0 || x >= CHUNK_SIZE)
		x %= CHUNK_SIZE;
	if (y < 0 || y >= CHUNK_SIZE)
		y %= CHUNK_SIZE;

	char& cell = m_cells[cellCoords2Index(x, y)];

	// Modify active cell counts for this chunk
	if (GOL_IS_CELL_ALIVE(cell))
	{
		if (!alive)
		{
			m_aliveCells--;
			m_sleepMode = Awake;
			m_cellCoordsInvalid = true;
		}
	}
	else
	{
		if (alive)
		{
			m_aliveCells++;
			m_sleepMode = Awake;
			m_cellCoordsInvalid = true;
		}
	}

	// Set cell states
	GOL_SET_CELL_ALIVE(cell, alive);
	GOL_SET_CELL_ALIVE_NEXTGEN(cell, alive);
}


//////////////////////////////////////////////////////////////////////
bool Chunk::getCell(int x, int y) const
{
	if (m_cells == nullptr)
		return false;

	if (x < 0 || x >= CHUNK_SIZE)
		x %= CHUNK_SIZE;
	if (y < 0 || y >= CHUNK_SIZE)
		y %= CHUNK_SIZE;
	
	return GOL_IS_CELL_ALIVE(m_cells[cellCoords2Index(x, y)]);
}


//////////////////////////////////////////////////////////////////////
int Chunk::countActiveNeighbourCells(int x, int y)
{
	int neighbours = 0;

	for (int ox = -1; ox <= 1; ox++)
	{
		for (int oy = -1; oy <= 1; oy++)
		{
			if (ox == 0 && oy == 0)
				continue; //> Skip self

			int nx = x + ox;
			int ny = y + oy;
			const Chunk* chunk = this;

			if (nx < 0)
			{
				if (ny < 0)
					chunk = chunk->getNeighbour(NorthWest);
				else if (ny >= CHUNK_SIZE)
					chunk = chunk->getNeighbour(SouthWest);
				else
					chunk = chunk->getNeighbour(West);
			}
			else if (nx >= CHUNK_SIZE)
			{
				if (ny < 0)
					chunk = chunk->getNeighbour(NorthEast);
				else if (ny >= CHUNK_SIZE)
					chunk = chunk->getNeighbour(SouthEast);
				else
					chunk = chunk->getNeighbour(East);
			}
			else
			{
				if (ny < 0)
					chunk = chunk->getNeighbour(North);
				else if (ny >= CHUNK_SIZE)
					chunk = chunk->getNeighbour(South);
			}

			if (chunk)
				neighbours += chunk->getCell(nx, ny) ? 1 : 0;
		}
	}

	return neighbours;
}


//////////////////////////////////////////////////////////////////////
void Chunk::checkInactivity()
{
	size_t temp = m_inactivity;
	m_inactivity = 0;

	// Chunk is inactive only if it's fully sleeping
	if (m_sleepMode != Sleeping)
		return;

	// Chunk is inactive when itself and neighbour chunks have no active cells
	const Chunk* n;
	if (m_aliveCells != 0)
		return;
	if (m_north && m_north->m_aliveCells != 0)
		return;
	if (m_east  && m_east->m_aliveCells  != 0)
		return;
	if (m_south && m_south->m_aliveCells != 0)
		return;
	if (m_west  && m_west->m_aliveCells  != 0)
		return;
	if ((n = this->getNeighbour(NorthEast)) && n->m_aliveCells != 0)
		return;
	if ((n = this->getNeighbour(NorthWest)) && n->m_aliveCells != 0)
		return;
	if ((n = this->getNeighbour(SouthEast)) && n->m_aliveCells != 0)
		return;
	if ((n = this->getNeighbour(SouthWest)) && n->m_aliveCells != 0)
		return;

	// Chunk is not being used, +1 to inactivity
	m_inactivity = temp + 1;
}


//////////////////////////////////////////////////////////////////////
Chunk* Chunk::getNeighbour(ENeighbour direction)
{
	switch (direction)
	{
	case North:
		return m_north;
	case East:
		return m_east;
	case South:
		return m_south;
	case West:
		return m_west;
	case NorthEast:
		if (m_north)
			return m_north->m_east;
		else if (m_east)
			return m_east->m_north;
		break;
	case NorthWest:
		if (m_north)
			return m_north->m_west;
		else if (m_west)
			return m_west->m_north;
		break;
	case SouthEast:
		if (m_south)
			return m_south->m_east;
		else if (m_east)
			return m_east->m_south;
		break;
	case SouthWest:
		if (m_south)
			return m_south->m_west;
		else if (m_west)
			return m_west->m_south;
		break;
	}
	return nullptr;
}


//////////////////////////////////////////////////////////////////////
const Chunk* Chunk::getNeighbour(ENeighbour direction) const
{
	switch (direction)
	{
	case North:
		return m_north;
	case East:
		return m_east;
	case South:
		return m_south;
	case West:
		return m_west;
	case NorthEast:
		if (m_north)
			return m_north->m_east;
		else if (m_east)
			return m_east->m_north;
		break;
	case NorthWest:
		if (m_north)
			return m_north->m_west;
		else if (m_west)
			return m_west->m_north;
		break;
	case SouthEast:
		if (m_south)
			return m_south->m_east;
		else if (m_east)
			return m_east->m_south;
		break;
	case SouthWest:
		if (m_south)
			return m_south->m_west;
		else if (m_west)
			return m_west->m_south;
		break;
	}
	return nullptr;
}


//////////////////////////////////////////////////////////////////////
const std::vector<std::pair<int, int>>& Chunk::getCellCoords() const
{
	if (m_cellCoordsInvalid)
	{
		m_cellCoords.clear();
		m_cellCoordsInvalid = false;
		size_t idx = 0;
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++, idx++)
			{
				if (GOL_IS_CELL_ALIVE(m_cells[idx]))
					m_cellCoords.emplace_back(x, y);
			}
		}
	}
	return m_cellCoords;
}