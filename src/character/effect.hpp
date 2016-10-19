/// @file   effect.hpp
/// @brief  Define the effect class.
/// @author Enrico Fraccaroli
/// @date   May 2 2016
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

#include "defines.hpp"
#include "utils.hpp"

/// @brief Allows to define an effect which can alter the status of a character.
class Effect
{
public:
    /// Name of the modifier.
    std::string name;
    /// How many tic until it expires.
    int expires;
    /// Message to show when the effect begins.
    std::string messageActivate;
    /// Message to show when the effect ends.
    std::string messageExpire;
    /// Health modifier.
    int health;
    /// Stamina modifier.
    int stamina;
    /// Abilities modifier.
    std::map<Ability, int> abilities;
    /// Hit chance modifier.
    int hit;
    /// Damage modifier.
    int damage;

    /// @brief Constructor.
    Effect(
        std::string _name,
        int _expires,
        std::string _messageActivate,
        std::string _messageFade);

    /// @brief Update the cooldown of the effect.
    /// @return <b>True</b> if the effect is expired,<br>
    ///         <b>False</b> otherwise.
    bool update();

    /// @brief Operator used to order the effect based on the remaining time.
    bool operator<(const Effect & right) const;
};

/// @brief A class which allows to manage a buffer of Active and Pending effects.
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

    /// @brief Provides the overall hit modifier.
    /// @return The total modifier.
    int getHitMod() const;

    /// @brief Provides the overall damage modifier.
    /// @return The total modifier.
    int getDamMod() const;

    /// @brief Provides the overall ability.
    /// @param ability The ability.
    /// @return The total modifier.
    int getAbilityModifier(const Ability & ability) const;

    /// @brief Allows to add an effect to the buffer of pending effects.
    /// @param effect The one which has to be added to the buffer.
    void addPendingEffect(const Effect & effect);

    /// @brief One by one moves the pending effects inside the buffer of Active effects.
    /// @param messages A vector of messages which is populated with the logs of the pending effects.
    /// @return <b>True</b> if there is a message to show,<br><b>False</b> otherwise.
    bool effectActivate(std::vector<std::string> & messages);

    /// @brief One by one updates the active effects.
    /// @param messages A vector of messages which is populated with the logs of the effects.
    /// @return <b>True</b> if there is a message to show,<br><b>False</b> otherwise.
    bool effectUpdate(std::vector<std::string> & messages);

    /// @brief Sort the list of effects.
    void sortList();

    /// @brief Provides an iterator to the begin of the list of active effects.
    iterator begin();

    /// @brief Provides a const_iterator to the begin of the list of active effects.
    const_iterator begin() const;

    /// @brief Provides an iterator to the end of the list of active effects.
    iterator end();

    /// @brief Provides a const_iterator to the end of the list of active effects.
    const_iterator end() const;
};
