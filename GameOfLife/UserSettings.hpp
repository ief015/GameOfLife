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
// UserSettings.hpp
// Author: Nathan Cousins
//
// class UserSettings
// 
// Provides utilties to load and save persistant data.
// 

#include <map>
#include <string>


class UserSettings final
{
public:
	typedef std::string TKey;
	typedef std::string TValue;

	UserSettings();
	~UserSettings();

	bool load(const std::string& path = "settings.cfg");
	bool save(const std::string& path = "settings.cfg");

	std::string getString(const TKey& key) const;
	std::string getString(const TKey& key, const std::string& defaultValue) const;
	int getInteger(const TKey& key) const;
	int getInteger(const TKey& key, int defaultValue) const;
	float getFloat(const TKey& key) const;
	float getFloat(const TKey& key, float defaultValue) const;

	void setString(const TKey& key, const std::string& val);
	void setInteger(const TKey& key, int val);
	void setFloat(const TKey& key, float val);

	// Returns true if key exists.
	bool hasKey(const TKey& key) const;

private:
	// load() will write data from settings tile to this map.
	std::map<TKey, TValue> m_loadedData;

	// Cached string values.
	mutable std::map<TKey, std::string> m_strCache;
	// Cached integer values.
	mutable std::map<TKey, int> m_intCache;
	// Cached float values.
	mutable std::map<TKey, float> m_fltCache;
};
