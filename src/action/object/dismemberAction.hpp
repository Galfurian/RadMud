/// @file   dismemberAction.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "generalAction.hpp"
#include "corpseItem.hpp"

/// @brief An action which allows to dismember corpses.
class DismemberAction :
    public GeneralAction
{
private:
    /// The corpse.
    CorpseItem * corpse;
    /// The body part that the actor is dismembering.
    std::shared_ptr<BodyPart> bodyPart;

public:
    /// @brief Constructor.
    DismemberAction(Character * _actor,
                    CorpseItem * _corpse,
                    const std::shared_ptr<BodyPart> & _bodyPart);

    /// @brief Destructor.
    virtual ~DismemberAction();

    bool check(std::string & error) const override;

    ActionType getType() const override;

    std::string getDescription() const override;

    std::string stop() override;

    ActionStatus perform() override;

    /// @brief Given an action, it returns the necessary cooldown.
    /// @param character The actor.
    /// @return The non-decreasing value of the cooldown.
    static unsigned int getCooldown(Character * character,
                                    const std::shared_ptr<BodyPart> & _bodyPart);
};
