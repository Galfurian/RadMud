/// @file   combatHandler.cpp
/// @brief  Contains implementation of combat classes.
/// @author Enrico Fraccaroli
/// @date   May 8 2016
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

#include "combatHandler.hpp"

#include "room.hpp"
#include "area.hpp"

CombatHandler::CombatHandler(Character *_owner) :
	owner(_owner),
	opponents(),
	predefinedTarget(),
	aimedCharacter(),
	charactersInSight()
{
	// Nothing to do.
}

CombatHandler::~CombatHandler()
{
	// Nothing to do.
}

bool CombatHandler::addOpponent(Character *character, unsigned int initAggro)
{
	if (!this->hasOpponent(character)) {
		if (initAggro == 0) {
			initAggro = this->getInitialAggro(character);
		}
		// Add the aggressor.
		opponents.push_back(std::make_shared<Aggression>(character, initAggro));
		// Sort the list.
		this->sortList();
		MudLog(LogLevel::Debug, "%s engage %s with %d.",
					owner->getNameCapital(), character->getName(), initAggro);
		this->printList();
		return true;
	}
	return false;
}

bool CombatHandler::remOpponent(Character *character)
{
	for (auto it = opponents.begin(); it != opponents.end(); ++it) {
		auto aggression = (*it);
		if (aggression->aggressor->name == character->name) {
			// Remove the opponent.
			opponents.erase(it);
			// Check if the opponent is currently the aimed character.
			if (aimedCharacter != nullptr) {
				if (aimedCharacter->name == aggression->aggressor->name) {
					aimedCharacter = nullptr;
				}
			}
			// Check if the opponent is currently the predefined target.
			if (predefinedTarget != nullptr) {
				if (predefinedTarget->name == aggression->aggressor->name) {
					predefinedTarget = nullptr;
				}
			}
			MudLog(LogLevel::Debug, "%s disengages %s",
						owner->getNameCapital(), character->getName());
			this->printList();
			// If the list of opponents is empty, stop the fighting.
			if (this->empty()) {
				auto action = owner->getAction();
				if (action->getType() == ActionType::Combat) {
					auto combatAction = action->toCombatAction();
					combatAction->handleStop();
				}
			}
			return true;
		}
	}
	MudLog(LogLevel::Debug, "Cannot find %s among the aggressors of %s",
				character->getName(), owner->getNameCapital());
	this->printList();
	return false;
}

bool CombatHandler::hasOpponent(Character *character)
{
	return std::find_if(opponents.begin(), opponents.end(),
						[&](std::shared_ptr<Aggression> const &element) {
							return element->aggressor->name == character->name;
						}) != opponents.end();
}

void CombatHandler::updateCharactersInSight()
{
	// Get the characters in sight.
	CharacterVector exceptions;
	exceptions.emplace_back(owner);
	charactersInSight = owner->room->area->getCharactersInSight(
		exceptions, owner->room->coord, owner->getViewDistance());
}

void CombatHandler::setPredefinedTarget(Character *character)
{
	predefinedTarget = character;
}

Character *CombatHandler::getPredefinedTarget()
{
	return predefinedTarget;
}

void CombatHandler::setAimedTarget(Character *character)
{
	aimedCharacter = character;
}

Character *CombatHandler::getAimedTarget()
{
	return aimedCharacter;
}

bool CombatHandler::setAggro(Character *character, unsigned int newAggression)
{
	bool ret = false;
	auto it = std::find_if(opponents.begin(), opponents.end(),
						   [&](std::shared_ptr<Aggression> const &p) {
							   return p->aggressor->name == character->name;
						   });
	if (it != opponents.end()) {
		// Set the new aggro.
		(*it)->aggression = newAggression;
		// Sort the list.
		this->sortList();
		this->printList();
		// Set return value to success.
		ret = true;
	}
	return ret;
}

std::shared_ptr<CombatHandler::Aggression> CombatHandler::getTopAggro()
{
	if (opponents.empty()) {
		return nullptr;
	}
	return opponents.front();
}

bool CombatHandler::moveToTopAggro(Character *character)
{
	// Check if the character is a valid opponent.
	if (!this->hasOpponent(character)) {
		return false;
	}
	auto topAggressor = this->getTopAggro();
	if (topAggressor == nullptr) {
		return false;
	}
	// Check if the character is already the top aggro.
	if (topAggressor->aggressor == character) {
		return false;
	}
	// Retrieve the top aggro.
	auto currentAggro = this->getAggro(character);
	auto topAggro = currentAggro;
	for (auto it : opponents) {
		if (it->aggression > topAggro) {
			topAggro = it->aggression;
		}
	}
	// Just set the aggro of the character enough to be put on the first place.
	this->setAggro(character, topAggro + 1);
	return true;
}

unsigned int CombatHandler::getInitialAggro(Character *character)
{
	if (owner->level > character->level) {
		return (owner->level - character->level);
	}
	return (character->level - owner->level);
}

unsigned int CombatHandler::getAggro(Character *character)
{
	for (auto it : opponents) {
		if (it->aggressor == character) {
			return it->aggression;
		}
	}
	return 0;
}

std::size_t CombatHandler::getSize()
{
	return opponents.size();
}

bool CombatHandler::empty()
{
	return opponents.empty();
}

void CombatHandler::checkList()
{
	auto temporaryList = opponents;
	for (auto it = temporaryList.begin(); it != temporaryList.end(); ++it) {
		// Check if the aggressor is null.
		if ((*it)->aggressor == nullptr) {
			this->remOpponent((*it)->aggressor);
		}
		// Check if the aggressor is nowhere.
		else if ((*it)->aggressor->room == nullptr) {
			this->remOpponent((*it)->aggressor);
		}
	}
}

void CombatHandler::resetList()
{
	for (auto it : opponents) {
		if (it->aggressor == nullptr) {
			MudLog(LogLevel::Error, "Found a nullptr aggressor.");
		} else {
			// Remove the owner from its list.
			if (it->aggressor->combatHandler.remOpponent(owner)) {
				MudLog(LogLevel::Debug, "%s disengages %s",
							owner->getNameCapital(), it->aggressor->getName());
			} else {
				MudLog(LogLevel::Error,
							"Could not remove %s from opponents of %s.",
							owner->getName(), it->aggressor->getName());
			}
		}
	}
	opponents.clear();
	// Clear the predefined target.
	this->setPredefinedTarget(nullptr);
	// Clear the aimed character.
	this->setAimedTarget(nullptr);
}

std::vector<std::shared_ptr<CombatHandler::Aggression> >::iterator
CombatHandler::begin()
{
	return opponents.begin();
}

std::vector<std::shared_ptr<CombatHandler::Aggression> >::iterator
CombatHandler::end()
{
	return opponents.end();
}

void CombatHandler::sortList()
{
	std::sort(opponents.begin(), opponents.end(),
			  [](const std::shared_ptr<Aggression> &a,
				 const std::shared_ptr<Aggression> &b) {
				  return a->aggression > b->aggression;
			  });
}

void CombatHandler::printList()
{
	std::cout << "Aggro List:" << std::endl;
	for (auto it : opponents) {
		std::cout
			<< "  [" + (*it).aggressor->name + "] " + ToString((*it).aggression)
			<< std::endl;
	}
}
