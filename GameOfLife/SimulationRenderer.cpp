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
// SimulationRenderer.cpp
// Author: Nathan Cousins
// 
// Implements class SimulationRenderer
// 

#include "SimulationRenderer.hpp"
#include "gol/CellManipulation.hpp"
#include "SceneManager.hpp"
#include <iostream>


using namespace gol;


//////////////////////////////////////////////////////////////////////
SimulationRenderer::SimulationRenderer()
	: m_renderTarget(nullptr)
	, m_simulation(nullptr)
	, showChunks(false)
	, showChunkID(false)
	, showChunksCellCount(false)
{

}


//////////////////////////////////////////////////////////////////////
void SimulationRenderer::setRenderTarget(sf::RenderTarget& renderTarget)
{
	m_renderTarget = &renderTarget;
}


//////////////////////////////////////////////////////////////////////
void SimulationRenderer::setSimulation(const Simulation& simulator)
{
	m_simulation = &simulator;
}


//////////////////////////////////////////////////////////////////////
void SimulationRenderer::render() const
{
	sf::RectangleShape chunkRect;
	if (showChunks)
	{
		chunkRect.setFillColor(sf::Color::Transparent);
		chunkRect.setOutlineColor(sf::Color(255, 255, 255, 64));
		chunkRect.setOutlineThickness(-1.f);
		chunkRect.setSize(sf::Vector2f(static_cast<float>(Chunk::CHUNK_SIZE), static_cast<float>(Chunk::CHUNK_SIZE)));
	}

	sf::Text chunkText;
	if (showChunksCellCount)
	{
		chunkText.setFont(SceneManager::getDefaultFont());
		chunkText.setCharacterSize(16);
		chunkText.setOutlineColor(sf::Color(0, 0, 0, 255));
		chunkText.setOutlineThickness(1.5f);
	}

	m_chunkBuffer.clear();
	m_simulation->getAllChunks(m_chunkBuffer);

	static sf::VertexArray cellGraph(sf::Quads, Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * 4);

	for (auto chunk : m_chunkBuffer)
	{
		if (!chunk->isValid())
			continue;

		int col = chunk->getColumn();
		int row = chunk->getRow();

		float xchunk = col * static_cast<float>(Chunk::CHUNK_SIZE);
		float ychunk = row * static_cast<float>(Chunk::CHUNK_SIZE);

		if ((xchunk + Chunk::CHUNK_SIZE) < cullZone.left)
			continue;
		if ((ychunk + Chunk::CHUNK_SIZE) < cullZone.top)
			continue;
		if (xchunk > (cullZone.left + cullZone.width))
			continue;
		if (ychunk > (cullZone.top + cullZone.height))
			continue;

		cellGraph.clear();

		auto& cellCoords = chunk->getCellCoords();
		for (const std::pair<int,int>& xy : cellCoords)
		{
			float xcell = static_cast<float>(xy.first  + xchunk);
			float ycell = static_cast<float>(xy.second + ychunk);
			cellGraph.append(sf::Vector2f(xcell + 1, ycell + 1));
			cellGraph.append(sf::Vector2f(xcell + 1, ycell));
			cellGraph.append(sf::Vector2f(xcell, ycell));
			cellGraph.append(sf::Vector2f(xcell, ycell + 1));
		}

		m_renderTarget->draw(cellGraph);

		if (showChunks)
		{
			switch (chunk->getSleepMode())
			{
			case Chunk::Sleeping:
				chunkRect.setOutlineColor(sf::Color(255, 32, 32, 64));
				break;
			case Chunk::BorderOnly:
				chunkRect.setOutlineColor(sf::Color(255, 255, 32, 64));
				break;
			case Chunk::Awake:
				chunkRect.setOutlineColor(sf::Color(32, 255, 255, 64));
				break;
			}
			chunkRect.setPosition(xchunk, ychunk);
			m_renderTarget->draw(chunkRect);
		}

		if (showChunksCellCount)
		{
			chunkText.setFillColor(sf::Color(255, 255, 255, 192));
			chunkText.setString(std::to_string(chunk->getAliveCells()));
			chunkText.setPosition(xchunk, ychunk);
			m_renderTarget->draw(chunkText);
		}

		if (showChunkID)
		{
			float y = ychunk + Chunk::CHUNK_SIZE - chunkText.getCharacterSize() - 4.f;
			chunkText.setString(std::to_string(chunk->getUniqueID()));
			chunkText.setFillColor(sf::Color(255, 255, 255, 64));
			chunkText.setPosition(xchunk, y);
			m_renderTarget->draw(chunkText);
		}
	}
}
