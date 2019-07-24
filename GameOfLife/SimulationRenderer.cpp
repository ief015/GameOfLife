#include "SimulationRenderer.hpp"
#include "gol/CellManipulation.hpp"
#include <iostream>


using namespace gol;


//////////////////////////////////////////////////////////////////////
SimulationRenderer::SimulationRenderer()
	: m_renderTarget(nullptr)
	, m_simulation(nullptr)
	, showChunks(false)
	, showChunkID(false)
{
	const std::string fontpath = "ProggyCleanSZ.ttf";
	if (!m_font.loadFromFile(fontpath))
	{
		std::cerr << "could not load font (" << fontpath << ")!" << std::endl;
	}
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
		chunkText.setFont(m_font);
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

		// TODO REMOVE: old rendering code
		/*
		if (chunk->getAliveCells() > 0)
		{
			unsigned int processed = 0;
			auto cells = chunk->getRawCellData();

			for (int x = 0; x < static_cast<int>(Chunk::CHUNK_SIZE); x++)
			{
				for (int y = 0; y < static_cast<int>(Chunk::CHUNK_SIZE); y++)
				{
					if (IS_CELL_ALIVE(cells[x][y]))
					{
						float cx = static_cast<float>(x + (col * Chunk::CHUNK_SIZE));
						float cy = static_cast<float>(y + (row * Chunk::CHUNK_SIZE));
						cellGraph.append(sf::Vector2f(cx+1, cy+1));
						cellGraph.append(sf::Vector2f(cx+1, cy));
						cellGraph.append(sf::Vector2f(cx, cy));
						cellGraph.append(sf::Vector2f(cx, cy+1));
						processed++;
					}
					if (processed >= static_cast<size_t>(chunk->getAliveCells()))
					{
						// All cells found in this chunk, this cell graph is done.
						x = Chunk::CHUNK_SIZE;
						break;
					}
				}
			}
		}
		*/

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
