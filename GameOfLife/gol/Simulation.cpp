#include "Simulation.hpp"
#include <vector>
#include <iostream>

using namespace gol;


//////////////////////////////////////////////////////////////////////
Simulation::Simulation()
	: m_generation(0)
	, m_chunkCount(0)
	, m_cellCount(0)
	, m_ruleset(Ruleset::GameOfLife)
	, m_multithreaded(true)
	, m_availableThreads(std::thread::hardware_concurrency())
	, m_ccWorking(0)
	, m_ccCellCount(0)
	, m_births(0)
	, m_deaths(0)
{
	// Initialize multithreaded mode
	this->setMultithreadMode(m_multithreaded);
}


//////////////////////////////////////////////////////////////////////
Simulation::~Simulation()
{
	// Destroy worker threads if necessary
	this->setMultithreadMode(false);

	// Free chunks
	this->reset();
}


//////////////////////////////////////////////////////////////////////
void Simulation::reset(bool resetGeneration)
{
	// Free all chunks
	for (auto itCol : m_chunks)
		for (auto itRow : itCol.second)
			delete itRow.second;
	m_chunks.clear();
	m_chunkCount = 0;

	if (resetGeneration)
		m_generation = 0;
}


//////////////////////////////////////////////////////////////////////
void Simulation::step()
{
	// Check if new chunks need to be made
	this->checkForNewChunks();

	if (m_multithreaded)
	{
		m_ccCellCount = 0;
		m_ccBirths    = 0;
		m_ccDeaths    = 0;

		for (const auto task : { CCTask_Update, CCTask_Apply })
		{
			// Assign workers new task
			m_ccTask = task;

			m_ccCompletions = 0;

			// Give workers chunks to process
			for (auto itCol : m_chunks)
			{
				for (auto itRow : itCol.second)
				{
					// Queue next chunk
					m_ccQueue.push(itRow.second);

					// Notify any waiting worker that we have a chunk ready
					m_ccSync.notify_one();
				}
			}

			// Notify all workers to work on remaining
			// chunks in case any weren't notified
			m_ccSync.notify_all();

			// Yield while workers finish up
			while (m_ccWorking > 0 || !m_ccQueue.empty())
			{
				if (m_ccWorking == 0) //> Sanity check
					m_ccSync.notify_all();

				std::this_thread::yield();
			}

			if (m_ccCompletions != m_chunkCount) // TODO REMOVE BLOCK
			{
				std::cout << "Error during task " << (m_ccTask == CCTask_Update ? "UPDATE" : "APPLY");
				std::cout << std::endl << "Chunks processed: ";
				std::cout << m_ccCompletions << " / " << m_chunkCount << std::endl;
			}
		}

		m_cellCount = m_ccCellCount;
		m_births    = m_ccBirths;
		m_deaths    = m_ccDeaths;
	}
	else
	{
		// Update cell states for next generation
		int births = 0;
		int deaths = 0;
		for (auto itCol : m_chunks)
		{
			for (auto itRow : itCol.second)
			{
				itRow.second->updateCellStates();
				births += itRow.second->getBirths();
				deaths += itRow.second->getDeaths();
			}
		}
		m_births = births;
		m_deaths = deaths;

		// Apply new cell states
		int cellCount = 0;
		for (auto itCol : m_chunks)
		{
			for (auto itRow : itCol.second)
			{
				itRow.second->applyCellStates();
				cellCount += itRow.second->getAliveCells();
			}
		}
		m_cellCount = cellCount;
	}

	// Check each chunk for deletion (if no active cells for self and neighbour chunks)
	this->freeInactiveChunks();

	m_generation++;
}


//////////////////////////////////////////////////////////////////////
void Simulation::ccStartWorker(Simulation* sim)
{
	Chunk* chunk;
	sim->m_ccWorking++;
	while (sim->m_multithreaded)
	{
		// Wait for workload
		if (sim->m_ccQueue.empty())
		{
			std::unique_lock<std::mutex> lk(sim->m_ccGuard);
			sim->m_ccWorking--;
			sim->m_ccSync.wait(lk);
			if (!sim->m_multithreaded)
				break; //> Simulation multithreading was disabled while on standby
			sim->m_ccWorking++;
		}

		// Process next chunk in queue
		if (sim->m_ccQueue.pop(chunk))
		{
			if (sim->m_ccTask == CCTask_Update)
			{
				chunk->updateCellStates();
				sim->m_ccBirths += chunk->getBirths();
				sim->m_ccDeaths += chunk->getDeaths();
			}
			else
			{
				chunk->applyCellStates();
				sim->m_ccCellCount += chunk->getAliveCells();
			}
			sim->m_ccCompletions++;
		}
	}
}


