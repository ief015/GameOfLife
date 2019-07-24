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
// main.cpp
// Author: Nathan Cousins
// 
// Application entry point and main program loop. Controls the state and
// lifetime of the program.
// 

#include "gol/Simulation.hpp"
#include "SimulationRenderer.hpp"

#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>


void process_event(const sf::Event& ev);
bool pre_update();
void toggle_debug(int mode);
void screen_to_world(int scr_x, int scr_y, int& out_x, int& out_y);
void place_cells(int x, int y, int size, bool alive);


float updatesPerSecond = 60;
bool paused    = true;
bool stepOnce  = false;
bool flagExit  = false;
int debugMode  = 0;

struct {
	bool mouseLeft  = false;
	bool mouseRight = false;
	bool shift      = false;
	bool moveUp     = false;
	bool moveDown   = false;
	bool moveLeft   = false;
	bool moveRight  = false;
	bool zoomIn     = false;
	bool zoomOut    = false;
} controls;

sf::View camera;
sf::RenderWindow rw;
gol::Simulation sim;
SimulationRenderer renderer;


/* TODO main menu?
GAME OF LIFE

implementation by nathan cousins


	  > play (B3/S23)
		change ruleset
		exit

up/down arrows: change selection
enter: make selection
*/


//////////////////////////////////////////////////////////////////////
int main()
{
	sf::Clock clk;
	sf::Event ev;
	sf::Time updateTimestamp;

	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	videoMode.width /= 2;
	videoMode.height /= 2;

	rw.create(videoMode, "GOL", sf::Style::Default);
	rw.setVerticalSyncEnabled(true);
	camera = rw.getDefaultView();
	renderer.setRenderTarget(rw);
	renderer.setSimulation(sim);

	// TODO: add user ability to change ruleset
	sim.setRuleset(gol::Ruleset("B45678/S2345"));
	//sim.setRuleset(gol::Ruleset::GameOfLife);
	rw.setTitle(std::string("GOL - ") + sim.getRuleset().getString());

	sf::Text txtDebug("", renderer.getDefaultFont(), 16);
	txtDebug.setFillColor(sf::Color(255, 255, 255, 255));
	txtDebug.setOutlineColor(sf::Color(0, 0, 0, 255));
	txtDebug.setOutlineThickness(1.5f);
	sf::Time tsDebugUpdate;
	sf::Time tsDebugRender;

	clk.restart();
	while (rw.isOpen())
	{
		while (rw.pollEvent(ev))
			process_event(ev);

		if (flagExit)
			break;

		if ((clk.getElapsedTime() - updateTimestamp).asSeconds() >= 1.f / updatesPerSecond)
		{
			updateTimestamp = clk.getElapsedTime();
			if (pre_update())
			{
				tsDebugUpdate = clk.getElapsedTime();
				sim.step();
				tsDebugUpdate = clk.getElapsedTime() - tsDebugUpdate;
				stepOnce = false;
			}
		}

		tsDebugRender = clk.getElapsedTime();
		rw.clear();
		rw.setView(camera);

		renderer.render();

		rw.setView(rw.getDefaultView());
		if (debugMode != 0)
			rw.draw(txtDebug);

		rw.display();
		tsDebugRender = clk.getElapsedTime() - tsDebugRender;

		if (debugMode != 0)
		{
			std::stringstream strDebug;
			strDebug << std::fixed << std::setprecision(2)
				<< "DEBUG\nupdate (ms) : " << tsDebugUpdate.asSeconds() * 1000.f
				<< "\nrender (ms) : " << tsDebugRender.asSeconds() * 1000.f
				<< "\nchunks : " << sim.getChunkCount()
				<< "\npopulation : " << sim.getPopulation()
				<< "\nbirths : " << sim.getBirths()
				<< "\ndeaths : " << sim.getDeaths()
				<< "\npop delta : " << (static_cast<long long>(sim.getBirths()) - sim.getDeaths())
				<< "\ngeneration : " << sim.getGeneration()
				<< "\nruleset : " << sim.getRuleset().getString();
			if (paused)
				strDebug << "\nPAUSED";
			if (sim.isMultithreaded())
				strDebug << "\nMULTITHREADED (" << sim.getWorkerThreadCount() << ")";
			txtDebug.setString(strDebug.str());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//sf::sleep(sf::milliseconds(1));

	} // while (rw.isOpen())

	rw.close();
	return 0;
}


//////////////////////////////////////////////////////////////////////
void process_event(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::MouseButtonPressed:
		if (ev.mouseButton.button == sf::Mouse::Left)
		{
			controls.mouseLeft = true;
			int x, y;
			screen_to_world(ev.mouseButton.x, ev.mouseButton.y, x, y);
			place_cells(x, y, 1, !controls.shift);
		}
		if (ev.mouseButton.button == sf::Mouse::Right)
		{
			controls.mouseRight = true;
			int x, y;
			screen_to_world(ev.mouseButton.x, ev.mouseButton.y, x, y);
			place_cells(x, y, 20, !controls.shift);
		}
		break;

	case sf::Event::MouseButtonReleased:
		if (ev.mouseButton.button == sf::Mouse::Left)
			controls.mouseLeft = false;
		else if (ev.mouseButton.button == sf::Mouse::Right)
			controls.mouseRight = false;
		break;

	case sf::Event::MouseMoved:
		if (controls.mouseLeft)
		{
			int x, y;
			screen_to_world(ev.mouseMove.x, ev.mouseMove.y, x, y);
			place_cells(x, y, 1, !controls.shift);
		}
		else if (controls.mouseRight)
		{
			int x, y;
			screen_to_world(ev.mouseMove.x, ev.mouseMove.y, x, y);
			place_cells(x, y, 20, !controls.shift);
		}
		break;

	case sf::Event::KeyPressed:
		controls.shift = ev.key.shift;
		switch (ev.key.code)
		{
		case sf::Keyboard::Space:
			paused = !paused;
			break;
		case sf::Keyboard::Escape:
			flagExit = true;
			break;
		case sf::Keyboard::R:
			sim.reset();
			break;
		case sf::Keyboard::Tilde:
			toggle_debug(++debugMode);
			break;
		case sf::Keyboard::T:
			sim.setMultithreadMode(!sim.isMultithreaded());
			break;
		case sf::Keyboard::Period:
			if (paused) stepOnce = true;
			break;
		case sf::Keyboard::Left:
			controls.moveLeft = true;
			break;
		case sf::Keyboard::Right:
			controls.moveRight = true;
			break;
		case sf::Keyboard::Up:
			controls.moveUp = true;
			break;
		case sf::Keyboard::Down:
			controls.moveDown = true;
			break;
		case sf::Keyboard::Equal:
		case sf::Keyboard::Add:
			controls.zoomIn = true;
			break;
		case sf::Keyboard::Hyphen:
		case sf::Keyboard::Subtract:
			controls.zoomOut = true;
			break;
		}
		break;

	case sf::Event::KeyReleased:
		controls.shift = ev.key.shift;
		switch (ev.key.code)
		{
		case sf::Keyboard::Left:
			controls.moveLeft = false;
			break;
		case sf::Keyboard::Right:
			controls.moveRight = false;
			break;
		case sf::Keyboard::Up:
			controls.moveUp = false;
			break;
		case sf::Keyboard::Down:
			controls.moveDown = false;
			break;
		case sf::Keyboard::Equal:
		case sf::Keyboard::Add:
			controls.zoomIn = false;
			break;
		case sf::Keyboard::Hyphen:
		case sf::Keyboard::Subtract:
			controls.zoomOut = false;
			break;
		}
		break;

	case sf::Event::Resized: 
	{
		camera.setSize(static_cast<float>(ev.size.width), static_cast<float>(ev.size.height));
	} break;

	case sf::Event::Closed:
		flagExit = true;
		break;
	}
}


