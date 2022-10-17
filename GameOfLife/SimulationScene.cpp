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
#include <iostream>


const float CAMERA_ZOOM_INIT = 1 / 8.f;
const float CAMERA_ZOOM_SHOW_GRID = 1 / 4.f;


//////////////////////////////////////////////////////////////////////
void SimulationScene::init()
{
	auto& rw = this->getManager().getWindow();
	auto& settings = UserSettings::instance();
	m_renderer.setRenderTarget(rw);
	m_renderer.setSimulation(m_sim);

	m_camera        = rw.getView();
	m_lastPreUpdate = this->getManager().getElapsedTime().asSeconds();
	this->cameraSetZoom(CAMERA_ZOOM_INIT);

	gol::Ruleset rules;
	if (rules.set(settings.getString("ruleset")))
		m_sim.setRuleset(rules);
	rw.setTitle(std::string("GOL - ") + m_sim.getRuleset().getString());

	this->setTargetStepsPerSecond(settings.getFloat("steps_per_second", 60.f));

	std::stringstream ss;
	ss << "                       WELCOME TO GAME OF LIFE" << std::endl;
	ss << std::endl;
	ss << std::endl;
	ss << "                              CONTROLS" << std::endl;
	ss << std::endl;
	ss << "                left click : place single cell (hold shift to remove)" << std::endl;
	ss << "               right click : place many cells (hold shift to remove)" << std::endl;
	ss << "w/up a/left s/down d/right : move camera " << std::endl;
	ss << "          equals / keypad+ : zoom in camera" << std::endl;
	ss << "          hyphen / keypad- : zoom out camera" << std::endl;
	ss << "           1-9 / keypad1-9 : set zoom level" << std::endl;
	ss << "                         g : toggle grid (when zoomed in)" << std::endl;
	ss << "                    period : step once (while paused)" << std::endl;
	ss << "                  spacebar : pause/resume simulation" << std::endl;
	ss << "                     [ / ] : change step rate (hold shift x10)" << std::endl;
	ss << "                         r : reset simulation" << std::endl;
	ss << "                    escape : exit simulation" << std::endl;
	ss << "                         t : toggle multithreading" << std::endl;
	ss << "                     tilde : toggle debug mode" << std::endl;
	ss << std::endl;
	ss << std::endl;
	ss << "            > start drawing cells to hide this message <" << std::endl;

	sf::FloatRect bounds;
	m_txtIntro = sf::Text(ss.str(), SceneManager::getDefaultFont(), 16);
	m_txtIntro.setFillColor(sf::Color(255, 255, 255, 255));
	m_txtIntro.setOutlineColor(sf::Color(0, 0, 0, 255));
	m_txtIntro.setOutlineThickness(1.5f);
	bounds = m_txtIntro.getLocalBounds();
	m_txtIntro.setOrigin(floor(bounds.width / 2.f), floor(bounds.height / 2.f));

	m_txtDebug = sf::Text("", SceneManager::getDefaultFont(), 16);
	m_txtDebug.setFillColor(sf::Color(255, 255, 255, 255));
	m_txtDebug.setOutlineColor(sf::Color(0, 0, 0, 255));
	m_txtDebug.setOutlineThickness(1.5f);
	m_txtDebug.setPosition(2.f, 0.f);

	m_txtPaused = sf::Text("PAUSED (press spacebar to resume, period to step)", SceneManager::getDefaultFont(), 16);
	m_txtPaused.setFillColor(sf::Color(255, 255, 255, 255));
	m_txtPaused.setOutlineColor(sf::Color(0, 0, 0, 255));
	m_txtPaused.setOutlineThickness(1.5f);
	bounds = m_txtPaused.getLocalBounds();
	m_txtPaused.setOrigin(floor(bounds.width / 2.f), 0.f);
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::finish()
{
	m_sim.reset();

	auto& rw = this->getManager().getWindow();
	rw.setTitle("GOL");

	auto& settings = UserSettings::instance();
	settings.setFloat("steps_per_second", this->getTargetStepsPerSecond());
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
		m_controls.mouseX = ev.mouseMove.x;
		m_controls.mouseY = ev.mouseMove.y;
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
		case sf::Keyboard::G:
			m_showGrid = !m_showGrid;
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
		case sf::Keyboard::Num1:
		case sf::Keyboard::Numpad1:
			cameraSetZoom(1.f);
			break;
		case sf::Keyboard::Num2:
		case sf::Keyboard::Numpad2:
			cameraSetZoom(1.f / 2);
			break;
		case sf::Keyboard::Num3:
		case sf::Keyboard::Numpad3:
			cameraSetZoom(1.f / 3);
			break;
		case sf::Keyboard::Num4:
		case sf::Keyboard::Numpad4:
			cameraSetZoom(1.f / 4);
			break;
		case sf::Keyboard::Num5:
		case sf::Keyboard::Numpad5:
			cameraSetZoom(1.f / 5);
			break;
		case sf::Keyboard::Num6:
		case sf::Keyboard::Numpad6:
			cameraSetZoom(1.f / 6);
			break;
		case sf::Keyboard::Num7:
		case sf::Keyboard::Numpad7:
			cameraSetZoom(1.f / 7);
			break;
		case sf::Keyboard::Num8:
		case sf::Keyboard::Numpad8:
			cameraSetZoom(1.f / 8);
			break;
		case sf::Keyboard::Num9:
		case sf::Keyboard::Numpad9:
			cameraSetZoom(1.f / 9);
			break;
		case sf::Keyboard::LBracket:
			this->setTargetStepsPerSecond(this->getTargetStepsPerSecond() - (m_controls.shift ? 10.f : 1.f));
			std::cout << "step rate: " << this->getTargetStepsPerSecond() << std::endl;
			break;
		case sf::Keyboard::RBracket:
			this->setTargetStepsPerSecond(this->getTargetStepsPerSecond() + (m_controls.shift ? 10.f : 1.f));
			std::cout << "step rate: " << this->getTargetStepsPerSecond() << std::endl;
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
		// Refresh view with current zoom level
		cameraSetZoom(m_cameraZoom);
		break;
	}
}


