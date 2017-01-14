/// @file   effectList.hpp
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

/// @brief A class which allows to manage a buffer of effects.
class EffectManager
{
private:
    /// The overall ability modifier.
    std::map<AbilityModifier, int> activeAbilityModifier;
    /// The overall combat modifier.
    std::map<CombatModifier, int> activeCombatModifier;
    /// The overall status modifier.
    std::map<StatusModifier, int> activeStatusModifier;
    /// The overall knowledge.
    std::map<Knowledge, bool> activeKnowledge;
    /// The vector of active effects.
    std::vector<Effect> activeEffects;
    /// The vector of pending effects.
    std::vector<Effect> pendingEffects;
    /// The vector of passive effects.
    std::vector<Effect> passiveEffects;

public:
    /// @brief Constructor.
    EffectManager();

    /// @brief Destructor.
    ~EffectManager();

    // -------------------------------------------------------------------------
    // Management functions

    /// @brief Allows to add a passive effect.
    /// @param effect The effect that has to be added.
    void addPassiveEffect(const Effect & effect);

    /// @brief Allows to remove a passive effect.
    /// @param effect The effect that has to be added.
    void removePassiveEffect(const Effect & effect);

    /// @brief Allows to remove all the passive effects.
    void removeAllPassiveEffect();

    /// @brief Allows to add an effect.
    /// @param effect The effect that has to be added.
    void forceAddEffect(const Effect & effect);

    /// @brief Allows to add an effect to the buffer of pending effects.
    /// @param effect The one which has to be added to the buffer.
    void addPendingEffect(const Effect & effect);

    /// @brief One by one moves the pending effects inside the buffer of
    ///         Active effects.
    /// @param messages A vector of messages which is populated with the logs
    ///                  of the pending effects.
    /// @return <b>True</b> if there is a message to show,<br>
    ///         <b>False</b> otherwise.
    bool effectActivate(std::vector<std::string> & messages);

    /// @brief One by one updates the active effects.
    /// @param messages A vector of messages which is populated with the logs
    ///                  of the effects.
    /// @return <b>True</b> if there is a message to show,<br>
    ///         <b>False</b> otherwise.
    bool effectUpdate(std::vector<std::string> & messages);

    // -------------------------------------------------------------------------
    // Iterators

    /// @brief Provides an iterator to the begin of the list of active effects.
    std::vector<Effect>::iterator begin();

    /// @brief Provides an iterator to the end of the list of active effects.
    std::vector<Effect>::iterator end();

    /// @brief Provides a const_iterator to the begin of the list of
    ///         active effects.
    std::vector<Effect>::const_iterator begin() const;

    /// @brief Provides a const_iterator to the end of the list of
    ///         active effects.
    std::vector<Effect>::const_iterator end() const;

    /// @brief Retrieve the overall ability modifier.
    /// @param modifier The modifier to retrieve.
    /// @return The overall value of the given modifier.
    int getAbilityModifier(const AbilityModifier & modifier) const;

    /// @brief Retrieve the overall combat modifier.
    /// @param modifier The modifier to retrieve.
    /// @return The overall value of the given modifier.
    int getCombatModifier(const CombatModifier & modifier) const;

    /// @brief Retrieve the overall status modifier.
    /// @param modifier The modifier to retrieve.
    /// @return The overall value of the given modifier.
    int getStatusModifier(const StatusModifier & modifier) const;

    /// @brief Retrieve the knowledge.
    /// @param knowledge The knowledge to retrieve.
    /// @return The status of the given knowledge.
    bool getKnowledge(const Knowledge & knowledge) const;

private:

    /// @brief Activate an effect by adding all the modifiers brought by the
    /// effect.
    void activateEffect(const Effect & effect);

    /// @brief Deactivate an effect by removing all the modifiers brought by
    /// the effect.
    void deactivateEffect(const Effect & effect);

    /// @brief Sort the list of active effects.
    void sortList();
};