//////////////////////////////////////////////////////////////////////
void Simulation::setMultithreadMode(bool enable)
{
	// Check if multithreading is available
	if (enable)
	{
		m_multithreaded = (m_availableThreads > 1);
		if (m_multithreaded && m_ccWorkers.empty())
		{
			// Create and start worker threads
			for (size_t i = 0; i < m_availableThreads; i++)
				m_ccWorkers.push_back(std::thread(&ccStartWorker, this));
		}
	}
	else
	{
		// Signal to close workers and join to main thread
		m_multithreaded = false;
		if (!m_ccWorkers.empty())
		{
			m_ccSync.notify_all();
			for (std::thread& worker : m_ccWorkers)
				worker.join();
			m_ccWorkers.clear();
		}
	}
}


//////////////////////////////////////////////////////////////////////
void Simulation::setCell(int x, int y, bool alive)
{
	Chunk* chunk = this->getChunkAt(x, y, alive);
	//^ "alive": only auto-create chunk if we're activating a cell
	
	if (chunk == nullptr)
		return;

	// Local {x,y} becomes chunk-coords
	x %= Chunk::CHUNK_SIZE;
	y %= Chunk::CHUNK_SIZE;

	m_cellCount -= chunk->getAliveCells();

	chunk->setCell(x, y, alive);

	if (x == 0 || y == 0 || x == Chunk::CHUNK_SIZE - 1 || y == Chunk::CHUNK_SIZE - 1)
	{
		// Border cell changed, create/update neighbour chunks to BorderOnly
		for (int ox = -1; ox <= 1; ox++)
		{
			for (int oy = -1; oy <= 1; oy++)
			{
				if (ox == 0 && oy == 0)
					continue;
				Chunk* n = this->createChunk(chunk->m_column + ox, chunk->m_row + oy);
				if (n && n->m_sleepMode == Chunk::Sleeping)
					n->m_sleepMode = Chunk::BorderOnly;
			}
		}
	}

	// Reapply cell count
	m_cellCount += chunk->getAliveCells();
}


//////////////////////////////////////////////////////////////////////
bool Simulation::getCell(int x, int y) const
{
	const Chunk* chunk = this->getChunkAt(x, y);
	
	if (chunk == nullptr)
		return false;

	// Local {x,y} becomes chunk-coords
	x %= Chunk::CHUNK_SIZE;
	y %= Chunk::CHUNK_SIZE;

	return chunk->getCell(x, y);
}


//////////////////////////////////////////////////////////////////////
Chunk* Simulation::createChunk(int col, int row)
{
	Chunk* chunk = nullptr;

	// Find or create chunk column
	auto itMapCol = m_chunks.find(col);
	if (itMapCol == m_chunks.end())
	{
		// No column found, make one
		itMapCol = m_chunks.insert(std::pair<int, RowMap>(col, RowMap())).first;
	}

	auto& mapCol = itMapCol->second;

	// Find or create chunk row
	auto itMapRow = mapCol.find(row);
	if (itMapRow == mapCol.end())
	{
		// Create new chunk
		chunk = new Chunk(this, col, row);
		itMapRow = mapCol.insert(std::pair<int, Chunk*>(row, chunk)).first;

		m_chunkCount++;
	}
	
	return itMapRow->second;
}


//////////////////////////////////////////////////////////////////////
Chunk* Simulation::getChunkAt(int x, int y, bool autoCreate)
{
	int col, row;
	col = x / Chunk::CHUNK_SIZE;
	row = y / Chunk::CHUNK_SIZE;

	// Auto-create it if requested
	if (autoCreate)
		return this->createChunk(col, row);

	auto itMapCol = m_chunks.find(col);
	if (itMapCol != m_chunks.end())
	{
		auto itMapRow = itMapCol->second.find(row);
		if (itMapRow != itMapCol->second.end())
		{
			// Chunk found
			return itMapRow->second;
		}
	}

	return nullptr;
}


