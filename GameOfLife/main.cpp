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
// Application entry point and main program loop. Controls the state of the
// Scene Manager, and ultimately the lifetime of the program.
// 

#include "MenuScene.hpp"
#include "Version.hpp"
#include <iostream>

//////////////////////////////////////////////////////////////////////
int main()
{
	std::cout << "game of life v" << VERSION_STRING << std::endl << std::endl;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	SceneManager sceneManager;
	sceneManager.load<MenuScene>();
	UserSettings::instance().load();
	do {
		sceneManager.update();
		sceneManager.render();
	} while (!sceneManager.empty());
	UserSettings::instance().save();
	return 0;
}
