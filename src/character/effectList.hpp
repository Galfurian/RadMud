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
class EffectList
{
public:
    /// Type of structure which contains effects.
    using EffectVector = typename std::vector<Effect>;
    /// Iterator for an effects vector.
    using iterator = typename std::vector<Effect>::iterator;
    /// Constant iterator for an effects vector.
    using const_iterator = typename std::vector<Effect>::const_iterator;

private:
    /// The list of active effects.
    EffectVector activeEffects;
    /// The list of pending effects.
    EffectVector pendingEffects;

public:
    /// @brief Constructor.
    EffectList();

    /// @brief Destructor.
    ~EffectList();

    /// @brief Provides the overall health modifier.
    /// @return The total modifier.
    int getHealthMod() const;

    /// @brief Provides the overall stamina modifier.
    /// @return The total modifier.
    int getStaminaMod() const;

    /// @brief Provides the overall melee hit modifier.
    /// @return The total modifier.
    int getMeleeHitMod() const;

    /// @brief Provides the overall melee damage modifier.
    /// @return The total modifier.
    int getMeleeDamMod() const;

    /// @brief Provides the overall ranged hit modifier.
    /// @return The total modifier.
    int getRangedHitMod() const;

    /// @brief Provides the overall ranged damage modifier.
    /// @return The total modifier.
    int getRangedDamMod() const;

    /// @brief Provides the overall ability.
    /// @param ability The ability.
    /// @return The total modifier.
    int getAbilityModifier(const Ability & ability) const;

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

    /// @brief Sort the list of effects.
    void sortList();

    /// @brief Provides an iterator to the begin of the list of active effects.
    iterator begin();

    /// @brief Provides a const_iterator to the begin of the list of
    ///         active effects.
    const_iterator begin() const;

    /// @brief Provides an iterator to the end of the list of active effects.
    iterator end();

    /// @brief Provides a const_iterator to the end of the list of
    ///         active effects.
    const_iterator end() const;
};
