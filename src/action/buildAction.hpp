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
class BuildAction: public GeneralAction
{
    private:
        /// The schematics associated with the action.
        Building * schematics;
        /// The item, target of the action.
        Item * building;
        /// The tool used by the actor for the action.
        std::vector<Item *> tools;
        /// The ingredients used by the actor for the action.
        std::vector<Item *> ingredients;

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
            std::vector<Item *> & _ingredients,
            unsigned int _cooldown);

        /// @brief Destructor.
        virtual ~BuildAction();

        /// @brief Checks the correctness of the action's values.
        /// @return <b>True</b> if it has correct values,<br>
        ///         <b>False</b> otherwise.
        virtual bool check() const;

        /// @brief Provides the type of the action.
        /// @return The type of action.
        virtual ActionType getType() const;

        /// @brief Provides the description of the action.
        /// @return The string which describe the current action.
        virtual std::string getDescription() const;

        /// @brief Stops the current action and returns a string which describe the intterruption.
        /// @return The stopping description.
        virtual std::string stop();

        /// @brief Performs the current action.
        /// @return the status after performing the action.
        virtual ActionStatus perform();

    private:
        /// @brief Checks the building.
        /// @return <b>True</b> if the building is available,<br>
        ///         <b>False</b> otherwise.
        bool checkBuilding() const;

        /// @brief Checks the schematics.
        /// @return <b>True</b> if the schematics are available,<br>
        ///         <b>False</b> otherwise.
        bool checkSchematics() const;

        /// @brief Checks the ingredients.
        /// @return <b>True</b> if the ingredients are available,<br>
        ///         <b>False</b> otherwise.
        bool checkIngredients() const;

        /// @brief Checks the used tools.
        /// @return <b>True</b> if the tools are available,<br>
        ///         <b>False</b> otherwise.
        bool checkTools() const;
};