//////////////////////////////////////////////////////////////////////
int SimulationScene::preUpdate()
{
	float curTime   = this->getManager().getElapsedTime().asSeconds();
	float dt        = curTime - m_lastPreUpdate;
	m_lastPreUpdate = curTime;
	
	bool isMovingCam = m_controls.moveLeft || m_controls.moveRight ||
	                   m_controls.moveUp || m_controls.moveDown;

	if (isMovingCam)
		m_cameraMoveSpeed = std::min(m_cameraMoveSpeed + (1000.f * dt), 1000.f);
	else
		m_cameraMoveSpeed = std::max(m_cameraMoveSpeed - (500.f * dt), 100.f);

	if (m_controls.moveLeft)
		m_camera.move(-m_cameraMoveSpeed * m_cameraZoom * dt, 0.f);
	if (m_controls.moveRight)
		m_camera.move(m_cameraMoveSpeed * m_cameraZoom * dt, 0.f);
	if (m_controls.moveUp)
		m_camera.move(0.f, -m_cameraMoveSpeed * m_cameraZoom * dt);
	if (m_controls.moveDown)
		m_camera.move(0.f, m_cameraMoveSpeed * m_cameraZoom * dt);
	if (m_controls.zoomIn)
		cameraSetZoom(std::max(0.01f, m_cameraZoom *= 1.f - dt));
	if (m_controls.zoomOut)
		cameraSetZoom(std::min(10.f, m_cameraZoom *= 1.f + dt));

	this->screenToWorld(m_controls.mouseX, m_controls.mouseY, m_controls.cursorX, m_controls.cursorY);

	if (m_stepOnce)
	{
		m_stepOnce = false;
		return 1;
	}

	if (m_paused)
		return 0;

	if (m_targetStepsPerSecond > 0)
	{
		m_stepAccumulator += dt;
		float stepTime = (1.f / m_targetStepsPerSecond);
		int steps = static_cast<int>(m_stepAccumulator / stepTime);
		if (steps > 0)
			m_stepAccumulator -= (stepTime * steps);
		return steps;
	}

	return 1;
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::update()
{
	int stepsFinished, steps = preUpdate();
	float curTime = this->getManager().getElapsedTime().asSeconds();
	for (stepsFinished = 0; stepsFinished < steps; stepsFinished++)
	{
		m_sim.step();

		// Break when we've been stepping for too long
		if (this->getManager().getElapsedTime().asSeconds() - curTime > 0.01f)
			break;
	}

	m_debugStepsPerSecond = 0;
	if (steps > 0)
	{
		float elapsedTime = this->getManager().getElapsedTime().asSeconds() - curTime;
		float stepTime = (1.f / elapsedTime) * stepsFinished;
		m_debugStepsPerSecond = std::min(m_targetStepsPerSecond, stepTime);
	}
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::render()
{
	auto& rw = this->getManager().getWindow();
	sf::Vector2u screen = rw.getSize();

	// Apply camera view
	rw.setView(m_camera);

	m_renderer.cullZone.width  = m_camera.getSize().x;
	m_renderer.cullZone.height = m_camera.getSize().y;
	m_renderer.cullZone.left   = m_camera.getCenter().x - m_renderer.cullZone.width / 2;
	m_renderer.cullZone.top    = m_camera.getCenter().y - m_renderer.cullZone.height / 2;
	m_renderer.render();
	
	if (m_showGrid && m_cameraZoom <= CAMERA_ZOOM_SHOW_GRID)
	{
		int pos;
		int left   = static_cast<int>(std::floor(m_renderer.cullZone.left));
		int top    = static_cast<int>(std::floor(m_renderer.cullZone.top));
		int right  = static_cast<int>(std::ceil(m_renderer.cullZone.left + m_renderer.cullZone.width));
		int bottom = static_cast<int>(std::ceil(m_renderer.cullZone.top + m_renderer.cullZone.height));
		
		float alpha = std::min(1.f, (1.f - m_cameraZoom / CAMERA_ZOOM_SHOW_GRID) * 2.f);
		const sf::Color LINE_COLOR1 = sf::Color(0x7F, 0x7F, 0x7F, static_cast<sf::Uint8>(alpha * 0x7Fu));
		const sf::Color LINE_COLOR2 = sf::Color(0x3F, 0x3F, 0x3F, static_cast<sf::Uint8>(alpha * 0x7Fu));

		m_gridLine.setScale(m_cameraZoom, m_cameraZoom);

		// left->right vertical lines
		m_gridLine.setSize(sf::Vector2f(1.f, static_cast<float>(screen.y)));
		for (pos = left; pos <= right; pos++)
		{
			m_gridLine.setPosition(static_cast<float>(pos), m_renderer.cullZone.top);
			m_gridLine.setFillColor((pos % 10 == 0) ? LINE_COLOR1 : LINE_COLOR2);
			rw.draw(m_gridLine);
		}

		// top->bottom horizontal lines
		m_gridLine.setSize(sf::Vector2f(static_cast<float>(screen.x), 1.f));
		for (pos = top; pos <= bottom; pos++)
		{
			m_gridLine.setPosition(m_renderer.cullZone.left, static_cast<float>(pos));
			m_gridLine.setFillColor((pos % 10 == 0) ? LINE_COLOR1 : LINE_COLOR2);
			rw.draw(m_gridLine);
		}
	}

	// Reset camera view
	rw.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(screen.x), static_cast<float>(screen.y))));

	if (!m_hideIntro)
	{
		m_txtIntro.setPosition(static_cast<float>(screen.x / 2), static_cast<float>(screen.y / 2));
		rw.draw(m_txtIntro);
	}

	if (m_debugMode != 0)
		rw.draw(m_txtDebug);

	if (m_paused)
	{
		m_txtPaused.setPosition(static_cast<float>(screen.x / 2), 0.f);
		rw.draw(m_txtPaused);
	}

	if (m_debugMode != 0)
	{
		std::stringstream strDebug;
		strDebug << std::fixed << std::setprecision(2);
		strDebug << "DEBUG (" << m_debugMode << ")";
		
		if (m_sim.isMultithreaded())
			strDebug << "\nMULTITHREADED (" << m_sim.getWorkerThreadCount() << ")";
		
		strDebug << "\nframes/sec  : " << static_cast<int>(this->getManager().getFramesPerSecond());
		if (this->getManager().getTargetFramerate() > 0)
			strDebug << " (target=" << static_cast<int>(this->getManager().getTargetFramerate()) << ")";
		
		strDebug << "\nsteps/sec   : " << static_cast<int>(this->getStepsPerSecond());
		if (this->getTargetStepsPerSecond() > 0)
			strDebug << " (target=" << static_cast<int>(this->getTargetStepsPerSecond()) << ")";

		strDebug << "\nupdate (ms) : " << this->getManager().getProfiledUpdateTime() * 1000.f
		         << "\nrender (ms) : " << this->getManager().getProfiledRenderTime() * 1000.f
		         << "\nchunks      : " << m_sim.getChunkCount()
		         << "\npopulation  : " << m_sim.getPopulation()
		         << "\nbirths      : " << m_sim.getBirths()
		         << "\ndeaths      : " << m_sim.getDeaths()
		         << "\npop delta   : " << (static_cast<long long>(m_sim.getBirths()) - m_sim.getDeaths())
		         << "\ngeneration  : " << m_sim.getGeneration()
		         << "\nruleset     : " << m_sim.getRuleset().getString()
		         << "\ncursor      : " << m_controls.cursorX << ",\t" << m_controls.cursorY
		         << "\nzoom        : " << m_cameraZoom;
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
	out_x = static_cast<int>(std::floor(v.x));
	out_y = static_cast<int>(std::floor(v.y));
}


//////////////////////////////////////////////////////////////////////
void SimulationScene::cameraSetZoom(float zoom)
{
	sf::Vector2u sz = this->getManager().getWindow().getSize();
	m_camera.setSize(static_cast<float>(sz.x), static_cast<float>(sz.y));
	m_camera.zoom(zoom);
	m_cameraZoom = zoom;
}
