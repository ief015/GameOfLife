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
// SimulationScene.cpp
// Author: Nathan Cousins
// 
// Implements class SimulationScene
// 

#include "SimulationScene.hpp"
#include "MenuScene.hpp"

#include <sstream>
#include <iomanip>


//////////////////////////////////////////////////////////////////////
void SimulationScene::init()
{
	auto& rw = this->getManager().getWindow();

	m_paused    = true;
	m_stepOnce  = false;
	m_hideIntro = false;
	m_debugMode = 0;
	m_camera = rw.getDefaultView();

	m_renderer.setRenderTarget(rw);
	m_renderer.setSimulation(m_sim);

	// TODO: add user ability to change ruleset
	m_sim.setRuleset(gol::Ruleset("B3/S5"));
	//m_sim.setRuleset(gol::Ruleset::Flock);

	rw.setTitle(std::string("GOL - ") + m_sim.getRuleset().getString());


	std::stringstream ss;
	ss << "                       WELCOME TO GAME OF LIFE" << std::endl;
	ss << std::endl;
	ss << std::endl;
	ss << "                              CONTROLS" << std::endl;
	ss << std::endl;
	ss << "                left click : place single cell (hold shift to remove)" << std::endl;
	ss << "               right click : place many cells (hold shift to remove)" << std::endl;
	ss << "w/up a/left s/down d/right : move camera " << std::endl;
	ss << "            equals/keypad+ : zoom in camera" << std::endl;
	ss << "            hyphen/keypad- : zoom out camera" << std::endl;
	ss << "                    period : step once (while paused)" << std::endl;
	ss << "                  spacebar : pause/resume simulation" << std::endl;
	ss << "                         r : reset simulation" << std::endl;
	ss << "                    escape : exit simulation" << std::endl;
	ss << "                         t : toggle multithreading" << std::endl;
	ss << "                     tilde : toggle debug mode" << std::endl;
	ss << std::endl;
	ss << std::endl;
	ss << "            > start drawing cells to hide this message <" << std::endl;

	m_txtIntro = sf::Text(ss.str(), SceneManager::getDefaultFont(), 16);
	m_txtIntro.setFillColor(sf::Color(255, 255, 255, 255));
	sf::FloatRect bounds = m_txtIntro.getLocalBounds();
	m_txtIntro.setOrigin(floor(bounds.width / 2.f), floor(bounds.height / 2.f));

	m_txtDebug = sf::Text("", SceneManager::getDefaultFont(), 16);
	m_txtDebug.setFillColor(sf::Color(255, 255, 255, 255));
	m_txtDebug.setOutlineColor(sf::Color(0, 0, 0, 255));
	m_txtDebug.setOutlineThickness(1.5f);
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::finish()
{
	auto& rw = this->getManager().getWindow();
	rw.setTitle("GOL");

	sf::View v = rw.getDefaultView();
	v.setSize(static_cast<float>(rw.getSize().x), static_cast<float>(rw.getSize().y));
	rw.setView(v);

	m_sim.reset();
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::processEvent(const sf::Event & ev)
{
	switch (ev.type)
	{
	case sf::Event::MouseButtonPressed:
		if (ev.mouseButton.button == sf::Mouse::Left)
		{
			m_controls.mouseLeft = true;
			int x, y;
			this->screenToWorld(ev.mouseButton.x, ev.mouseButton.y, x, y);
			this->placeCells(x, y, 1, !m_controls.shift);
		}
		if (ev.mouseButton.button == sf::Mouse::Right)
		{
			m_controls.mouseRight = true;
			int x, y;
			this->screenToWorld(ev.mouseButton.x, ev.mouseButton.y, x, y);
			this->placeCells(x, y, 20, !m_controls.shift);
		}
		if (!m_hideIntro)
		{
			m_txtIntro.setString("");
			m_hideIntro = true;
		}
		break;

	case sf::Event::MouseButtonReleased:
		if (ev.mouseButton.button == sf::Mouse::Left)
			m_controls.mouseLeft = false;
		else if (ev.mouseButton.button == sf::Mouse::Right)
			m_controls.mouseRight = false;
		break;

	case sf::Event::MouseMoved:
		if (m_controls.mouseLeft)
		{
			int x, y;
			this->screenToWorld(ev.mouseMove.x, ev.mouseMove.y, x, y);
			this->placeCells(x, y, 1, !m_controls.shift);
		}
		else if (m_controls.mouseRight)
		{
			int x, y;
			this->screenToWorld(ev.mouseMove.x, ev.mouseMove.y, x, y);
			this->placeCells(x, y, 20, !m_controls.shift);
		}
		break;

	case sf::Event::KeyPressed:
		m_controls.shift = ev.key.shift;
		switch (ev.key.code)
		{
		case sf::Keyboard::Space:
			m_paused = !m_paused;
			break;
		case sf::Keyboard::Escape:
			this->getManager().load<MenuScene>();
			this->close();
			break;
		case sf::Keyboard::R:
			m_sim.reset();
			break;
		case sf::Keyboard::Tilde:
			toggleDebug(++m_debugMode);
			break;
		case sf::Keyboard::T:
			m_sim.setMultithreadMode(!m_sim.isMultithreaded());
			break;
		case sf::Keyboard::Period:
			if (m_paused) m_stepOnce = true;
			break;
		case sf::Keyboard::A:
		case sf::Keyboard::Left:
			m_controls.moveLeft = true;
			break;
		case sf::Keyboard::D:
		case sf::Keyboard::Right:
			m_controls.moveRight = true;
			break;
		case sf::Keyboard::W:
		case sf::Keyboard::Up:
			m_controls.moveUp = true;
			break;
		case sf::Keyboard::S:
		case sf::Keyboard::Down:
			m_controls.moveDown = true;
			break;
		case sf::Keyboard::Equal:
		case sf::Keyboard::Add:
			m_controls.zoomIn = true;
			break;
		case sf::Keyboard::Hyphen:
		case sf::Keyboard::Subtract:
			m_controls.zoomOut = true;
			break;
		}
		break;

	case sf::Event::KeyReleased:
		m_controls.shift = ev.key.shift;
		switch (ev.key.code)
		{
		case sf::Keyboard::A:
		case sf::Keyboard::Left:
			m_controls.moveLeft = false;
			break;
		case sf::Keyboard::D:
		case sf::Keyboard::Right:
			m_controls.moveRight = false;
			break;
		case sf::Keyboard::W:
		case sf::Keyboard::Up:
			m_controls.moveUp = false;
			break;
		case sf::Keyboard::S:
		case sf::Keyboard::Down:
			m_controls.moveDown = false;
			break;
		case sf::Keyboard::Equal:
		case sf::Keyboard::Add:
			m_controls.zoomIn = false;
			break;
		case sf::Keyboard::Hyphen:
		case sf::Keyboard::Subtract:
			m_controls.zoomOut = false;
			break;
		}
		break;

	case sf::Event::Resized:
		m_camera.setSize(static_cast<float>(ev.size.width), static_cast<float>(ev.size.height));
		break;
	}
}


//////////////////////////////////////////////////////////////////////
bool SimulationScene::pre_update()
{
	if (m_controls.moveLeft)
		m_camera.move(-3.f, 0.f);
	if (m_controls.moveRight)
		m_camera.move(3.f, 0.f);
	if (m_controls.moveUp)
		m_camera.move(0.f, -3.f);
	if (m_controls.moveDown)
		m_camera.move(0.f, 3.f);
	if (m_controls.zoomIn)
		m_camera.zoom(1.f - 0.025f);
	if (m_controls.zoomOut)
		m_camera.zoom(1.f + 0.025f);

	if (m_stepOnce)
	{
		m_stepOnce = false;
		return true;
	}
	return !m_paused;
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::update()
{
	if (!pre_update())
		return;
	m_debugUpdateTimestamp = this->getManager().getElapsedTime();
	m_sim.step();
	m_debugUpdateTimestamp = this->getManager().getElapsedTime() - m_debugUpdateTimestamp;
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::render()
{
	m_debugRenderTimestamp = this->getManager().getElapsedTime();
	auto& rw = this->getManager().getWindow();

	rw.setView(m_camera);
	m_renderer.render();

	sf::Vector2u screen = rw.getSize();
	sf::FloatRect visibleArea(0, 0, static_cast<float>(screen.x), static_cast<float>(screen.y));
	rw.setView(sf::View(visibleArea));

	if (!m_hideIntro)
	{
		m_txtIntro.setPosition(static_cast<float>(screen.x / 2), static_cast<float>(screen.y / 2));
		rw.draw(m_txtIntro);
	}

	if (m_debugMode != 0)
		rw.draw(m_txtDebug);

	m_debugRenderTimestamp = this->getManager().getElapsedTime() - m_debugRenderTimestamp;

	if (m_debugMode != 0)
	{
		std::stringstream strDebug;
		strDebug << std::fixed << std::setprecision(2) << "DEBUG";
		if (m_sim.isMultithreaded())
			strDebug << "\nMULTITHREADED (" << m_sim.getWorkerThreadCount() << ")";
		if (m_paused)
			strDebug << "\nPAUSED";
		strDebug << "\nupdate (ms) : " << m_debugUpdateTimestamp.asSeconds() * 1000.f
		         << "\nrender (ms) : " << m_debugRenderTimestamp.asSeconds() * 1000.f
		         << "\nchunks      : " << m_sim.getChunkCount()
		         << "\npopulation  : " << m_sim.getPopulation()
		         << "\nbirths      : " << m_sim.getBirths()
		         << "\ndeaths      : " << m_sim.getDeaths()
		         << "\npop delta   : " << (static_cast<long long>(m_sim.getBirths()) - m_sim.getDeaths())
		         << "\ngeneration  : " << m_sim.getGeneration()
		         << "\nruleset     : " << m_sim.getRuleset().getString();
		m_txtDebug.setString(strDebug.str());
	}
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::toggleDebug(int mode)
{
	m_debugMode = mode;

	switch (mode)
	{
	default:
		m_debugMode = 0;
		m_renderer.showChunks = false;
		m_renderer.showChunksCellCount = false;
		m_renderer.showChunkID = false;
		break;

	case 1:
		m_renderer.showChunks = false;
		m_renderer.showChunksCellCount = false;
		m_renderer.showChunkID = false;
		break;

	case 2:
		m_renderer.showChunks = true;
		m_renderer.showChunksCellCount = false;
		m_renderer.showChunkID = false;
		break;

	case 3:
		m_renderer.showChunks = true;
		m_renderer.showChunksCellCount = true;
		m_renderer.showChunkID = true;
		break;
	}
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::placeCells(int x, int y, int size, bool alive)
{
	if (size <= 1)
	{
		m_sim.setCell(x, y, alive);
	}
	else
	{
		for (int ox = -(size - 1); ox <= (size - 1); ox++)
			for (int oy = -(size - 1); oy <= (size - 1); oy++)
				m_sim.setCell(x + ox, y + oy, alive);
	}
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::screenToWorld(int scr_x, int scr_y, int& out_x, int& out_y)
{
	auto& rw = this->getManager().getWindow();
	sf::Vector2f v = rw.mapPixelToCoords(sf::Vector2i(scr_x, scr_y), m_camera);
	out_x = static_cast<int>(v.x);
	out_y = static_cast<int>(v.y);
}
