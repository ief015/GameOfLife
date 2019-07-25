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
// SimulationRenderer.hpp
// Author: Nathan Cousins
//
// class SimulationRenderer
// 
// A graphical renderer for the data provided by gol::Simulation.
// Utilizes SFML for graphics handling.
// 

#include <SFML/Graphics.hpp>
#include "gol/Simulation.hpp"
#include "gol/Chunk.hpp"

class SimulationRenderer
{
public:
	SimulationRenderer();

	void setRenderTarget(sf::RenderTarget& renderTarget);
	void setSimulation(const gol::Simulation& simulator);

	void render() const;

	bool showChunks;
	bool showChunksCellCount;
	bool showChunkID;

private:
	sf::RenderTarget* m_renderTarget;
	const gol::Simulation* m_simulation;

	mutable std::vector<const gol::Chunk*> m_chunkBuffer;

	//sf::Shader m_shader;
	//static const std::string m_frag;
};
