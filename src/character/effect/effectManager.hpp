/// @file   effectManager.hpp
/// @brief  Define a container for effects.
/// @author Enrico Fraccaroli
/// @date   Oct 22 2016
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

#include "effect.hpp"
#include "modifierManager.hpp"

#include <memory>

/// @brief A class which allows to manage effects.
class EffectManager : public ModifierManager {
private:
	/// The list of active effects.
	std::vector<Effect> activeEffects;
	/// The list of pending effects.
	std::vector<Effect> pendingEffects;

public:
	/// @brief Constructor.
	EffectManager();

	/// @brief Destructor.
	~EffectManager();

	/// @brief Allows to add an effect to the buffer of pending effects.
	/// @param effect    The one which has to be added to the buffer.
	/// @param immediate If specified the effect is immediately activated.
	void addEffect(Effect const &effect, bool immediate = false);

	/// @brief Activate pending effects.
	void effectActivate();

	/// @brief Updates the active effects.
	void effectUpdate();

	/// @brief Provides the list of active effects.
	inline std::vector<Effect> const &getActiveEffects() const
	{
		return activeEffects;
	}

	/// @brief Provides the list of pending effects.
	inline std::vector<Effect> const &getPendingEffects() const
	{
		return pendingEffects;
	}

private:
	/// @brief Sort the list of active effects.
	inline void sortEffects(std::vector<Effect> &effects)
	{
		std::sort(effects.begin(), effects.end(), std::less<Effect>());
	}
};
