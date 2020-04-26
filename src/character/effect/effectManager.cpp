/// @file   effectManager.hpp
/// @brief  Implements a container for effects.
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

#include "character/effect/effectManager.hpp"
#include "character/effect/effect.hpp"

EffectManager::EffectManager() : activeEffects(), pendingEffects()
{
	// Nothing to do.
}

EffectManager::~EffectManager()
{
	// Nothing to do.
}

void EffectManager::addEffect(Effect const &effect, bool immediate)
{
	if (immediate) {
		// Check if the effect is already active.
		auto active = std::find_if(activeEffects.begin(), activeEffects.end(),
								   [effect](Effect const &e) {
									   return (effect.name == e.name);
								   });
		// If the same effect is already active, replace it.
		if (active != activeEffects.end()) {
			// Remove the modifiers due to the old effect.
			(*this) -= (*active);
			// Replace the active effect with the new one.
			(*active) = effect;
		} else {
			// Add the effect to the active effects.
			activeEffects.emplace_back(effect);
		}
		// Add the modifiers due to the effect.
		(*this) += effect;
		// Sort the list of active effects.
		this->sortEffects(activeEffects);
		return;
	}
	// First check if the same effect is already in the pending queue.
	for (auto &pending : pendingEffects) {
		// Check the equality between the effects.
		if (pending == effect) {
			// If the effect is already active, set the remaining time to the
			// longest one.
			if (pending.tremain < effect.tremain) {
				pending.tremain = effect.tremain;
			}
			return;
		}
	}
	// Add the effect to the pending effects.
	pendingEffects.emplace_back(effect);
}

void EffectManager::effectActivate()
{
	// First check if the same effect is already active.
	auto it = pendingEffects.begin();
	while (it != pendingEffects.end()) {
		// Check if the effect is still delayed.
		if (!it->updateDelayTime()) {
			// Move the iterator.
			++it;
			continue;
		}
		// Check if there is the same effect already active.
		if (std::find_if(activeEffects.begin(), activeEffects.end(),
						 [it](Effect const &e) {
							 return (it->name == e.name);
						 }) != activeEffects.end()) {
			// Remove the pending effect if already active.
			it = pendingEffects.erase(it);
			continue;
		}
		// Execute the activation functionality if present.
		if (it->functions.activate) {
			it->functions.activate(it->affected);
		}
		// Add the modifiers due to the effect.
		(*this) += (*it);
		// Add the effect to the active effects.
		activeEffects.emplace_back(std::move(*it));
		// Remove the activated effect.
		it = pendingEffects.erase(it);
	}
	// Sort the list of active effects.
	this->sortEffects(activeEffects);
}

void EffectManager::effectUpdate()
{
	// Iterate trough the active effects.
	auto it = activeEffects.begin();
	while (it != activeEffects.end()) {
		// If the effect is expired, remove it.
		if ((*it).updateRemainingTime()) {
			// If the effect has an expiration function, execute it.
			if (it->functions.expire) {
				it->functions.expire(it->affected);
			}
			// Remove the modifiers due to the effect.
			(*this) -= (*it);
			// Remove the effect from the list of active effects.
			it = activeEffects.erase(it);
		} else {
			// If the effect has a periodic function, execute it.
			if (it->functions.periodic) {
				it->functions.periodic(it->affected);
			}
			++it;
		}
	}
	// Sort the list of active effects.
	this->sortEffects(activeEffects);
}
