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
	/*
	if (!m_shader.loadFromMemory(m_frag, sf::Shader::Fragment))
	{
		std::cerr << "could not load fragment shader!" << std::endl;
	}
	*/
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
	// TODO BUG:  things aren't being rendererd in negative space
	// TODO: only render chunks within view

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
		int col = chunk->getColumn();
		int row = chunk->getRow();

		if (!chunk->isValid())
			continue;

		cellGraph.clear();

		auto& cellCoords = chunk->getCellCoords();
		for (const std::pair<int,int>& xy : cellCoords)
		{
			float cx = static_cast<float>(xy.first  + (col * Chunk::CHUNK_SIZE));
			float cy = static_cast<float>(xy.second + (row * Chunk::CHUNK_SIZE));
			cellGraph.append(sf::Vector2f(cx + 1, cy + 1));
			cellGraph.append(sf::Vector2f(cx + 1, cy));
			cellGraph.append(sf::Vector2f(cx, cy));
			cellGraph.append(sf::Vector2f(cx, cy + 1));
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
			chunkRect.setPosition(static_cast<float>(col * Chunk::CHUNK_SIZE), static_cast<float>(row * Chunk::CHUNK_SIZE));
			m_renderTarget->draw(chunkRect);
		}

		if (showChunksCellCount)
		{
			chunkText.setFillColor(sf::Color(255, 255, 255, 192));
			chunkText.setString(std::to_string(chunk->getAliveCells()));
			chunkText.setPosition(static_cast<float>(col * Chunk::CHUNK_SIZE), static_cast<float>(row * Chunk::CHUNK_SIZE));
			m_renderTarget->draw(chunkText);
		}

		if (showChunkID)
		{
			float y = static_cast<float>(row * Chunk::CHUNK_SIZE);
			y += Chunk::CHUNK_SIZE - chunkText.getCharacterSize() - 4;
			chunkText.setString(std::to_string(chunk->getUniqueID()));
			chunkText.setFillColor(sf::Color(255, 255, 255, 64));
			chunkText.setPosition(static_cast<float>(col * Chunk::CHUNK_SIZE), y);
			m_renderTarget->draw(chunkText);
		}
	}
}


//////////////////////////////////////////////////////////////////////
/*
const std::string SimulationRenderer::m_frag =
"uniform float[] cellBuffer;"\
"void main()"\
"{"\
"	gl_FragColor = gl_Color;"\
"}";*/
