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

#ifndef EFFECT_HPP
#define EFFECT_HPP

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
        /// Health modification.
        int health_mod;
        /// Stamina modification.
        int stamina_mod;
        /// Strength modification.
        int str_mod;
        /// Agility modification.
        int agi_mod;
        /// Perception modification.
        int per_mod;
        /// Constitution modification.
        int con_mod;
        /// Intelligence modification.
        int int_mod;
        /// Hit chance modification.
        int hit_mod;
        /// Damage modification.
        int dam_mod;

        /// @brief Constructor.
        Effect(std::string _name, int _expires, std::string _messageActivate, std::string _messageFade);

        /// @brief Destructor.
        ~Effect();

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
        /// A list of active effects.
        std::vector<Effect> activeEffects;
        /// A list of pending effects.
        std::vector<Effect> pendingEffects;

        /// @brief Constructor.
        EffectList();

        /// @brief Destructor.
        ~EffectList();

        /// @brief Provides the overall health modifier.
        /// @return The total modifier.
        int getHealthMod();

        /// @brief Provides the overall stamina modifier.
        /// @return The total modifier.
        int getStaminaMod();

        /// @brief Provides the overall stength modifier.
        /// @return The total modifier.
        int getStrMod();

        /// @brief Provides the overall agility modifier.
        /// @return The total modifier.
        int getAgiMod();

        /// @brief Provides the overall perception modifier.
        /// @return The total modifier.
        int getPerMod();

        /// @brief Provides the overall constitution modifier.
        /// @return The total modifier.
        int getConMod();

        /// @brief Provides the overall intelligence modifier.
        /// @return The total modifier.
        int getIntMod();

        /// @brief Provides the overall hit modifier.
        /// @return The total modifier.
        int getHitMod();

        /// @brief Provides the overall damage modifier.
        /// @return The total modifier.
        int getDamMod();

        /// @brief Provides the overall ability.
        /// @param ability The ability.
        /// @return The total modifier.
        int getAbilityModifier(const Ability & ability);

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
};

#endif
