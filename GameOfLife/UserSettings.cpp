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
// UserSettings.cpp
// Author: Nathan Cousins
//
// class UserSettings
// 
// Implements class UserSettings.
// 


#include "UserSettings.hpp"
#include <iostream>


//////////////////////////////////////////////////////////////////////
UserSettings::UserSettings()
{

}


//////////////////////////////////////////////////////////////////////
UserSettings::~UserSettings()
{

}


//////////////////////////////////////////////////////////////////////
bool UserSettings::load(const std::string& path)
{
	m_strCache.clear();
	m_intCache.clear();
	m_fltCache.clear();

	// TODO: UserSettings::load
	std::cout << "UserSettings::load not yet implemented" << std::endl;
	return false;
}


//////////////////////////////////////////////////////////////////////
bool UserSettings::save(const std::string& path)
{
	// TODO: UserSettings::save
	std::cout << "UserSettings::save not yet implemented" << std::endl;
	return false;
}


//////////////////////////////////////////////////////////////////////
std::string UserSettings::getString(const TKey& k) const
{
	auto it = m_strCache.find(k);
	if (it == m_strCache.end())
	{
		auto it2 = m_loadedData.find(k);
		if (it == m_strCache.end())
		{
			std::cerr << "attempt to get string for nonexistent user setting: key=" << k << std::endl;
			return std::string();
		}
		// Add to cache and return value.
		return m_strCache.emplace(k, it2->second).first->second;
	}
	return it->second;
}


//////////////////////////////////////////////////////////////////////
std::string UserSettings::getString(const TKey& k, const std::string& defaultValue) const
{
	auto it = m_strCache.find(k);
	if (it == m_strCache.end())
	{
		auto it2 = m_loadedData.find(k);
		if (it == m_strCache.end())
			return defaultValue;
		// Add to cache and return value.
		return m_strCache.emplace(k, it2->second).first->second;
	}
	return it->second;
}


//////////////////////////////////////////////////////////////////////
int UserSettings::getInteger(const TKey& k) const
{
	auto it = m_intCache.find(k);
	if (it == m_intCache.end())
	{
		auto it2 = m_loadedData.find(k);
		if (it == m_intCache.end())
		{
			std::cerr << "attempt to get integer for nonexistent user setting: key=" << k << std::endl;
			return 0;
		}
		int val = std::atoi(it2->second.c_str());
		// Add to cache and return value.
		return m_intCache.emplace(k, val).first->second;
	}
	return it->second;
}


//////////////////////////////////////////////////////////////////////
int UserSettings::getInteger(const TKey& k, int defaultValue) const
{
	auto it = m_intCache.find(k);
	if (it == m_intCache.end())
	{
		auto it2 = m_loadedData.find(k);
		if (it == m_intCache.end())
			return defaultValue;
		int val = std::atoi(it2->second.c_str());
		// Add to cache and return value.
		return m_intCache.emplace(k, val).first->second;
	}
	return it->second;
}


//////////////////////////////////////////////////////////////////////
float UserSettings::getFloat(const TKey& k) const
{
	auto it = m_fltCache.find(k);
	if (it == m_fltCache.end())
	{
		auto it2 = m_loadedData.find(k);
		if (it == m_fltCache.end())
		{
			std::cerr << "attempt to get float for nonexistent user setting: key=" << k << std::endl;
			return 0;
		}
		float val = static_cast<float>(std::atof(it2->second.c_str()));
		// Add to cache and return value.
		return m_fltCache.emplace(k, val).first->second;
	}
	return it->second;
}


//////////////////////////////////////////////////////////////////////
float UserSettings::getFloat(const TKey& k, float defaultValue) const
{
	auto it = m_fltCache.find(k);
	if (it == m_fltCache.end())
	{
		auto it2 = m_loadedData.find(k);
		if (it == m_fltCache.end())
			return defaultValue;
		float val = static_cast<float>(std::atof(it2->second.c_str()));
		// Add to cache and return value.
		return m_fltCache.emplace(k, val).first->second;
	}
	return it->second;
}


//////////////////////////////////////////////////////////////////////
void UserSettings::setString(const TKey& key, const std::string& val)
{
	m_strCache[key] = val;
}


//////////////////////////////////////////////////////////////////////
void UserSettings::setInteger(const TKey& key, int val)
{
	m_intCache[key] = val;

	
}


//////////////////////////////////////////////////////////////////////
void UserSettings::setFloat(const TKey& key, float val)
{
	m_fltCache[key] = val;
}


//////////////////////////////////////////////////////////////////////
bool UserSettings::hasKey(const TKey& key) const
{
	if (m_loadedData.find(key) != m_loadedData.end())
		return true;
	if (m_strCache.find(key) != m_strCache.end())
		return true;
	if (m_intCache.find(key) != m_intCache.end())
		return true;
	if (m_fltCache.find(key) != m_fltCache.end())
		return true;
	return false;
}