//////////////////////////////////////////////////////////////////////
const Chunk* Simulation::getChunkAt(int x, int y) const
{
	int col, row;
	col = x / Chunk::CHUNK_SIZE;
	row = y / Chunk::CHUNK_SIZE;

	auto itMapCol = m_chunks.find(col);
	if (itMapCol != m_chunks.end())
	{
		auto itMapRow = itMapCol->second.find(row);
		if (itMapRow != itMapCol->second.end())
		{
			// Chunk found
			return itMapRow->second;
		}
	}

	return nullptr;
}


//////////////////////////////////////////////////////////////////////
Chunk* Simulation::getChunk(int col, int row)
{
	// Find column
	auto itMapCol = m_chunks.find(col);
	if (itMapCol == m_chunks.end())
		return nullptr;
	
	auto& mapCol = itMapCol->second;

	// Find row
	auto itMapRow = mapCol.find(row);
	if (itMapRow == mapCol.end())
		return nullptr;
	
	return itMapRow->second;
}


//////////////////////////////////////////////////////////////////////
const Chunk* Simulation::getChunk(int col, int row) const
{
	// Find column
	auto itMapCol = m_chunks.find(col);
	if (itMapCol == m_chunks.end())
		return nullptr;

	auto& mapCol = itMapCol->second;

	// Find row
	auto itMapRow = mapCol.find(row);
	if (itMapRow == mapCol.end())
		return nullptr;

	return itMapRow->second;
}


//////////////////////////////////////////////////////////////////////
void Simulation::getAllChunks(std::vector<const Chunk*>& out_vec) const
{
	// Reserve element space if necessary
	if (out_vec.capacity() < m_chunkCount)
		out_vec.reserve(m_chunkCount);

	for (auto itCol : m_chunks)
		for (auto itRow : itCol.second)
			out_vec.push_back(itRow.second);
}


//////////////////////////////////////////////////////////////////////
void Simulation::setRuleset(const Ruleset& ruleset)
{
	m_ruleset = ruleset;
}


//////////////////////////////////////////////////////////////////////
void Simulation::checkForNewChunks()
{
	static std::vector<std::pair<int, int>> newColRows;

	// Search for column-rows to be created
	for (auto itCol : m_chunks)
	{
		for (auto itRow : itCol.second)
		{
			Chunk* chunk = itRow.second;
			int col = itCol.first;
			int row = itRow.first;

			if (chunk->m_aliveCells > 0)
			{
				if (chunk->getNeighbour(Chunk::North) == nullptr)
					newColRows.push_back(std::pair<int, int>(col, row - 1));
				if (chunk->getNeighbour(Chunk::East) == nullptr)
					newColRows.push_back(std::pair<int, int>(col + 1, row));
				if (chunk->getNeighbour(Chunk::South) == nullptr)
					newColRows.push_back(std::pair<int, int>(col, row + 1));
				if (chunk->getNeighbour(Chunk::West) == nullptr)
					newColRows.push_back(std::pair<int, int>(col - 1, row));
				if (chunk->getNeighbour(Chunk::NorthEast) == nullptr)
					newColRows.push_back(std::pair<int, int>(col + 1, row - 1));
				if (chunk->getNeighbour(Chunk::NorthWest) == nullptr)
					newColRows.push_back(std::pair<int, int>(col - 1, row - 1));
				if (chunk->getNeighbour(Chunk::SouthEast) == nullptr)
					newColRows.push_back(std::pair<int, int>(col + 1, row + 1));
				if (chunk->getNeighbour(Chunk::SouthWest) == nullptr)
					newColRows.push_back(std::pair<int, int>(col - 1, row + 1));
			}
		}
	}

	// Create chunks
	for (const auto& cr : newColRows)
		this->createChunk(cr.first, cr.second);

	newColRows.clear();
}


//////////////////////////////////////////////////////////////////////
void Simulation::freeInactiveChunks()
{
	// Chunks are deleted if they are inactive for too many steps.
	for (auto itCol = m_chunks.begin(); itCol != m_chunks.end(); )
	{
		for (auto itRow = itCol->second.begin(); itRow != itCol->second.end(); )
		{
			Chunk* chunk = itRow->second;
			if (chunk->m_inactivity > Chunk::INACTIVITY_TIMEOUT)
			{
				// Delete chunk, remove row from map
				delete chunk;
				itRow = itCol->second.erase(itRow);
				m_chunkCount--;
			}
			else
			{
				itRow++;
			}
		}

		// Delete column if it's now empty
		if (itCol->second.empty())
			itCol = m_chunks.erase(itCol);
		else
			itCol++;
	}
}