//////////////////////////////////////////////////////////////////////
bool pre_update()
{
	if (controls.moveLeft)
		camera.move(-3.f, 0.f);
	if (controls.moveRight)
		camera.move(3.f, 0.f);
	if (controls.moveUp)
		camera.move(0.f, -3.f);
	if (controls.moveDown)
		camera.move(0.f, 3.f);
	if (controls.zoomIn)
		camera.zoom(1.f - 0.025f);
	if (controls.zoomOut)
		camera.zoom(1.f + 0.025f);

	if (stepOnce)
	{
		stepOnce = false;
		return true;
	}
	return !paused;
}


//////////////////////////////////////////////////////////////////////
void screen_to_world(int scr_x, int scr_y, int& out_x, int& out_y)
{
	sf::Vector2f v = rw.mapPixelToCoords(sf::Vector2i(scr_x, scr_y), camera);
	out_x = static_cast<int>(v.x);
	out_y = static_cast<int>(v.y);
}


//////////////////////////////////////////////////////////////////////
void place_cells(int x, int y, int size, bool alive)
{
	if (size <= 1)
	{
		sim.setCell(x, y, alive);
	}
	else
	{
		for (int ox = -(size - 1); ox <= (size - 1); ox++)
			for (int oy = -(size - 1); oy <= (size - 1); oy++)
				sim.setCell(x + ox, y + oy, alive);
	}
}


//////////////////////////////////////////////////////////////////////
void toggle_debug(int mode)
{
	debugMode = mode;

	switch (mode)
	{
	default:
		debugMode = 0;
		renderer.showChunks = false;
		renderer.showChunksCellCount = false;
		renderer.showChunkID = false;
		break;

	case 1:
		renderer.showChunks = false;
		renderer.showChunksCellCount = false;
		renderer.showChunkID = false;
		break;

	case 2:
		renderer.showChunks = true;
		renderer.showChunksCellCount = false;
		renderer.showChunkID = false;
		break;

	case 3:
		renderer.showChunks = true;
		renderer.showChunksCellCount = true;
		renderer.showChunkID = true;
		break;
	}
}
