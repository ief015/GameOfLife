#pragma once
#include <string>


namespace gol
{

class Ruleset
{
public:
	static const size_t MAX_NEIGHBOURS = 8;

	enum Preset
	{
		// B/S
		None,

		// B3/S23
		GameOfLife,

		// B36/S23
		HighLife,

		// B34/S34
		ThirtyFourLife,

		// B1357/S1357
		Replicator,

		// B1357/S02468
		Fredkin,

		// B2/S
		Seeds,

		// B2/S0
		LiveFreeOrDie,

		// B25/S4
		B25S4,

		// B3/S12
		Flock,

		// B3/S012345678
		LifeWithoutDeath,

		// B35678/S5678
		Diamoeba,

		// B36/S125
		TwoByTwo,

		// B3678/S34678
		DayAndNight,

		// B37/S23
		DryLife,

		// B368/S245
		Move,

		// B4678/S35678
		Anneal,

		// B3/S1234
		Mazectric,

		// B3/S12345
		Maze,

		// B38/S23
		PedestrianLife,
	};

	// Create a rule-set from a preset.
	Ruleset(Preset preset = None);

	// Create a rule-set using a formatted B/S notation string. (ex. "B3/S23")
	// Note: B0 not supported.
	Ruleset(const std::string& bsRule);

	// Change rule-set using a formatted B/S notation string. (ex. "B3/S23")
	// Note: B0 not supported.
	// Returns true if string was parsed successfully.
	bool set(const std::string& bsRule);

	// Change rule-set to a preset.
	void set(Preset preset);

	// Manually change birth rules.
	void setBirth(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8);
	
	// Manually change survival rules.
	void setSurvival(bool s0, bool s1, bool s2, bool s3, bool s4, bool s5, bool s6, bool s7, bool s8);

	// Get this rule-set as a B/S notation c-string.
	inline const char* getString() const {
		this->invalidateString();
		return m_ruleString.c_str();
	}

	// Returns true if number of neighbours passes birth rules.
	inline bool testBirth(size_t neighbours) const {
		return (neighbours > MAX_NEIGHBOURS) ? false : m_birthSet[neighbours];
	}

	// Returns true if number of neighbours passes survival rules.
	inline bool testSurvival(size_t neighbours) const {
		return (neighbours > MAX_NEIGHBOURS) ? false : m_survivalSet[neighbours];
	}

private:
	// String is not updated until invalidateString() is called.
	// Use getString() instead of accessing directly.
	mutable std::string m_ruleString;
	mutable bool m_stringOutOfDate;
	void invalidateString() const;

	// returns -1 on error
	// returns 0 if successful births
	// returns 1 if successful survival
	static int parseHalfSet(const std::string& half, bool* outBirths, bool* outSurvival);

	bool m_birthSet[1 + MAX_NEIGHBOURS]; // B1-8 supported, B0 always is false in testBirth()
	bool m_survivalSet[1 + MAX_NEIGHBOURS]; // S0-8 supported

};

}