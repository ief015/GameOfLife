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
// Scene.hpp
// Author: Nathan Cousins
//
// class Scene
// 
// Abstraction interface for implementing various scenes.
// 

#include <SFML/Window.hpp>
#include "SceneManager.hpp"

class Scene
{
public:
	// Notify the Scene Manager to close this scene.
	void close() { m_closed = true; }

	// Returns true if this scene has a parent Scene Manager and is not flagged to close.
	inline bool isValid() const { return (m_parent != nullptr) && (!m_closed); }

	// Get the name of the scene.
	inline const char* getSceneName() const { return m_sceneName; }

	// Get the parent Scene Manager.
	inline SceneManager* getManager() { return m_parent; }
	// Get the parent Scene Manager.
	inline const SceneManager* getManager() const { return m_parent; }

protected:
	friend SceneManager;
	Scene(SceneManager* m, const char* sceneName = "")
		: m_parent(m), m_closed(false), m_sceneName(sceneName) { }

	// Called before the first update().
	virtual void init() = 0;
	// Called when this scene is closed and ready to be destroyed.
	virtual void finish() = 0;
	// Called when polling user events.
	virtual void processEvent(const sf::Event& ev) = 0;
	// Called very often to update scene logic.
	virtual void update() = 0;
	// Called once per frame to render the scene.
	virtual void render() = 0;


private:
	SceneManager* m_parent;
	const char* m_sceneName;
	bool m_closed;
};
