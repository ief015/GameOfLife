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
// SceneManager.cpp
// Author: Nathan Cousins
//
// class SceneManager
// 
// Implements class SceneManager
// 

#include "SceneManager.hpp"
#include "Scene.hpp"
#include <iostream>
#include <thread>


sf::Font SceneManager::s_defaultFont = sf::Font();


//////////////////////////////////////////////////////////////////////
SceneManager::SceneManager()
	: m_autoWindowClose(true)
	, m_minWindowSize(500, 400)
	, m_targetFPS(60)
	, m_fps(0)
	, m_loadScene(nullptr)
{
	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	videoMode.width  /= 2;
	videoMode.height /= 2;

	sf::Uint32 windowMode = sf::Style::Default;

	if (videoMode.width < 640 || videoMode.height < 480)
		windowMode = sf::Style::Fullscreen;

	m_rw.create(videoMode, "GOL", windowMode);
	m_timestampLastRender = m_clock.getElapsedTime();
}


//////////////////////////////////////////////////////////////////////
SceneManager::~SceneManager()
{
	if (m_loadScene)
		delete m_loadScene;
	for (auto scene : m_scenes)
		scene->finish();
	for (auto scene : m_scenes)
		delete scene;
}


//////////////////////////////////////////////////////////////////////
void SceneManager::closeAll()
{
	for (auto scene : m_scenes)
		scene->close();
}


//////////////////////////////////////////////////////////////////////
const sf::Font& SceneManager::getDefaultFont()
{
	static bool loaded = false;
	if (!loaded)
	{
		auto& settings = UserSettings::instance();
		std::string fontpath = "default.ttf";
		if (!settings.hasKey("font"))
			settings.setString("font", fontpath);
		else
			fontpath = settings.getString("font", fontpath);
		if (!s_defaultFont.loadFromFile(fontpath))
			std::cerr << "could not load font (" << fontpath << ")!" << std::endl;
		loaded = true;
	}
	return s_defaultFont;
}


//////////////////////////////////////////////////////////////////////
void SceneManager::pushAndPopScenes()
{
	// Check for invalid/closed scenes and delete them
	for (auto it = m_scenes.begin(); it != m_scenes.end();)
	{
		Scene* scene = (*it);
		if (!scene->isValid())
		{
			scene->finish();
			it = m_scenes.erase(it);
			std::cout << "closed scene: " << scene->getSceneName() << std::endl;
			delete scene;
		}
		else
		{
			it++;
		}
	}

	// Load a pending scene (via load())
	if (m_loadScene)
	{
		m_loadScene->init();
		m_scenes.push_back(m_loadScene);
		std::cout << "loaded scene: " << m_loadScene->getSceneName() << std::endl;
		m_loadScene = nullptr;
	}
}


//////////////////////////////////////////////////////////////////////
void SceneManager::processEvents()
{
	if (m_scenes.empty())
		return;
	Scene* scene = m_scenes.back();
	if (!scene->isValid())
		return;
	sf::Event ev;
	for (int i = 0; i < 64 && m_rw.pollEvent(ev); i++)
	{
		if (m_autoWindowClose)
		{
			switch (ev.type)
			{
			case sf::Event::Closed:
				this->closeAll();
				scene->processEvent(ev);
				return;
			case sf::Event::Resized:
			{
				bool bMinThresh = false;
				unsigned int nw = ev.size.width, nh = ev.size.height;
				if (nw < m_minWindowSize.x)
				{
					nw = m_minWindowSize.x;
					bMinThresh = true;
				}
				if (nh < m_minWindowSize.y)
				{
					nh = m_minWindowSize.y;
					bMinThresh = true;
				}
				if (!bMinThresh)
				{
					m_rw.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(nw), static_cast<float>(nh))));
					scene->processEvent(ev);
				}
				else
				{
					m_rw.setSize(sf::Vector2u(nw, nh));
				}
			} break;
			default:
				scene->processEvent(ev);
				break;
			}
		}
		else
		{
			scene->processEvent(ev);
		}
	}
}


//////////////////////////////////////////////////////////////////////
void SceneManager::update()
{
	if (m_scenes.empty())
		return;
	Scene* scene = m_scenes.back();
	if (!scene->isValid())
		return;
	scene->update();
}


//////////////////////////////////////////////////////////////////////
void SceneManager::render()
{
	if (m_scenes.empty())
		return;
	Scene* scene = m_scenes.back();
	if (!scene->isValid())
		return;

	// Render scene
	m_rw.clear();
	scene->render();
	m_rw.display();

	// Calculate FPS
	sf::Time ts = m_clock.getElapsedTime();
	m_fps = 1.f / (ts - m_timestampLastRender).asSeconds();
	m_timestampLastRender = ts;

	// Rest
	if (m_targetFPS > 0.f)
	{
		float sleepFor = 1.f / m_targetFPS;
		sf::sleep(sf::seconds(std::max(0.001f, sleepFor)));
	}
}
