#pragma once
#include <SFML/Graphics.hpp>
#include "gol/Simulation.hpp"
#include "gol/Chunk.hpp"


namespace gol
{

class SimulationRenderer
{
public:
	SimulationRenderer();

	void setRenderTarget(sf::RenderTarget& renderTarget);
	void setSimulation(const Simulation& simulator);

	inline const sf::Font& getDefaultFont() const { return m_font; }

	void render() const;

	bool showChunks;
	bool showChunksCellCount;
	bool showChunkID;

private:
	sf::RenderTarget* m_renderTarget;
	const Simulation* m_simulation;

	mutable std::vector<const Chunk*> m_chunkBuffer;

	sf::Font m_font;
	//sf::Shader m_shader;
	//static const std::string m_frag;
};

}