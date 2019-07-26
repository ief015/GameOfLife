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
// SceneManager.hpp
// Author: Nathan Cousins
//
// class SceneManager
// 
// Controls the loading, closing, updating, and otherwise management of Scenes.
// 

#include <SFML/Graphics.hpp>
#include <list>

class Scene;
class SceneManager final
{
public:
	SceneManager();
	~SceneManager();

	// Get the currently opened scene.
	inline Scene* getCurrentScene() { return (m_scenes.empty() ? nullptr : m_scenes.back()); }
	// Get the currently opened scene.
	inline const Scene* getCurrentScene() const { return (m_scenes.empty() ? nullptr : m_scenes.back()); }

	// Creates and load a scene of type 'TScene'.
	template<class TScene>
	void load() {
		if (m_loadScene) delete m_loadScene;
		m_loadScene = new TScene(this);
	}

	// Returns true if no scenes are loaded.
	inline bool empty() const { return m_scenes.empty(); }

	// Close all scenes.
	void closeAll();

	// Push scenes if a scene is ready to be loaded. Invalid/closed scenes will be removed and destroyed.
	// init() and finish() are called here.
	void pushAndPopScenes();
	// Poll user events and pass to current scene.
	// processEvents() is called here.
	void processEvents();
	// Update current scene.
	// update() is called here.
	void update();
	// Render current scene.
	// render() is called here.
	void render();

	// Get the render window.
	inline sf::RenderWindow& getWindow() { return m_rw; }
	// Get the render window.
	inline const sf::RenderWindow& getWindow() const { return m_rw; }

	// Get the default loaded font.
	static const sf::Font& getDefaultFont();

	// If true, window Close event will automatically close all open scenes and exit application.
	// Enabled by default.
	inline void setAutoWindowClose(bool enable = true) { m_autoWindowClose = enable; }

	// Get if automatic close event is enabled. See setAutoWindowClose.
	inline bool getAutoWindowClose() const { return m_autoWindowClose; }

private:
	static sf::Font s_defaultFont;
	sf::RenderWindow m_rw;

	std::list<Scene*> m_scenes;
	Scene* m_loadScene;

	bool m_autoWindowClose;
};
