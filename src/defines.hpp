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

#ifndef DEFINE_HPP
#define DEFINE_HPP

#include <string>
#include <vector>
#include <map>
#include <set>

#include "utilities/coordinates.hpp"

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
// ROOM /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the flag of the room.
typedef enum class RoomFlags
{
    /// A player can rent and disconnect.
    Rent = 1,
    /// Everyone here can't be harmful.
    Peaceful = 2,
    /// From here a player can travel to another location.
    TravelPoint = 4,
    /// It is a spawn room for players.
    SpawnPoint = 8
} RoomFlag;

// //////////////////////////////////////////////////////////////////
// CHARACTER ////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Handle all the player's phases during login.
typedef enum class ConnectionStates
{
    /// The player has no state.
    NoState,
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
typedef enum class ActionTypes
{
    /// The character is fighting.
    NoAction,
    /// The character is doing common action.
    Wait,
    /// The character is moving to another location.
    Move,
    /// The character is crafting something.
    Crafting,
    /// The character is crafting something.
    Building,
    /// The character is fighting.
    Combat
} ActionType;

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

/// Used to determine the type of the model.
typedef enum class ModelTypes
{
    /// [0] No type.
    NoType,
    /// [1] A corpse, it's a knowledge for internal use.
    Corpse,
    /// [2] Any weapon.
    Weapon,
    /// [3] Any armor.
    Armor,
    /// [4] A shield.
    Shield,
    /// [5] Projectiles.
    Projectile,
    /// [6] A container for items.
    Container,
    /// [7] A container for liquids.
    LiqContainer,
    /// [8] Any tool.
    Tool,
    /// [9] Any node of resources.
    Node,
    /// [10] A resource.
    Resource,
    /// [11] Any kind of seed.
    Seed,
    /// [12] A key.
    Key,
    /// [13] A forniture (eg. chair, bed, painting, table and so on).
    Furniture,
    /// [14] A food.
    Food,
    /// [15] A source of light ((eg. torch, lamp, candle and so on).
    Light,
    /// [16] Any kind of vehicle.
    Vehicle,
    /// [17] A pen.
    Pen,
    /// [18] Container of parchements.
    Book,
    /// [19] A generic rope.
    Rope,
    /// [20] Anything which has no specific use.
    Trash,
    /// [21] Any kind of mechanism.
    Mechanism,
    /// [22] Any kind of currency.
    Currency
} ModelType;

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

/// Used to determine the flag of the model.
typedef enum class ModelFlags
{
    /// No flag.
    None = 0,
    /// The item can't be collected, put down or moved.
    Static = 1,
    /// The item can't be seen.
    Invisible = 2,
    /// The item can't be damaged.
    Unbreakable = 4,
    /// The item cannot be sold.
    NoSaleable = 8,
    /// Must be wielded with two hands.
    TwoHand = 16
} ModelFlag;

// //////////////////////////////////////////////////////////////////
// WEAPON ///////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Types of weapons.
typedef enum WeaponTypes
{
    /// [1]  Pistols that uses bullets
    kPistol = 1,
    /// [2]  Rifles that uses bullets
    kRifle,
    /// [3]  Shotguns
    kShotgun,
    /// [4]  Big guns, heavy weapons that uses bullets
    kHeavyWeapon,
    /// [5]  Pistols that uses energy
    kEnergyPistol,
    /// [6]  Rifles that uses energy.
    kEnergyRifle,
    /// [7]  Big guns, heavy weapons that uses energy.
    kEnergyHeavyWeapon,
    /// [8]  Granades
    kGranade,
    /// [9]  Placeble explosives
    kPlaced,
    /// [10] All sort of blades
    kBladed,
    /// [11] All sort of blunt weapons
    kBlunt,
    /// [12] Throwable weapons
    kThrown,
    /// [13] Weapon to enanche unarmed fight
    kUnarmed,
} WeaponType;

/// The values associated to model of type Weapon.
struct WeaponFunc
{
        /// The type of the weapon.
        WeaponType type;
        /// The minimum damage of the weapon.
        unsigned int minDamage;
        /// The maximum damage of the weapon.
        unsigned int maxDamage;
        /// The range of the weapon.
        unsigned int range;
};

// //////////////////////////////////////////////////////////////////
// ARMOR ////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Sizes of armor.
typedef enum ArmorSizes
{
    /// [0] All kind of size.
    kASAll,
    /// [1] Light Armor
    kASLight,
    /// [2] Medium Armor
    kASMedium,
    /// [3] Heavy Armor
    kASHeavy
} ArmorSize;

/// The values associated to model of type Armor.
struct ArmorFunc
{
        /// The size of the armor.
        ArmorSize size;
        /// The damage absorption of the armor.
        unsigned int damageAbs;
        /// The anatomy which a character must have in order to wear this armor.
        unsigned int allowedAnatomy;
};

// //////////////////////////////////////////////////////////////////
// SHIELD ///////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Shield sizes.
typedef enum ShieldSizes
{
    /// [1] Light Shield
    kSSLight = 1,
    /// [2] Medium Shield
    kSSMedium,
    /// [3] Heavy Shield
    kSSHeavy
} ShieldSize;

/// The values associated to model of type Shield.
struct ShieldFunc
{
        /// The size of the shield.
        ShieldSize size;
        /// The chace to parry with this weapon.
        unsigned int parryChance;
};

// //////////////////////////////////////////////////////////////////
// PROJECTILE ///////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// The values associated to model of type Projectile.
struct ProjectileFunc
{
        /// The increment to the damage dealt.
        unsigned int damageBonus;
        /// The increment to the range of the weapon.
        unsigned int rangeBonus;
};

// //////////////////////////////////////////////////////////////////
// CONTAINER ////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the flag of the container.
typedef enum class ContainerFlags
{
    /// No flag.
    None = 0,
    /// Can be closed.
    CanClose = 1,
    /// Can be burgled.
    CanBurgle = 2,
    /// Can see inside even if closed.
    CanSee = 4,
} ContainerFlag;

/// The values associated to model of type Container.
struct ContainerFunc
{
        /// The maximum weight which this container can carry.
        unsigned int maxWeight;
        /// The flags of the container.
        int flags;
        /// The vnum of the key which opens this container.
        unsigned int keyVnum;
        /// The lockpicking difficulty.
        unsigned int difficulty;
};

// //////////////////////////////////////////////////////////////////
// CONTAINER OF LIQUIDS /////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the flag of the container of liquid.
typedef enum class LiqContainerFlags
{
    /// No flag.
    None = 0,
    /// Liquid is poisoned.
    Poisoned = 1,
    /// Content never ends.
    Endless = 2,
    /// Container is destroyed once emptied.
    Destroy = 4,
} LiqContainerFlag;

/// The values associated to model of type Liquid Container.
struct LiqContainerFunc
{
        /// The maximum weight which this container can carry.
        unsigned int maxWeight;
        /// The flags of the container.
        int flags;
};

// //////////////////////////////////////////////////////////////////
// TOOL /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the type of the tool.
typedef enum class ToolTypes
{
    ///
    NoType = 0,
    ///
    Pickaxe = 1,
    ///
    WoodcutterAxe = 10,
    ///
    Saw = 11,
    ///
    PrecisionChisel = 12,
    ///
    Hammer = 20,
    ///
    PlaneChisel = 21,
    ///
    Forge = 30,
    ///
    Anvil = 31,
    ///
    BlacksmithHammer = 32,
    ///
    Bellows = 33,
    ///
    Crucible = 34,
    ///
    Tinderbox = 40
} ToolType;

/// The values associated to model of type Tool.
struct ToolFunc
{
        /// The type of the tool.
        ToolType type;
};

/// Set of tool types.
typedef std::set<ToolType> ToolSet;

// //////////////////////////////////////////////////////////////////
// NODE /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the type of the node.
typedef enum class NodeTypes
{
    NoType,
    /// [1] The node is a METAL node.
    Metal,
    /// [2] The node is a WOOD node.
    Wood,
    /// [3] The node is a STONE node.
    Stone
} NodeType;

/// The values associated to model of type Node.
struct NodeFunc
{
        /// Type of node.
        NodeType type;
        /// The vnum of the item provided during the extraction.
        int provides;
};

// //////////////////////////////////////////////////////////////////
// RESOURCE /////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the type of the resource.
typedef enum class ResourceTypes
{
    NoType,
    /// [1] COAL
    Coal,
    /// [2] ORE
    Ore,
    /// [3] BAR
    Bar,
    /// [4] LOG
    Log,
    /// [5] PLANK
    Plank,
    /// [6] TREE
    Tree,
    /// [7] FASTENER
    Fastener,
    /// [8] LEATHER
    Leather,
    /// [9] CLOTH
    Cloth,
    /// [10] STONE_BLOCK
    StoneBlock,
    /// [11] METAL_VEIN
    MetalVein,
    /// [12] STONE_MONOLITH
    StoneMonolith
} ResourceType;

/// The values associated to model of type Resource.
struct ResourceFunc
{
        /// Type of resource.
        ResourceType type;
};

/// Map which stores a type of resource and a quantity.
typedef std::map<ResourceType, unsigned int> IngredientMap;

// //////////////////////////////////////////////////////////////////
// SEED /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the type of the resource.
typedef enum SeedTypes
{
    /// [1] The seed is used to plant a tree.
    kSeedTree = 1,
    /// [2] The seed is used to plant a plant.
    kSeedPlant,
} SeedType;

/// The values associated to model of type Seed.
struct SeedFunc
{
        /// Type of seed.
        SeedType type;
};

// //////////////////////////////////////////////////////////////////
// FOOD /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the flag of the food.
typedef enum class FoodFlags
{
    /// No flag.
    None = 0,
    /// Food is poisoned.
    Poisoned = 1,
    /// Food is not cocked.
    Raw = 2,
} FoodFlag;

/// The values associated to model of type Food.
struct FoodFunc
{
        /// Hours of feeding.
        unsigned int hours;
        /// The food flags.
        int flags;
};

// //////////////////////////////////////////////////////////////////
// LIGHT ////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// The values associated to model of type Light.
struct LightFunc
{
        /// Maximum number of hours autonomy.
        unsigned int maxHours;
        /// Recharging polocy
        unsigned int policy;
};

// //////////////////////////////////////////////////////////////////
// BOOK /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// The values associated to model of type Book.
struct BookFunc
{
        /// Maximum number of parchments.
        unsigned int maxParchments;
};

// //////////////////////////////////////////////////////////////////
// ROPE /////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// The values associated to model of type Rope.
struct RopeFunc
{
        /// The difficulty to untie.
        unsigned int difficulty;
        /// Type of rope
        unsigned int type;
};

// //////////////////////////////////////////////////////////////////
// MECHANISM ////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

/// Used to determine the type of the resource.
typedef enum class MechanismTypes
{
    /// No flag
    None,
    /// [1] The mechanism is a door.
    Door,
    /// [2] The mechanism is a lock.
    Lock,
    /// [3] The mechanism is a picklock.
    Picklock,
    /// [4] The mechanism is a lever.
    Lever
} MechanismType;

/// The values associated to model of type Mechanism.
struct MechanismFunc
{
        /// Type of mechanism.
        MechanismType type;
        /// The vnum of the key which opens the door/lock.
        unsigned int key;
        /// Lockpicking difficulty.
        unsigned int difficulty;
        /// Lockpick efficency.
        unsigned int efficency;
        /// Lever command.
        unsigned int command;
        /// Vnum of the target of the lever.
        unsigned int target;
};

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
Coordinates GetCoordinates(Direction direction);

/// @brief Check if the passed flag is valid.
/// @param flag The flag to check.
/// @return <b>True</b> if the flag is valid,<br><b>False</b> otherwise.
bool ValidCharacterFlag(int flag);

/// @defgroup EnumToString Enumerator to Strings.
/// @brief All the functions necessary to transform into string an enum.
/// @{

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
/// Return the string describing the type of a Model.
std::string GetModelTypeName(ModelType type);
/// Return the string describing the equipment slot.
std::string GetEquipmentSlotName(EquipmentSlot slot);
/// Return the string describing the type of a Weapon.
std::string GetWeaponTypeName(WeaponType type);
/// Return the string describing the size of an Armor.
std::string GetArmorSizeName(ArmorSize size);
/// Return the string describing the size of a Shield.
std::string GetShieldSizeName(ShieldSize size);
/// Return the string describing the type of a Tool.
std::string GetToolTypeName(ToolType type);
/// Return the string describing the type of a Node.
std::string GetNodeTypeName(NodeType type);
/// Return the string describing the type of a Resource.
std::string GetResourceTypeName(ResourceType type);
/// Return the string describing the type of a Seed.
std::string GetSeedTypeName(SeedType type);
/// Return the string describing the type of a Mechanism.
std::string GetMechanismTypeName(MechanismType type);
/// Return the string describing the type of Action.
std::string GetActionTypeName(ActionType type);
///@}

/// @defgroup FlagsToList Flags to List of Strings.
/// @brief All the functions necessary to transform into a list of string a pool of flags.
/// @{

/// Return a list of string containg the Model flags contained inside the value.
std::string GetModelFlagString(int flags);
/// Return a list of string containg the Room flags contained inside the value.
std::string GetRoomFlagString(int flags);
/// Return a list of string containg the Character flags contained inside the value.
std::string GetCharacterFlagString(int flags);
/// Return a list of string containg the Exit flags contained inside the value.
std::string GetExitFlagString(int flags);
/// Return a list of string containg the Container flags contained inside the value.
std::string GetContainerFlagString(int flags);
/// Return a list of string containg the Container of Liquids flags contained inside the value.
std::string GetLiqContainerFlagString(int flags);
/// Return a list of string containg the Food flags contained inside the value.
std::string GetFoodFlagString(int flags);
///@}

#endif