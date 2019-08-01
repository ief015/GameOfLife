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
// MenuScene.cpp
// Author: Nathan Cousins
//
// class MenuScene
// 
// Implements class MenuScene
// 

#include "MenuScene.hpp"
#include "SimulationScene.hpp"
#include "gol/Ruleset.hpp"


//////////////////////////////////////////////////////////////////////
void MenuScene::init()
{
	auto& rw = this->getManager().getWindow();
	auto& settings = UserSettings::instance();

	m_txt = sf::Text("", SceneManager::getDefaultFont(), 16);
	m_txt.setFillColor(sf::Color::White);

	// Default is B3/S23 (Conway's Game of Life)
	m_menuRuleset.rules.set(gol::Ruleset::GameOfLife);
	if (!settings.hasKey("ruleset") || !m_menuRuleset.rules.set(settings.getString("ruleset")))
		settings.setString("ruleset", m_menuRuleset.rules.getString());
	m_menuRuleset.userInput = m_menuRuleset.rules;
	
	this->invalidate();
}


//////////////////////////////////////////////////////////////////////
void MenuScene::finish()
{
	auto& settings = UserSettings::instance();
	settings.setString("ruleset", m_menuRuleset.rules.getString());
}


//////////////////////////////////////////////////////////////////////
void MenuScene::processEvent(const sf::Event & ev)
{
	auto& rw = this->getManager().getWindow();
	bool bInvalidate = false;

	switch (ev.type)
	{

	case sf::Event::TextEntered:
		bInvalidate = this->onText(ev.text.unicode);
		break;

	case sf::Event::KeyPressed:
		bInvalidate = this->onKeyPress(ev.key.code, ev.key.shift, ev.key.control, ev.key.alt, ev.key.system);
		break;

	case sf::Event::Resized:
		bInvalidate = true;
		break;

	}

	if (bInvalidate)
		this->invalidate();
}


//////////////////////////////////////////////////////////////////////
void MenuScene::update()
{

}


//////////////////////////////////////////////////////////////////////
void MenuScene::render()
{
	auto& rw = this->getManager().getWindow();
	auto screen = rw.getSize();
	m_txt.setPosition(static_cast<float>(screen.x / 2), static_cast<float>(screen.y / 2));
	rw.draw(m_txt);
}


//////////////////////////////////////////////////////////////////////
void MenuScene::invalidate()
{
	// Clear stringstream
	std::stringstream().swap(m_ss);

	switch (m_currentMenu)
	{

	case MainMenu:
		m_ss << "          GAME OF LIFE" << std::endl;
		m_ss << "      a cellular automaton" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "implementation by nathan cousins" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "      " << (m_menuMain.selection == 0 ? "> " : "  ") << "start (" << m_menuRuleset.rules << ")" << std::endl;
		m_ss << "      " << (m_menuMain.selection == 1 ? "> " : "  ") << "change ruleset" << std::endl;
		m_ss << "      " << (m_menuMain.selection == 2 ? "> " : "  ") << "exit" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "up/down arrows : change selection" << std::endl;
		m_ss << "        return : make selection" << std::endl;
		break;

	case ChangeRuleset:
		m_ss << "          RULESET" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		if (m_menuRuleset.userInputValid)
			m_ss << std::endl;
		else
			m_ss << "   invalid B/S rulestring" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "      " << (m_menuRuleset.selection == 0 ? "> " : "  ") << "enter ruleset";
		if (m_menuRuleset.isUserInputting)
		{
			std::string withCaret(m_menuRuleset.userInput);
			withCaret.insert(m_menuRuleset.caretPos, 1, '_');
			m_ss << ": " << withCaret << std::endl;
		}
		else
			m_ss << " (" << m_menuRuleset.rules << ")" << std::endl;
		m_ss << "      " << (m_menuRuleset.selection == 1 ? "> " : "  ") << "back" << std::endl;
		
		break;
	}

	m_txt.setString(m_ss.str());
	sf::FloatRect bounds = m_txt.getLocalBounds();
	m_txt.setOrigin(floor(bounds.width / 2.f), floor(bounds.height / 2.f));
}


