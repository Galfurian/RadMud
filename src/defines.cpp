/// @file   defines.cpp
/// @brief  This file contains implementations of functions.
/// @author Enrico Fraccaroli
/// @date   5 Aug 2015
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

// Basic Include.
#include "defines.hpp"

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
