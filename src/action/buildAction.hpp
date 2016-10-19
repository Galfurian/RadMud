/// @file   buildAction.hpp
/// @brief  Definition of the class for a build action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include "generalAction.hpp"

/// @brief An action executed by characters during building.
class BuildAction :
    public GeneralAction
{
private:
    /// The schematics associated with the action.
    Building * schematics;
    /// The item, target of the action.
    Item * building;
    /// The tool used by the actor for the action.
    std::vector<Item *> tools;
    /// The ingredients used by the actor for the action.
    std::vector<std::pair<Item *, unsigned int>> ingredients;

public:
    /// @brief Constructor.
    /// @param _actor       The actor who is doing the action.
    /// @param _schematics  A pointer to the schematic of the building.
    /// @param _building    A pointer to the item which has to be built.
    /// @param _tools       The list of used tools.
    /// @param _ingredients The list of used ingredients.
    /// @param _cooldown    How many seconds are required to complete the action.
    BuildAction(
        Character * _actor,
        Building * _schematics,
        Item * _building,
        std::vector<Item *> & _tools,
        std::vector<std::pair<Item *, unsigned int>> & _ingredients,
        unsigned int _cooldown);

    /// @brief Destructor.
    virtual ~BuildAction();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    static unsigned int getConsumedStamina(Character * character);
};
