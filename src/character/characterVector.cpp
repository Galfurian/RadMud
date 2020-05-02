/// @file   characterVector.cpp
/// @author Enrico Fraccaroli
/// @date   Oct 11 2016
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

#include "character/characterVector.hpp"
#include "character/character.hpp"
#include "character/player.hpp"
#include "character/mobile.hpp"

static inline bool IsAnException(Character *character, std::vector<Character *> const &ex)
{
	return std::find_if(ex.begin(), ex.end(), [&character](Character *other) {
			   return other->getName() == character->getName();
		   }) != ex.end();
}

CharacterVector::CharacterVector()
{
	// Nothing to do.
}

Character *CharacterVector::findCharacter(const std::string &target, unsigned int number,
										  unsigned int *number_ptr,
										  const std::vector<Character *> &exceptions,
										  bool skipMobile, bool skipPlayer) const
{
	Character *found_character = nullptr;
	for (auto character : (*this)) {
		if ((skipMobile && character->isMobile()) || (skipPlayer && character->isPlayer())) {
			continue;
		}
		if (IsAnException(character, exceptions))
			continue;
		// Check if the character is a mobile or a player.
		if (character->isMobile()) {
			if (character->toMobile()->hasKey(ToLower(target))) {
				if (number == 1) {
					found_character = character;
					break;
				}
				--number;
			}
		} else {
			if (character->toPlayer()->isPlaying()) {
				if (BeginWith(character->toPlayer()->getName(), ToLower(target))) {
					if (number == 1) {
						found_character = character;
						break;
					}
					--number;
				}
			}
		}
	}
	if (number_ptr)
		*number_ptr = number;
	return found_character;
}

bool CharacterVector::containsCharacter(Character *character) const
{
	for (auto it : (*this)) {
		if (it->getName() == character->getName())
			return true;
	}
	return false;
}

void CharacterVector::emplace_back_character(Character *character)
{
	if (!this->containsCharacter(character))
		emplace_back(character);
}

void CharacterVector::addUnique(const CharacterVector &other)
{
	for (auto character : other)
		emplace_back_character(character);
}
