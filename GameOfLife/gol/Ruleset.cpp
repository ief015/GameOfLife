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
// gol/Ruleset.cpp
// Author: Nathan Cousins
// 
// Implements class gol::Ruleset
// 

#include "Ruleset.hpp"
#include <sstream>


using namespace gol;


//////////////////////////////////////////////////////////////////////
Ruleset::Ruleset(Preset preset)
	: m_stringOutOfDate(true)
	, m_birthSet { false }
	, m_survivalSet { false }
{
	this->set(preset);
}


//////////////////////////////////////////////////////////////////////
Ruleset::Ruleset(const std::string& bsRule)
	: m_stringOutOfDate(true)
	, m_birthSet { false }
	, m_survivalSet { false }
{
	if (!this->set(bsRule))
		this->set(None);
	
}


//////////////////////////////////////////////////////////////////////
bool Ruleset::set(const std::string& bsRule)
{
	size_t iSep = bsRule.find_first_of('/');

	if (iSep == std::string::npos)
		return false; // error - no separator found

	std::string half1 = bsRule.substr(0, iSep);
	std::string half2 = bsRule.substr(iSep+1);

	if (half1.size() == 0)
		return false; // error - first half is empty
	if (half2.size() == 0)
		return false; // error - second half is empty

	bool newBirths[1 + MAX_NEIGHBOURS] = { false,false,false,false,false,false,false,false,false };
	bool newSurvival[1 + MAX_NEIGHBOURS] = { false,false,false,false,false,false,false,false,false };

	int errFirstHalf = parseHalfSet(half1, newBirths, newSurvival);
	if (errFirstHalf == -1)
		return false; // error parsing first half

	int errSecondHalf = parseHalfSet(half2, newBirths, newSurvival);
	if (errSecondHalf == -1)
		return false; // error parsing second half

	if (errFirstHalf == errSecondHalf)
		return false; // error - both halfs are the same subruleset! (i.e. was either B/B or S/S)

	for (size_t i = 0; i <= MAX_NEIGHBOURS; i++)
	{
		m_birthSet[i]    = newBirths[i];
		m_survivalSet[i] = newSurvival[i];
	}

	m_stringOutOfDate = true;
	return true;
}


//////////////////////////////////////////////////////////////////////
int Ruleset::parseHalfSet(const std::string& half, bool* outBirths, bool* outSurvival)
{
	// I sincerily apologize for this monstrocity of a function.
	// (but it does it's job...)
	// Perhaps utilize regex?

	bool* outSelected = nullptr;
	char cSet = toupper(half[0]);
	if (cSet == 'B')
		outSelected = outBirths;
	else if (cSet == 'S')
		outSelected = outSurvival;
	else
		return -1;

	for (size_t i = 1; i < half.size(); i++)
	{
		char c = half[i];
		if (!isdigit(c))
			return -1;
		int d = c - '0'; //> 'c' to int 0-9
		if (d > Ruleset::MAX_NEIGHBOURS)
			continue;
		if (cSet == 'B')
		{
			if (d == 0)
				continue; //> Simulation does not support b0 (birth on zero neighbours)
		}
		outSelected[d] = true;
	}

	if (cSet == 'B')
		return 0;
	if (cSet == 'S')
		return 1;
	return -1;
}


//////////////////////////////////////////////////////////////////////
void Ruleset::set(Preset preset)
{
	switch (preset)
	{
	default:
		this->setBirth(false, false, false, false, false, false, false, false);
		this->setSurvival(false, false, false, false, false, false, false, false, false);
		break;
	case GameOfLife:
		this->set("B3/S23");
		break;
	case HighLife:
		this->set("B36/S23");
		break;
	case ThirtyFourLife:
		this->set("B34/S34");
		break;
	case Replicator:
		this->set("B1357/S1357");
		break;
	case Fredkin:
		this->set("B1357/S02468");
		break;
	case Seeds:
		this->set("B2/S");
		break;
	case LiveFreeOrDie:
		this->set("B2/S0");
		break;
	case B25S4:
		this->set("B25/S4");
		break;
	case Flock:
		this->set("B3/S12");
		break;
	case LifeWithoutDeath:
		this->set("B2/S012345678");
		break;
	case Diamoeba:
		this->set("B35678/S5678");
		break;
	case TwoByTwo:
		this->set("B36/S125");
		break;
	case DayAndNight:
		this->set("B3678/S34678");
		break;
	case DryLife:
		this->set("B37/S23");
		break;
	case Move:
		this->set("B368/S245");
		break;
	case Anneal:
		this->set("B4678/S35678");
		break;
	case Mazectric:
		this->set("B3/S1234");
		break;
	case Maze:
		this->set("B3/S12345");
		break;
	case PedestrianLife:
		this->set("B38/S23");
		break;
	}
	m_stringOutOfDate = true;
}


//////////////////////////////////////////////////////////////////////
void Ruleset::setBirth(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	m_birthSet[0] = false; // B0 not supported
	m_birthSet[1] = b1;
	m_birthSet[2] = b2;
	m_birthSet[3] = b3;
	m_birthSet[4] = b4;
	m_birthSet[5] = b5;
	m_birthSet[6] = b6;
	m_birthSet[7] = b7;
	m_birthSet[8] = b8;

	m_stringOutOfDate = true;
}


//////////////////////////////////////////////////////////////////////
void Ruleset::setSurvival(bool s0, bool s1, bool s2, bool s3, bool s4, bool s5, bool s6, bool s7, bool s8)
{
	m_survivalSet[0] = s0;
	m_survivalSet[1] = s1;
	m_survivalSet[2] = s2;
	m_survivalSet[3] = s3;
	m_survivalSet[4] = s4;
	m_survivalSet[5] = s5;
	m_survivalSet[6] = s6;
	m_survivalSet[7] = s7;
	m_survivalSet[8] = s8;

	m_stringOutOfDate = true;
}


//////////////////////////////////////////////////////////////////////
void Ruleset::invalidateString() const
{
	if (!m_stringOutOfDate)
		return;
	std::stringstream buf;
	buf << "B";
	for (size_t i = 0; i <= MAX_NEIGHBOURS; i++)
	{
		if (m_birthSet[i])
			buf << i;
	}
	buf << "/S";
	for (size_t i = 0; i <= MAX_NEIGHBOURS; i++)
	{
		if (m_survivalSet[i])
			buf << i;
	}
	m_ruleString = buf.str();
	m_stringOutOfDate = false;
}
