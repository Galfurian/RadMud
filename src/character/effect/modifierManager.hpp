/// @file   modifierManager.hpp
/// @author Enrico Fraccaroli
/// @date   ott 14 2017
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

#include "ability.hpp"
#include "combatModifier.hpp"
#include "statusModifier.hpp"
#include "knowledge.hpp"
#include "utils.hpp"

#include <memory>

/// @brief Addition-Assignment operator for two Ability Modifier maps.
template<typename ModifierType>
inline std::map<ModifierType, int> & operator+=(
    std::map<ModifierType, int> & left,
    const std::map<ModifierType, int> & right)
{
    for (const auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second += rightModifier.second;
        }
        else
        {
            left.insert(rightModifier);
        }
    }
    return left;
}

inline std::map<Knowledge, int> & operator+=(
    std::map<Knowledge, int> & left,
    const std::map<Knowledge, int> & right)
{
    for (const auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second = true;
        }
        else
        {
            left.insert(rightModifier);
        }
    }
    return left;
}

/// @brief Subtraction-Assignment operator for two Ability Modifier maps.
template<typename ModifierType>
inline std::map<ModifierType, int> & operator-=(
    std::map<ModifierType, int> & left,
    const std::map<ModifierType, int> & right)
{
    for (const auto & rightModifier : right)
    {
        auto leftModifier = left.find(rightModifier.first);
        if (leftModifier != left.end())
        {
            leftModifier->second -= rightModifier.second;
            if (leftModifier->second < 0)
            {
                leftModifier->second = 0;
            }
        }
    }
    return left;
}

class ModifierManager
{
protected:
    /// The overall ability modifier.
    std::map<Ability, int> modAbility;
    /// The overall combat modifier.
    std::map<CombatModifier, int> modCombat;
    /// The overall status modifier.
    std::map<StatusModifier, int> modStatus;
    /// The overall knowledge.
    std::map<Knowledge, int> modKnowledge;

private:

    /// @brief
    /// @param modifier The modifier to retrieve.
    /// @return The total value of the given modifier.
    template<typename ModifierType>
    inline void applyModifier(std::map<ModifierType, int> & receiver,
                              std::map<ModifierType, int> & provider,
                              const int & multiplier)
    {
        typename std::map<ModifierType, int>::iterator receiverIt;
        // Iterate through the modifiers of the skill.
        for (std::pair<const ModifierType, int> & providerIt : provider)
        {
            // Evaluate the modifier.
            auto modifier = providerIt.second * multiplier;
            // Otherwise add the modifier based on the skill rank.
            receiverIt = receiver.find(providerIt.first);
            if (receiverIt == receiver.end())
            {
                receiver.insert(std::make_pair(providerIt.first, modifier));
            }
            else
            {
                receiverIt->second += modifier;
            }
        }
    }

public:
    /// @brief Constructor.
    ModifierManager() :
        modAbility(),
        modCombat(),
        modStatus(),
        modKnowledge()
    {
        // Nothing to do.
    }

    inline void setAbilityMod(const Ability & key, const int & mod)
    {
        modAbility[key] = mod;
    }

    inline void setCombatMod(const CombatModifier & key, const int & mod)
    {
        modCombat[key] = mod;
    }

    inline void setStatusMod(const StatusModifier & key, const int & mod)
    {
        modStatus[key] = mod;
    }

    inline void setKnowledge(const Knowledge & key, const int & mod)
    {
        modKnowledge[key] = mod;
    }

    /// @brief Retrieve the total ability modifier.
    /// @param modifier The modifier to retrieve.
    /// @return The total value of the given modifier.
    inline int getAbilityMod(const Ability & key) const
    {
        auto it = modAbility.find(key);
        return (it != modAbility.end()) ? it->second : 0;
    }

    /// @brief Retrieve the total combat modifier.
    /// @param modifier The modifier to retrieve.
    /// @return The total value of the given modifier.
    inline int getCombatMod(const CombatModifier & key) const
    {
        auto it = modCombat.find(key);
        return (it != modCombat.end()) ? it->second : 0;
    }

    /// @brief Retrieve the total status modifier.
    /// @param modifier The modifier to retrieve.
    /// @return The total value of the given modifier.
    inline int getStatusMod(const StatusModifier & key) const
    {
        auto it = modStatus.find(key);
        return (it != modStatus.end()) ? it->second : 0;
    }

    /// @brief Retrieve the total knowledge.
    /// @param knowledge The knowledge to retrieve.
    /// @return The total value of the given knowledge.
    inline int getKnowledge(const Knowledge & key) const
    {
        auto it = modKnowledge.find(key);
        return (it != modKnowledge.end()) ? it->second : 0;
    }

    /// @brief Provides the list of active ability modifiers.
    inline std::map<Ability, int> getAbilityMod() const
    {
        return modAbility;
    }

    /// @brief Provides the list of active combat modifiers.
    inline std::map<CombatModifier, int> getCombatMod() const
    {
        return modCombat;
    }

    /// @brief Provides the list of active status modifiers.
    inline std::map<StatusModifier, int> getStatusMod() const
    {
        return modStatus;
    }

    /// @brief Provides the list of active knowledge.
    inline std::map<Knowledge, int> getKnowledge() const
    {
        return modKnowledge;
    }

    inline void reset()
    {
        modAbility.clear();
        modCombat.clear();
        modStatus.clear();
        modKnowledge.clear();
    }

    /// @brief Adds the modifiers of another manager.
    inline ModifierManager & operator+=(const ModifierManager & other)
    {
        modAbility += other.modAbility;
        modCombat += other.modCombat;
        modStatus += other.modStatus;
        modKnowledge += other.modKnowledge;
        return (*this);
    }

    /// @brief Removes the modifiers of another manager.
    inline ModifierManager & operator-=(const ModifierManager & other)
    {
        modAbility -= other.modAbility;
        modCombat -= other.modCombat;
        modStatus -= other.modStatus;
        modKnowledge -= other.modKnowledge;
        return (*this);
    }

    inline void applyModifier(const std::shared_ptr<ModifierManager> & provider,
                              const int & multiplier)
    {
        this->applyModifier(modAbility, provider->modAbility, multiplier);
        this->applyModifier(modCombat, provider->modCombat, multiplier);
        this->applyModifier(modStatus, provider->modStatus, multiplier);
        this->applyModifier(modKnowledge, provider->modKnowledge, multiplier);
    }

    friend inline std::shared_ptr<ModifierManager> & operator-=(
        std::shared_ptr<ModifierManager> & left,
        const std::shared_ptr<ModifierManager> & right);

    friend inline std::shared_ptr<ModifierManager> & operator+=(
        std::shared_ptr<ModifierManager> & left,
        const std::shared_ptr<ModifierManager> & right);
};

/// @brief Adds the modifiers of another manager.
inline std::shared_ptr<ModifierManager> & operator+=(
    std::shared_ptr<ModifierManager> & left,
    const std::shared_ptr<ModifierManager> & right)
{
    left->modAbility += right->modAbility;
    left->modCombat += right->modCombat;
    left->modStatus += right->modStatus;
    left->modKnowledge += right->modKnowledge;
    return left;
}

/// @brief Removes the modifiers of another manager.
inline std::shared_ptr<ModifierManager> & operator-=(
    std::shared_ptr<ModifierManager> & left,
    const std::shared_ptr<ModifierManager> & right)
{
    left->modAbility -= right->modAbility;
    left->modCombat -= right->modCombat;
    left->modStatus -= right->modStatus;
    left->modKnowledge -= right->modKnowledge;
    return left;
}