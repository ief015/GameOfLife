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
	auto& rw = this->getManager()->getWindow();

	m_txt = sf::Text("", SceneManager::getDefaultFont(), 16);
	m_txt.setFillColor(sf::Color::White);

	m_currentMenu = MainMenu;
	this->invalidate();
}


//////////////////////////////////////////////////////////////////////
void MenuScene::finish()
{

}


//////////////////////////////////////////////////////////////////////
void MenuScene::processEvent(const sf::Event & ev)
{
	auto& rw = this->getManager()->getWindow();
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
	{
		sf::FloatRect visibleArea(0, 0, static_cast<float>(ev.size.width), static_cast<float>(ev.size.height));
		rw.setView(sf::View(visibleArea));
		bInvalidate = true;
	} break;

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
	auto& rw = this->getManager()->getWindow();
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
		m_ss << "          GAME OF LIFE          " << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "implementation by nathan cousins" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "      " << (m_menuMain.selection == 0 ? "> " : "  ") << "start (" << gol::Ruleset() << ")" << std::endl;
		m_ss << "      " << (m_menuMain.selection == 1 ? "> " : "  ") << "change ruleset" << std::endl;
		m_ss << "      " << (m_menuMain.selection == 2 ? "> " : "  ") << "exit" << std::endl;
		m_ss << std::endl;
		m_ss << std::endl;
		m_ss << "up/down arrows : change selection" << std::endl;
		m_ss << "        return : make selection" << std::endl;
		break;

	case ChangeRuleset:
		m_ss << "> back <" << std::endl;
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
			if (m_menuMain.selection == 0)
			{
				this->getManager()->load<SimulationScene>();
				this->close();
				return false;
			}
			else if (m_menuMain.selection == 1)
			{
				m_currentMenu = ChangeRuleset;
				return true;
			}
			else if (m_menuMain.selection == 2)
			{
				this->getManager()->closeAll();
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
			m_currentMenu = MainMenu;
			return true;
		}
		break;
	}

	return false;
}
