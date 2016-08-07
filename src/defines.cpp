/// @file   defines.cpp
/// @brief  This file contains implementations of functions.
/// @author Enrico Fraccaroli
/// @date   5 Aug 2015
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

// Basic Include.
#include "defines.hpp"

#include "utils.hpp"

std::string GetAbilityName(Ability ability)
{
    if (ability == Ability::Strength) return "strength";
    if (ability == Ability::Agility) return "agility";
    if (ability == Ability::Perception) return "perception";
    if (ability == Ability::Constitution) return "constitution";
    if (ability == Ability::Intelligence) return "intelligence";
    return "none";
}

std::string GetPostureName(CharacterPosture posture)
{
    if (posture == CharacterPosture::Stand) return "standing";
    if (posture == CharacterPosture::Crouch) return "crouched";
    if (posture == CharacterPosture::Sit) return "sitting";
    if (posture == CharacterPosture::Prone) return "prone";
    if (posture == CharacterPosture::Rest) return "resting";
    return "none";
}

std::string GetCharacterFlagName(CharacterFlag flag)
{
    if (flag == CharacterFlag::IsGod) return "IsGod";
    if (flag == CharacterFlag::IsGod) return "Invisible";
    return "None";
}

std::string GetMaterialTypeName(MaterialType type)
{
    if (type == MaterialType::Metal) return "Metal";
    if (type == MaterialType::Stone) return "Stone";
    if (type == MaterialType::Wood) return "Wood";
    if (type == MaterialType::Skin) return "Skin";
    if (type == MaterialType::Cloth) return "Cloth";
    if (type == MaterialType::Vegetable) return "Vegetable";
    if (type == MaterialType::Meat) return "Meat";
    if (type == MaterialType::Glass) return "Glass";
    if (type == MaterialType::Paper) return "Paper";
    if (type == MaterialType::Coal) return "Coal";
    return "No Material Type";
}

std::string GetEquipmentSlotName(EquipmentSlot slot)
{
    if (slot == EquipmentSlot::Head) return "Head";
    if (slot == EquipmentSlot::Torso) return "Torso";
    if (slot == EquipmentSlot::Back) return "Back";
    if (slot == EquipmentSlot::Legs) return "Legs";
    if (slot == EquipmentSlot::Feet) return "Feet";
    if (slot == EquipmentSlot::RightHand) return "Right Hand";
    if (slot == EquipmentSlot::LeftHand) return "Left Hand";
    return "No Equipment Slot";
}
std::string GetGenderTypeName(GenderType type)
{
    if (type == GenderType::Male) return "Male";
    if (type == GenderType::Female) return "Female";
    return "none";
}

std::string GetTelnetCharName(TelnetChar c)
{
    if (c == TelnetChar::TerminalType) return "TerminalType";
    if (c == TelnetChar::NegotiateAboutWindowSize) return "NegotiateAboutWindowSize";
    if (c == TelnetChar::TerminalSpeed) return "TerminalSpeed";
    if (c == TelnetChar::RemoteFlowControl) return "RemoteFlowControl";
    if (c == TelnetChar::LineMode) return "LineMode";
    if (c == TelnetChar::EnvironmentVariables) return "EnvironmentVariables";
    if (c == TelnetChar::NewEnvironmentOption) return "NewEnvironmentOption";
    if (c == TelnetChar::TTYPE) return "TTYPE";
    if (c == TelnetChar::MSDP) return "MSDP";
    if (c == TelnetChar::MSDP_VAR) return "MSDP_VAR";
    if (c == TelnetChar::MSDP_VAL) return "MSDP_VAL";
    if (c == TelnetChar::MSDP_TABLE_OPEN) return "MSDP_TABLE_OPEN";
    if (c == TelnetChar::MSDP_TABLE_CLOSE) return "MSDP_TABLE_CLOSE";
    if (c == TelnetChar::MSDP_ARRAY_OPEN) return "MSDP_ARRAY_OPEN";
    if (c == TelnetChar::MSDP_ARRAY_CLOSE) return "MSDP_ARRAY_CLOSE";
    if (c == TelnetChar::MSSP) return "MSSP";
    if (c == TelnetChar::MCCP) return "MCCP";
    if (c == TelnetChar::SubNegotiationEnd) return "SubNegotiationEnd";
    if (c == TelnetChar::NoOperation) return "NoOperation";
    if (c == TelnetChar::DataMark) return "DataMark";
    if (c == TelnetChar::Break) return "Break";
    if (c == TelnetChar::InterruptProcess) return "InterruptProcess";
    if (c == TelnetChar::AbortOutput) return "AbortOutput";
    if (c == TelnetChar::AreYouThere) return "AreYouThere";
    if (c == TelnetChar::EraseCharacter) return "EraseCharacter";
    if (c == TelnetChar::EraseLine) return "EraseLine";
    if (c == TelnetChar::GoAhead) return "GoAhead";
    if (c == TelnetChar::SubnegotiationBegin) return "SubnegotiationBegin";
    if (c == TelnetChar::WILL) return "WILL";
    if (c == TelnetChar::WONT) return "WONT";
    if (c == TelnetChar::DO) return "DO";
    if (c == TelnetChar::DONT) return "DONT";
    if (c == TelnetChar::IAC) return "IAC";
    if (c == TelnetChar::DRAW_MAP) return "DRAW_MAP";
    if (c == TelnetChar::DRAW_MAP) return "DRAW_MAP";
    if (c == TelnetChar::FORMAT) return "FORMAT";
    return ToString(static_cast<int>(c));
}
