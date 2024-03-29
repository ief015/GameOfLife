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
// gol/Simulation.hpp
// Author: Nathan Cousins
//
// class gol::Simulation
// 
// Handles the processing of the "Game Of Life" simulation in an unbounded,
// expanding universe. The primary function is to control all simulation
// chunks; including allocating, freeing, and processing. gol::Simulation
// also tracks information about the simulation universe, such as population
// (including deltas), simulation rulesets, chunk count, generation, etc.
// 

#include "Chunk.hpp"
#include "Ruleset.hpp"
#include <unordered_map>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>


namespace gol
{

class Simulation
{
public:
	Simulation();
	virtual ~Simulation();

	// Resets the entire simulation.
	// resetGeneration: Resets generation counter to zero.
	void reset(bool resetGeneration = true);

	// Steps the simulation once. This will move on to the next generation of the simulation.
	void step();

	// Set alive state for cell at position {x,y}.
	void setCell(int x, int y, bool alive);

	// Get alive state for cell at position {x,y}.
	bool getCell(int x, int y) const;

	// Get the current generation.
	inline unsigned int getGeneration() const { return m_generation; }

	// Get number of births for this generation.
	inline unsigned int getBirths() const { return m_births; }

	// Get number of births for this generation.
	inline unsigned int getDeaths() const { return m_deaths; }

	// Get the count of current simulation chunks.
	inline unsigned int getChunkCount() const { return m_chunkCount; }

	// Get the count of currently alive cells.
	inline unsigned int getPopulation() const { return m_cellCount; }

	// Get chunk by {column,row}.
	Chunk* getChunk(int col, int row);
	const Chunk* getChunk(int col, int row) const;

	// Get chunk containing cell position {x,y}.
	Chunk* getChunkAt(int x, int y, bool autoCreate = false);
	const Chunk* getChunkAt(int x, int y) const;

	// Push all chunks in simulator to the vector provided.
	// (Vector is not cleared here, data is only appended.)
	void getAllChunks(std::vector<const Chunk*>& out_vec) const;

	// Get simulation rule-set.
	inline const Ruleset& getRuleset() const { return m_ruleset; }

	// Set simulation rule-set.
	void setRuleset(const Ruleset& ruleset);

	// Enable or disable multithreading mode.
	void setMultithreadMode(bool enable);

	// Get whether the simulation is multithreaded.
	inline bool isMultithreaded() const { return m_multithreaded; }

	// Get number of worker threads running.
	// Returns 0 if multithreading mode is disabled.
	inline size_t getWorkerThreadCount() const { return m_ccWorkers.size(); }


private:
	typedef std::unordered_map<int, Chunk*> RowMap;
	typedef std::unordered_map<int, RowMap> ColumnMap;

	ColumnMap m_chunks; // m_chunks[col][row]
	unsigned int m_chunkCount;
	unsigned int m_cellCount;
	unsigned int m_births;
	unsigned int m_deaths;
	unsigned int m_generation;
	Ruleset m_ruleset;

	Chunk* createChunk(int column, int row);
	void checkForNewChunks();
	void freeInactiveChunks();

	///////////////////////
	///// Concurrency /////
	///////////////////////

	bool m_multithreaded;
	size_t m_availableThreads;
	std::vector<std::thread> m_ccWorkers;
	std::atomic_int m_ccWorking;
	std::mutex m_ccGuard;
	std::condition_variable m_ccSync;
	enum { CCTask_Update, CCTask_Apply } m_ccTask;
	// Simple and minimal thread-safe wrapper for std::queue
	template<class T> class CCSharedQueue {
	private:
		mutable std::mutex m_guard;
		std::queue<T> m_queue;
	public:
		void push(T t) {
			std::unique_lock<std::mutex> lk(m_guard);
			m_queue.push(t);
		}
		bool pop(T& out_t) {
			std::unique_lock<std::mutex> lk(m_guard);
			if (m_queue.empty())
				return false;
			out_t = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}
		bool empty() const {
			std::unique_lock<std::mutex> lk(m_guard);
			return m_queue.empty();
		}
	};
	CCSharedQueue<Chunk*> m_ccQueue;
	std::atomic_int m_ccCellCount;
	std::atomic_int m_ccBirths;
	std::atomic_int m_ccDeaths;
	static void ccStartWorker(Simulation* sim);
};

}