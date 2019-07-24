#pragma once
#include <atomic>
#include <vector>


namespace gol
{

class Simulation;
class SimulationRenderer;

class Chunk
{
public:
	Chunk(Simulation* simulator, int column, int row);
	virtual ~Chunk();

	enum ESleepMode
	{
		// updateCellStates() will iterate every cell in the chunk.
		Awake,

		// updateCellStates() will only iterate cells on the border of this chunk.
		BorderOnly,

		// updateCellStates() will do nothing.
		Sleeping,
	};

	enum ENeighbour {
		North,
		East,
		South,
		West,
		NorthEast,
		NorthWest,
		SouthEast,
		SouthWest,
	};

	// Cell size of chunks are CHUNK_SIZE squared.
	static const size_t CHUNK_SIZE = 64;

	// Chunk becomes marked for deletion after sleeping for this many steps.
	static const size_t INACTIVITY_TIMEOUT = 100;

	// Returns true if this chunk is valid and active.
	inline bool isValid() const {
		return (m_sim != nullptr) && (m_cells != nullptr);
	}

	// Set cell state at chunk-local coordinates {x,y}.
	void setCell(int x, int y, bool alive);

	// Get cell state at chunk-local coordinates {x,y}.
	bool getCell(int x, int y) const;

	// Get the column this chunk belongs to.
	inline int getColumn() const { return m_column; }

	// Get the row this chunk belongs to.
	inline int getRow() const { return m_row; }

	// Reset all cells in this chunk.
	void clear();

	// Update next generation cell states.
	void updateCellStates();

	// Apply next generation cell states as current states.
	// This function will also update its sleep mode.
	void applyCellStates();

	// Get count of active cells in this chunk.
	inline unsigned int getAliveCells() const { return m_aliveCells; }

	// Get number of births for this generation.
	inline unsigned int getBirths() const { return m_births; }

	// Get number of births for this generation.
	inline unsigned int getDeaths() const { return m_deaths; }

	// Get the current mode of sleep.
	inline ESleepMode getSleepMode() const { return m_sleepMode; }

	// Get parent simulator.
	inline const Simulation* getSimulation() const { return m_sim; }

	// Safely retrieve one of the eight neighbours. Returns nullptr if neighbour does not exist.
	Chunk* getNeighbour(ENeighbour direction);

	// Safely retrieve one of the eight neighbours. Returns nullptr if neighbour does not exist.
	const Chunk* getNeighbour(ENeighbour direction) const;

	// Get unique chunk ID.
	inline unsigned int getUniqueID() const { return m_uid; }

	// Get raw cell data table.
	inline const char* const* getRawCellData() const { return m_cells; }

	// Get {x,y} chunk-local coords for each alive cell.
	const std::vector<std::pair<int,int>>& getCellCoords() const;

private:
	friend Simulation;

	static unsigned int NEXT_UNIQUE_ID;
	const unsigned int m_uid;

	// Safely counts neighbouring cells, including from neighbouring chunks.
	int countActiveNeighbourCells(int x, int y);
	void checkInactivity();

	Simulation* m_sim;
	char** m_cells;
	unsigned int m_aliveCells;
	unsigned int m_births;
	unsigned int m_deaths;

	mutable bool m_cellCoordsInvalid;
	mutable std::vector<std::pair<int,int>> m_cellCoords;

	unsigned int m_inactivity;
	bool m_borderChanged;
	ESleepMode m_sleepMode;

	int m_column;
	int m_row;
	Chunk* m_north;
	Chunk* m_east;
	Chunk* m_south;
	Chunk* m_west;
};

}
