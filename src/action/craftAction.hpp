/// @file   craftAction.hpp
/// @brief  Definition of the class for a craft action.
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
#include "../profession.hpp"
#include "../production.hpp"

/// @brief An action executed by characters when crafting new objects.
class CraftAction :
    public GeneralAction
{
private:
    /// The production associated with the action.
    Production * production;
    /// The material of which the production will be made of.
    Material * material;
    /// The tool used by the actor for the action.
    std::vector<Item *> tools;
    /// The ingredients used by the actor for the action.
    std::vector<std::pair<Item *, unsigned int>> ingredients;

public:
    /// @brief Constructor.
    /// @param _actor       The actor who is doing the action.
    /// @param _production  A pointer to the production to craft.
    /// @param _material    The material of the outcome.
    /// @param _tools       The list of used tools.
    /// @param _ingredients The list of used ingredients.
    /// @param _cooldown    How many seconds are required to complete the action.
    CraftAction(
        Character * _actor,
        Production * _production,
        Material * _material,
        std::vector<Item *> & _tools,
        std::vector<std::pair<Item *, unsigned int>> & _ingredients,
        unsigned int & _cooldown);

    /// @brief Destructor.
    virtual ~CraftAction();

    bool check() const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

private:
    /// @brief Checks the production.
    /// @return <b>True</b> if the production is available,<br>
    ///         <b>False</b> otherwise.
    bool checkProduction() const;

    /// @brief Checks the material.
    /// @return <b>True</b> if the material is available,<br>
    ///         <b>False</b> otherwise.
    bool checkMaterial() const;

    /// @brief Checks the ingredients.
    /// @return <b>True</b> if the ingredients are available,<br>
    ///         <b>False</b> otherwise.
    bool checkIngredients() const;

    /// @brief Checks the used tools.
    /// @return <b>True</b> if the tools are available,<br>
    ///         <b>False</b> otherwise.
    bool checkTools() const;
};
