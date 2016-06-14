/// @file   actions.hpp
/// @brief  Defines all the actions that a player can do and which takes more than one turn to conclude.
/// @author Enrico Fraccaroli
/// @date   Feb 28 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include "production.hpp"
#include "building.hpp"
#include "defines.hpp"
#include "combat.hpp"
#include "utils.hpp"
#include "item.hpp"

/// Used to determine what are the character doing.
class Action
{
    private:
        /// The type of the action.
        ActionType type;
        /// Actor of the action.
        Character * actor;
        /// The character, target of the action.
        Character * target;
        /// The item, target of the action.
        Item * itemTarget;
        /// The destination of the actor.
        Room * destination;
        /// The direction of the actor.
        Direction direction;
        /// The production associated with the action.
        Production * production;
        /// The schematics associated with the action.
        Building * schematics;
        /// The material of which the production will be made of.
        Material * craftMaterial;
        /// The tool used by the actor for the action.
        std::vector<Item *> usedTools;
        /// The ingredients used by the actor for the action.
        std::vector<Item *> usedIngredients;
        /// The time point in the future needed by the action to complete.
        TimeClock actionCooldown;
        /// Next combat action.
        CombatAction nextCombatAction;

    public:
        /// @brief Constructor with init values.
        Action(Character * _character);

        /// @brief Destructor.
        ~Action();

        /// @brief Disable copy constructor.
        Action(const Action & source) = delete;

        /// @brief Disable assign operator.
        Action& operator=(const Action&) = delete;

        /// @brief Provides the type of the action.
        /// @return The type of action.
        ActionType getType();

        /// @brief Provides the description of the action.
        /// @return The string which describe the current action.
        std::string getDescription();

        /// @brief Stops the current action and returns a string which describe the intterruption.
        /// @return The stopping description.
        std::string stop();

        /// @brief Performs the current action.
        void perform();

        /// @brief Allows to set as next action a Movement action.
        /// @param _destination The destionation of the movement.
        /// @param _direction   The direction of the movement.
        /// @param _cooldown    How many seconds are required to complete the movement.
        /// @return <b>True</b> if it has correct values,<br> <b>False</b> otherwise.
        bool setMove(Room * _destination, Direction _direction, unsigned int _cooldown);

        /// @brief Allows to set a crafting action.
        /// @param _production      A pointer to the production to craft.
        /// @param _usedTools       The list of used tools.
        /// @param _usedIngredients The list of used ingredients.
        /// @param _craftMaterial   The material of the outcome.
        /// @param _cooldown        How many seconds are required to complete the action.
        /// @return <b>True</b> if it has correct values,<br> <b>False</b> otherwise.
        bool setCraft(
            Production * _production,
            ItemVector & _usedTools,
            ItemVector & _usedIngredients,
            Material * _craftMaterial,
            unsigned int _cooldown);

        /// @brief Allows to set a building action.
        /// @param _schematics      A pointer to the schematic of the building.
        /// @param _itemTarget      A pointer to the item which has to be built.
        /// @param _usedTools       The list of used tools.
        /// @param _usedIngredients The list of used ingredients.
        /// @param _cooldown        How many seconds are required to complete the action.
        /// @return <b>True</b> if it has correct values,<br> <b>False</b> otherwise.
        bool setBuild(
            Building * _schematics,
            Item * _itemTarget,
            ItemVector & _usedTools,
            ItemVector & _usedIngredients,
            unsigned int _cooldown);

        /// @brief Try to set the action to combat state.
        /// @return <b>True</b> if the type was not Combat,<br>
        ///         <b>False</b> otherwise.
        bool setInCombat();

        /// @brief Allows to set a combat action.
        /// @param nextAction The next action to execute in combat.
        /// @return <b>True</b> if correct values have been provided,<br>
        ///			<b>False</b> otherwise.
        bool setNextCombatAction(const CombatAction & nextAction);

        /// @brief Provides a copy of the next combat action.
        CombatAction getNextCombatAction() const;

        /// @brief This function resets the action.
        void reset();

    private:
        /// @brief Check if the cooldown of the action is elapsed.
        /// @return <b>True</b> if the time has passed,<br> <b>False</b> otherwise.
        bool checkElapsed();

        /// @brief Perform a Move action.
        void performMove();

        /// @brief Perform a Mine action.
        void performMine();

        /// @brief Perform a Chop action.
        void performChop();

        /// @brief Perform a Craft action.
        void performCraft();

        /// @brief Perform a Build action.
        void performBuild();

        /// @brief Perform a Comb action.
        void performComb();

        /// @brief Returns the attack damage.
        /// @return The value of the attack.
        void performCombatAction(const CombatAction & move);

};

#endif
