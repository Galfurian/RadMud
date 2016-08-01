/// @file   defines.hpp
/// @brief  This file contains all the definition needed by the mud.
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

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "utilities/coordinates.hpp"
#include "utilities/enum_checker.hpp"

/// The list of character's abilities.
typedef enum class Abilities
{
    Strength,
    Agility,
    Perception,
    Constitution,
    Intelligence
} Ability;
/// Tester for ability enums.
using AbilityTest = EnumCheck<Ability,
Ability::Strength,
Ability::Agility,
Ability::Perception,
Ability::Constitution,
Ability::Intelligence>;

/// List of telnet commands.
typedef enum class TelnetChars
{
    None = 0,
    //OPTIONS
    //Echo = 1,
    //SuppressGoAhead = 3,
    //Status = 5,
    //TimingMark = 6,
    TerminalType = 24,
    NegotiateAboutWindowSize = 31,
    TerminalSpeed = 32,
    RemoteFlowControl = 33,
    LineMode = 34,
    EnvironmentVariables = 36,
    NewEnvironmentOption = 39,
    // MUD OPTIONS
    /// Mud Terminal Type Standard
    TTYPE = 24,
    /// Mud Server Data Protocol
    MSDP = 69,
    /// MSDP Commands
    MSDP_VAR = 1,
    MSDP_VAL = 2,
    MSDP_TABLE_OPEN = 3,
    MSDP_TABLE_CLOSE = 4,
    MSDP_ARRAY_OPEN = 5,
    MSDP_ARRAY_CLOSE = 6,
    /// Mud Server Status Protocol
    MSSP = 70,
    /// Mud Client Compression Protocol
    MCCP = 86,
    // NEGOTIATION 1
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
    // NEGOTIATION 2
    /// I will use option.
    WILL = 251,
    /// I wont use option.
    WONT = 252,
    /// Please, you use option
    DO = 253,
    /// Don't use option.
    DONT = 254,
    /// Interpret As Command
    IAC = 255,
    // RADMUD SPECIFIC
    /// I will send the map.
    DRAW_MAP = 91,
    /// Please, clear the already drawn map.
    CLR_MAP = 92,
    /// I will send a format string.
    FORMAT = 100
} TelnetChar;
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

/// The possible directions.
typedef enum class Directions
{
    /// No direction.
    None,
    /// North.
    North,
    /// South.
    South,
    /// West.
    West,
    /// East.
    East,
    /// Up.
    Up,
    /// Down.
    Down
} Direction;

/// Map which associate a string to a direction.
typedef std::map<std::string, Direction> DirectionMap;

// //////////////////////////////////////////////////////////////////
// AREA /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the type of Zone.
typedef enum class TypesOfAreas
{
    NoType,
    /// Identifies a cavern.
    Cavern,
    /// Identifies a region of the underdark.
    Underdark
} AreaType;

/// Used to determine the status of a Zone.
typedef enum class TypesOfAreaStatus
{
    NoStatus,
    /// The area has no environmentals status.
    Normal,
    /// The area is freezing.
    Iced,
    /// The area have vulcanic activities.
    Magmatic,
    /// The area is overrun by the vegetation.
    Vegetal
} AreaStatus;

// //////////////////////////////////////////////////////////////////
// ROOM /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

// //////////////////////////////////////////////////////////////////
// CHARACTER ////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Handle all the player's phases during login.
typedef enum class ConnectionStates
{
    /// The player has no state.
    NoState,
    /// The player is negotiating for MSDP.
    NegotiatingMSDP,
    /// The player is negotiating for MCCP.
    NegotiatingMCCP,
    /// We want their player name.
    AwaitingName,
    /// We want their password.
    AwaitingPassword,
    /// Step 1  - Choose the Name.
    AwaitingNewName,
    /// Step 2  - Choose the Password.
    AwaitingNewPwd,
    /// Step 3  - Confirm the Password.
    AwaitingNewPwdCon,
    /// Step 4  - Short story of the mud world.
    AwaitingNewStory,
    /// Step 5  - Choose the Race.
    AwaitingNewRace,
    /// Step 6  - Choose the Attributes.
    AwaitingNewAttr,
    /// Step 7  - Choose the Gender.
    AwaitingNewGender,
    /// Step 8  - Choose the Age.
    AwaitingNewAge,
    /// Step 9  - Choose the description (optional).
    AwaitingNewDesc,
    /// Step 10  - Choose the Weight.
    AwaitingNewWeight,
    /// Step 11 - Confirm the character.
    AwaitingNewConfirm,
    /// This is the normal 'connected' mode.
    Playing
} ConnectionState;

/// Handle all the player's phases during login.
typedef enum class ConnectionFlags
{
    None,
    UseMSDP = 1,
    UseMCCP = 2
} ConnectionFlag;

