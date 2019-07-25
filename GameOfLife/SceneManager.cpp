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


sf::Font SceneManager::s_defaultFont = sf::Font();


//////////////////////////////////////////////////////////////////////
SceneManager::SceneManager()
{
	sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
	videoMode.width  /= 2;
	videoMode.height /= 2;

	m_rw.create(videoMode, "GOL", sf::Style::Default);
	m_rw.setVerticalSyncEnabled(true);
}


//////////////////////////////////////////////////////////////////////
SceneManager::~SceneManager()
{
	
}


//////////////////////////////////////////////////////////////////////
const sf::Font& SceneManager::getDefaultFont()
{
	static bool loaded = false;
	if (!loaded)
	{
		// TODO: font path shouldn't be hardcoded
		const std::string fontpath = "ProggyCleanSZ.ttf";
		if (!s_defaultFont.loadFromFile(fontpath))
		{
			std::cerr << "could not load font (" << fontpath << ")!" << std::endl;
		}
		loaded = true;
	}
	return s_defaultFont;
}


//////////////////////////////////////////////////////////////////////
void SceneManager::pushAndPopScenes()
{
	// Load a pending scene (via load())
	if (m_loadScene)
	{
		m_loadScene->init();
		m_scenes.push_back(m_loadScene);
		m_loadScene = nullptr;
	}

	// Check for invalid/closed scenes and delete them
	for (auto it = m_scenes.begin(); it != m_scenes.end();)
	{
		Scene* scene = (*it);
		if (!scene->isValid())
		{
			scene->finish();
			it = m_scenes.erase(it);
			delete scene;
		}
		else
		{
			it++;
		}
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
	for (int i = 0; i < 32 && m_rw.pollEvent(ev); i++)
		scene->processEvent(ev);
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
	m_rw.clear();
	if (!m_scenes.empty())
	{
		Scene* scene = m_scenes.back();
		if (scene->isValid())
			scene->render();
	}
	m_rw.display();
}


//////////////////////////////////////////////////////////////////////
void SceneManager::closeAll()
{
	for (auto scene : m_scenes)
		scene->close();
}
