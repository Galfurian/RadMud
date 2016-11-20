/// @file   defines.hpp
/// @brief  This file contains all the definition needed by the mud.
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

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "coordinates.hpp"
#include "enum_checker.hpp"

/// List of telnet commands.
using TelnetChar = enum class TelnetChar_t
{
    None = 0,
    // OPTIONS
    // Echo = 1,
    // SuppressGoAhead = 3,
    // Status = 5,
    // TimingMark = 6,

    TerminalType = 24,
    NegotiateAboutWindowSize = 31,
    TerminalSpeed = 32,
    RemoteFlowControl = 33,
    LineMode = 34,
    EnvironmentVariables = 36,
    NewEnvironmentOption = 39,
    TTYPE = 24, ///< Mud Terminal Type Standard
    MSDP = 69, ///< Mud Server Data Protocol
    MSDP_VAR = 1,
    MSDP_VAL = 2,
    MSDP_TABLE_OPEN = 3,
    MSDP_TABLE_CLOSE = 4,
    MSDP_ARRAY_OPEN = 5,
    MSDP_ARRAY_CLOSE = 6,
    MSSP = 70, ///< Mud Server Status Protocol
    MCCP = 86, ///< Mud Client Compression Protocol
    SubNegotiationEnd = 240,
    NoOperation = 241,
    DataMark = 242,
    Break = 243,
    InterruptProcess = 244,
    AbortOutput = 245,
    AreYouThere = 246,
    EraseCharacter = 247,
    EraseLine = 248,
    GoAhead = 249,
    SubnegotiationBegin = 250,
    WILL = 251, ///< I will use option.
    WONT = 252, ///< I wont use option.
    DO = 253,   ///< Please, you use option
    DONT = 254, ///< Don't use option.
    IAC = 255,  ///< Interpret As Command

    // RADMUD SPECIFIC

    DRAW_MAP = 91,  ///< I will send the map.
    CLR_MAP = 92,   ///< Please, clear the already drawn map.
    FORMAT = 100    ///< I will send a format string.
};
/// Tester for telnet enums.
using TelnetCharTest = EnumCheck<TelnetChar,
//TelnetChar::Echo,
//TelnetChar::SuppressGoAhead,
//TelnetChar::Status,
//TelnetChar::TimingMark,
    TelnetChar::TerminalType,
    TelnetChar::NegotiateAboutWindowSize,
    TelnetChar::TerminalSpeed,
    TelnetChar::RemoteFlowControl,
    TelnetChar::LineMode,
    TelnetChar::EnvironmentVariables,
    TelnetChar::NewEnvironmentOption,
    TelnetChar::TTYPE,
    TelnetChar::MSDP,
    TelnetChar::MSDP_VAR,
    TelnetChar::MSDP_VAL,
    TelnetChar::MSDP_TABLE_OPEN,
    TelnetChar::MSDP_TABLE_CLOSE,
    TelnetChar::MSDP_ARRAY_OPEN,
    TelnetChar::MSDP_ARRAY_CLOSE,
    TelnetChar::MSSP,
    TelnetChar::MCCP,
    TelnetChar::SubNegotiationEnd,
    TelnetChar::NoOperation,
    TelnetChar::DataMark,
    TelnetChar::Break,
    TelnetChar::InterruptProcess,
    TelnetChar::AbortOutput,
    TelnetChar::AreYouThere,
    TelnetChar::EraseCharacter,
    TelnetChar::EraseLine,
    TelnetChar::GoAhead,
    TelnetChar::SubnegotiationBegin,
    TelnetChar::WILL,
    TelnetChar::WONT,
    TelnetChar::DO,
    TelnetChar::DONT,
    TelnetChar::IAC,
    TelnetChar::DRAW_MAP,
    TelnetChar::CLR_MAP,
    TelnetChar::FORMAT>;

/// Used to determine the type of Zone.
using AreaType = enum class AreaType_t
{
    NoType,     ///< No type.
    Cavern,     ///< Identifies a cavern.
    Underdark   ///< Identifies a region of the underdark.
};

/// Used to determine the status of a Zone.
using AreaStatus = enum class AreaStatus_t
{
    Normal,     ///< The area has no environmentals status.
    Iced,       ///< The area is freezing.
    Magmatic,   ///< The area have vulcanic activities.
    Vegetal     ///< The area is overrun by the vegetation.
};

/// Handle all the player's phases during login.
using ConnectionState = enum class ConnectionState_t
{
    LoggingIn, ///< The player is logging in.
    Playing    ///< The player is playing.
};

/// Handle all the player's phases during login.
using ConnectionFlag = enum class ConnectionFlag_t
{
    None,
    UseMSDP = 1,
    UseMCCP = 2
};

/// Used to determine the posture of the player.
using CharacterPosture = enum class CharacterPosture_t
{
    NoPosure,   ///< The character has no posture.
    Stand,      ///< The character it's standing.
    Crouch,     ///< The character it's crouched.
    Sit,        ///< The character it's sitting.
    Prone,      ///< The character it's prone.
    Rest,       ///< The character it's lying down.
};

/// Used to determine the flag of the character.
using CharacterFlag = enum class CharacterFlag_t
{
    None = 0,       ///< No flag.
    IsGod = 1,      ///< The character is a GOD.
    Invisible = 2   ///< The character is invisible.
};

/// The list of possible actions.
using GenderType = enum class GenderType_t
{
    None,   ///< The character has no gender (robot).
    Female, ///< The character is a female.
    Male    ///< The character is a male.
};

/// Used to determine the flag of the character.
using MobileFlag = enum class MobileFlag_t
{
    None = 0,       ///< No flag.
    NoViolent = 1   ///< The mobile is not violent.
};

/// Used to determine the flag of the exit.
using ExitFlag = enum class ExitFlag_t
{
    None = 0,   ///< No flag.
    NoMob = 1,  ///< A mob can't move through this exit.
    Hidden = 2, ///< The exit it's hidden.
    Stairs = 4, ///< The exit has stairs.
};

/// The list of materials.
using MaterialType = enum class MaterialType_t
{
    NoType,     ///< [0] No type.
    Metal,      ///< [1] Metal
    Stone,      ///< [2] Stone
    Wood,       ///< [3] Wood
    Skin,       ///< [4] Skin
    Cloth,      ///< [5] Cloth
    Vegetable,  ///< [6] Vegetable
    Meat,       ///< [7] Meat
    Glass,      ///< [8] Glass
    Paper,      ///< [9] Paper
    Coal        ///< [10] Coal
};

/// Used to determine the flag of the item.
using ItemFlag = enum class ItemFlag_t
{
    None = 0,
    Closed = 1,
    Locked = 2,
    Built = 4
};

// //////////////////////////////////////////////////////////////////
// MODEL ////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// @brief Check if the passed flag is valid.
/// @param flag The flag to check.
/// @return <b>True</b> if the flag is valid,<br><b>False</b> otherwise.
bool ValidCharacterFlag(int flag);

/// @defgroup EnumToString Enumerator to Strings.
/// @brief All the functions necessary to transform into string an enum.
/// @{

/// Return the string describing the given posture.
std::string GetPostureName(CharacterPosture posture);

/// Return the string describing the given character flag.
std::string GetCharacterFlagName(CharacterFlag flag);

/// Return the string describing the type of a Material.
std::string GetMaterialTypeName(MaterialType type);

/// Return the string describing the type of Gender.
std::string GetGenderTypeName(GenderType type);

/// Return the string describing the telnet character.
std::string GetTelnetCharName(TelnetChar c);
///@}
