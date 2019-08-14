/// @file   effect.hpp
/// @brief  Define the effect class.
/// @author Enrico Fraccaroli
/// @date   May 2 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include "ability.hpp"
#include "combatModifier.hpp"
#include "statusModifier.hpp"
#include "knowledge.hpp"
#include "utils.hpp"
#include "modifierManager.hpp"
#include "skill.hpp"

#include <functional>

class Character;

/// @brief Allows to define an effect which can alter the status of a character.
class Effect : public ModifierManager {
public:
	/// Functions associated with precise moments of the lifespan of an effect.
	typedef struct {
		/// Function executed when the effect expires.
		std::function<void(Character *character)> activate;
		/// Function executed at each tic.
		std::function<void(Character *character)> periodic;
		/// Function executed when the effect expires.
		std::function<void(Character *character)> expire;
	} Functionality;

	/// The character affected by the effect.
	Character *affected;
	/// Name of the modifier.
	std::string name;
	/// How many tic before activation.
	unsigned int tdelay;
	/// How many tic until it expires.
	unsigned int tremain;
	/// The functionality of the effet.
	Functionality functions;

	/// @brief Constructor.
	Effect(Character *_affected, std::string _name, unsigned int _tdelay,
		   unsigned int _tremain, Functionality _functions) :
		affected(_affected),
		name(std::move(_name)),
		tdelay(_tdelay),
		tremain(_tremain),
		functions(std::move(_functions))
	{
		Logger::log(LogLevel::Debug, "Created effect %s.", name);
	}

	/// @brief Destructor.
	~Effect()
	{
		Logger::log(LogLevel::Debug, "Deleted effect %s.", name);
	}

	/// @brief Update the remaining tics.
	/// @return <b>True</b> if the effect is expired,<br>
	///         <b>False</b> otherwise.
	inline bool updateRemainingTime()
	{
		if (tremain == 0)
			return true;
		--tremain;
		return false;
	}

	/// @brief Update the delay.
	/// @return <b>True</b> if the delay has expired,<br>
	///         <b>False</b> otherwise.
	inline bool updateDelayTime()
	{
		if (tdelay == 0)
			return true;
		--tdelay;
		return false;
	}

	/// @brief Operator used to order the effect based on the remaining time.
	inline bool operator<(const Effect &right) const
	{
		return tremain < right.tremain;
	}

	/// @brief Equality operator between the names of two effects.
	inline bool operator==(const Effect &right) const
	{
		return name == right.name;
	}
};