//////////////////////////////////////////////////////////////////////
bool MenuScene::onText(sf::Uint32 unicode)
{
	switch (m_currentMenu)
	{
	case MainMenu:
		break;

	case ChangeRuleset:
		if (m_menuRuleset.isUserInputting)
		{
			std::string str = sf::String(unicode).toAnsiString();
			if (!str.empty())
			{
				char c = str[0];
				if (isdigit(c) || c == '/' || c == 'b' || c == 's')
				{
					m_menuRuleset.userInput.insert(m_menuRuleset.caretPos, 1, c);
					m_menuRuleset.checkValid();
					m_menuRuleset.caretPos++;
					return true;
				}
			}
		}
		break;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////
bool MenuScene::onKeyPress(sf::Keyboard::Key key, bool shift, bool ctrl, bool alt, bool syskey)
{
	switch (m_currentMenu)
	{

	//////////////////////////////////////////////////////////////
	case MainMenu:
		if (key == sf::Keyboard::Return)
		{
			if (m_menuMain.selection == 0) //> start
			{
				this->getManager().load<SimulationScene>();
				this->close();
				return false;
			}
			else if (m_menuMain.selection == 1) //> change ruleset
			{
				m_currentMenu = ChangeRuleset;
				return true;
			}
			else if (m_menuMain.selection == 2) //> exit
			{
				this->getManager().closeAll();
				return false;
			}
		}
		else if (key == sf::Keyboard::Down)
		{
			if (m_menuMain.selection < 2)
			{
				m_menuMain.selection++;
				return true;
			}
		}
		else if (key == sf::Keyboard::Up)
		{
			if (m_menuMain.selection > 0)
			{
				m_menuMain.selection--;
				return true;
			}
		}
		break;

	//////////////////////////////////////////////////////////////
	case ChangeRuleset:
		if (key == sf::Keyboard::Return)
		{
			if (m_menuRuleset.selection == 0) // enter ruleset
			{
				if (m_menuRuleset.isUserInputting)
				{
					if (m_menuRuleset.userInputValid)
						m_menuRuleset.rules.set(m_menuRuleset.userInput);
					m_menuRuleset.userInput = m_menuRuleset.rules;
					m_menuRuleset.userInputValid = true;
				}
				else
				{
					m_menuRuleset.caretPos = m_menuRuleset.userInput.size();
				}
				m_menuRuleset.isUserInputting = !m_menuRuleset.isUserInputting;
				return true;
			}
			else if (m_menuRuleset.selection == 1) // back
			{
				m_currentMenu = MainMenu;
				return true;
			}
		}
		else if (key == sf::Keyboard::Down && !m_menuRuleset.isUserInputting)
		{
			if (m_menuRuleset.selection < 1)
			{
				m_menuRuleset.selection++;
				return true;
			}
		}
		else if (key == sf::Keyboard::Up && !m_menuRuleset.isUserInputting)
		{
			if (m_menuRuleset.selection > 0)
			{
				m_menuRuleset.selection--;
				return true;
			}
		}
		else if (key == sf::Keyboard::Backspace && m_menuRuleset.isUserInputting)
		{
			if (!m_menuRuleset.userInput.empty() && m_menuRuleset.caretPos > 0)
			{
				m_menuRuleset.userInput.erase(m_menuRuleset.userInput.begin() + m_menuRuleset.caretPos - 1);
				m_menuRuleset.checkValid();
				m_menuRuleset.caretPos--;
				return true;
			}
		}
		else if (key == sf::Keyboard::Left && m_menuRuleset.isUserInputting)
		{
			if (m_menuRuleset.caretPos > 0)
				m_menuRuleset.caretPos--;
			return true;
		}
		else if (key == sf::Keyboard::Right && m_menuRuleset.isUserInputting)
		{
			if (m_menuRuleset.caretPos < m_menuRuleset.userInput.size())
				m_menuRuleset.caretPos++;
			return true;
		}
		else if (key == sf::Keyboard::Home && m_menuRuleset.isUserInputting)
		{
			m_menuRuleset.caretPos = 0;
			return true;
		}
		else if (key == sf::Keyboard::End && m_menuRuleset.isUserInputting)
		{
			m_menuRuleset.caretPos = m_menuRuleset.userInput.size();
			return true;
		}
		break;
	}

	return false;
}
