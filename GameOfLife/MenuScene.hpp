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
// MenuScene.hpp
// Author: Nathan Cousins
//
// class MenuScene
// 
// Initial scene.
// 

#include "Scene.hpp"
#include "gol/Ruleset.hpp"
#include <sstream>


class MenuScene : public Scene
{
public:
	MenuScene(SceneManager& m) : Scene(m, "Menu") { }

	// Invalidate on-screen text.
	void invalidate();

protected:
	virtual void init() override;
	virtual void finish() override;
	virtual void processEvent(const sf::Event & ev) override;
	virtual void update() override;
	virtual void render() override;

private:
	std::stringstream m_ss;
	sf::Text m_txt;

	// When text is typed.
	// Returns true if on-screen text update is needed.
	bool onText(sf::Uint32 unicode);

	// When a key is pressed down.
	// Returns true if on-screen text update is needed.
	bool onKeyPress(sf::Keyboard::Key key, bool shift, bool ctrl, bool alt, bool syskey);

	// Current menu displayed.
	enum {
		MainMenu,
		ChangeRuleset,
	} m_currentMenu;

	// Information for main menu.
	struct {
		int selection = 0;
	} m_menuMain;

	// Information for change ruleset menu.
	struct {
		int selection = 0;
		gol::Ruleset rules;
		std::string userInput;
		bool isUserInputting = false;
		bool userInputValid = true;
		size_t caretPos;
		void checkValid() {
			gol::Ruleset rules;
			userInputValid = rules.set(userInput);
		}
	} m_menuRuleset;
};
