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
// SimulationScene.hpp
// Author: Nathan Cousins
// 
// Scene for running the simulation.
// 

#include "Scene.hpp"
#include "gol/Simulation.hpp"
#include "SimulationRenderer.hpp"


class SimulationScene : public Scene
{
public:
	SimulationScene(SceneManager& m)
		: Scene(m, "Simulation")
		, m_paused(true)
		, m_stepOnce(false)
		, m_hideIntro(false)
		, m_debugMode(0)
		, m_cameraZoom(1.f)
		, m_cameraMoveSpeed(1.f)
		, m_targetStepsPerSecond(60.f)
		, m_debugStepsPerSecond(0.f)
		, m_lastPreUpdate(0.f)
		, m_stepAccumulator(0.f)
	{ }

	// Get steps/second performance.
	inline float getStepsPerSecond() const { return m_debugStepsPerSecond; }

	// Get the target simulation steps/second.
	inline float getTargetStepsPerSecond() const { return m_targetStepsPerSecond; }

	// Set the target simulation steps/second.
	// Setting to 0 disables update limiting and will step once per frame.
	inline void setTargetStepsPerSecond(float updateRate) { m_targetStepsPerSecond = std::max(0.f, updateRate); }

protected:
	virtual void init()   override;
	virtual void finish() override;
	virtual void processEvent(const sf::Event & ev) override;
	virtual void update() override;
	virtual void render() override;

private:
	gol::Simulation    m_sim;
	SimulationRenderer m_renderer;

	sf::View m_camera;
	float    m_cameraZoom;
	float    m_cameraMoveSpeed;

	bool m_paused;
	bool m_stepOnce;

	int   m_debugMode;
	float m_debugStepsPerSecond;
	float m_targetStepsPerSecond;

	sf::Text m_txtIntro;
	bool     m_hideIntro;
	sf::Text m_txtDebug;
	sf::Text m_txtPaused;

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
	} m_controls;
	
	float m_lastPreUpdate;
	float m_stepAccumulator;
	int preUpdate();
	void toggleDebug(int mode);
	void placeCells(int x, int y, int size, bool alive);
	void screenToWorld(int scr_x, int scr_y, int& out_x, int& out_y);
	void cameraSetZoom(float zoom);
};