/// Used to determine the posture of the player.
typedef enum class CharacterPostures
{
    /// The character has no posture.
    NoPosure,
    /// The character it's standing.
    Stand,
    /// The character it's crouched.
    Crouch,
    /// The character it's sitting.
    Sit,
    /// The character it's prone.
    Prone,
    /// The character it's lying down.
    Rest,
} CharacterPosture;

/// Used to determine the flag of the character.
typedef enum class CharacterFlags
{
    None = 0,
    /// The character is a GOD.
    IsGod = 1,
    /// The character is invisible.
    Invisible = 2
} CharacterFlag;

/// The list of possible actions.
typedef enum class GenderTypes
{
    /// The character has no gender (robot).
    None,
    /// The character is a female.
    Female,
    /// The character is a male.
    Male
} GenderType;

/// Used to determine the flag of the character.
typedef enum class MobileFlags
{
    /// No flag.
    None = 0,
    /// The mobile is not violent.
    NoViolent = 1
} MobileFlag;

// //////////////////////////////////////////////////////////////////
// EXIT /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the flag of the exit.
typedef enum class ExitFlags
{
    /// No flag.
    None = 0,
    /// A mob can't move through this exit.
    NoMob = 1,
    /// The exit it's hidden.
    Hidden = 2,
    /// The exit has stairs.
    Stairs = 4,
} ExitFlag;

// //////////////////////////////////////////////////////////////////
// MATERIAL /////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// The list of materials.
typedef enum class MaterialTypes
{
    /// [0] No type.
    NoType,
    /// [1] Metal
    Metal,
    /// [2] Stone
    Stone,
    /// [3] Wood
    Wood,
    /// [4] Skin
    Skin,
    /// [5] Cloth
    Cloth,
    /// [6] Vegetable
    Vegetable,
    /// [7] Meat
    Meat,
    /// [8] Glass
    Glass,
    /// [9] Paper
    Paper,
    /// [10] Coal
    Coal
} MaterialType;

// //////////////////////////////////////////////////////////////////
// ITEM /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the flag of the item.
typedef enum class ItemFlags
{
    /// No flag
    None = 0,
    /// Open
    Closed = 1,
    /// Locked
    Locked = 2,
    /// Built
    Built = 4
} ItemFlag;

/// The quality of an item.
typedef enum class ItemQualities
{
    /// No quality set.
    None,
    /// Disastrous
    Disastrous,
    /// Poor
    Poor,
    /// Normal
    Normal,
    /// Fine
    Fine,
    /// Masterful
    Masterful
} ItemQuality;

// //////////////////////////////////////////////////////////////////
// MODEL ////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// All the possible equipment slots.
typedef enum class EquipmentSlots
{
    /// It can't be worn.
    None,
    /// It can be worn on the head.
    Head = 1,
    /// It can be worn on the chest.
    Torso = 2,
    /// It can be worn on the back (eg. backpack).
    Back = 4,
    /// It can be worn on the legs.
    Legs = 8,
    /// It can be worn on the feet.
    Feet = 16,
    /// Can be wielded on the right hand.
    RightHand = 32,
    /// Can be wielded on the lefty hand.
    LeftHand = 64,
} EquipmentSlot;

/// @brief Given the source direction, it returns the opposite direction.
/// @param direction The source direction.
/// @return The opposite direction.
Direction InverDirection(Direction direction);

/// @brief Given a string containing a direction, it returns the enum.
/// @param direction The string of the direction.
/// @return The enum of the direction.
Direction GetDirection(std::string direction);

/// @brief Given an enum of a possible direction, the function return the changing in terms of coordinates.
/// @param direction The enum of the direction.
/// @return The coordinates to that direction.
Coordinates<int> GetCoordinates(Direction direction);

/// @brief Check if the passed flag is valid.
/// @param flag The flag to check.
/// @return <b>True</b> if the flag is valid,<br><b>False</b> otherwise.
bool ValidCharacterFlag(int flag);

/// @defgroup EnumToString Enumerator to Strings.
/// @brief All the functions necessary to transform into string an enum.
/// @{

/// Return the string describing the given ability.
std::string GetAbilityName(Ability ability);
/// Return the string describing the given direction.
std::string GetDirectionName(Direction direction);
/// Return the string describing the given posture.
std::string GetPostureName(CharacterPosture posture);
/// Return the string describing the given character flag.
std::string GetCharacterFlagName(CharacterFlag flag);
/// Return the string describing the type of a Material.
std::string GetMaterialTypeName(MaterialType type);
/// Return the string describing the quality of an item.
std::string GetItemQualityName(ItemQuality quality);
/// Return the string describing the equipment slot.
std::string GetEquipmentSlotName(EquipmentSlot slot);
/// Return the string describing the type of Gender.
std::string GetGenderTypeName(GenderType type);
/// Return the string describing the telnet character.
std::string GetTelnetCharName(TelnetChar c);
///@}
