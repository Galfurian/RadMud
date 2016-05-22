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

Direction InverDirection(Direction direction)
{
    if (direction == Direction::North) return Direction::South;
    if (direction == Direction::South) return Direction::North;
    if (direction == Direction::West) return Direction::East;
    if (direction == Direction::East) return Direction::West;
    if (direction == Direction::Up) return Direction::Down;
    if (direction == Direction::Down) return Direction::Up;
    return Direction::None;
}

Direction GetDirection(std::string direction)
{
    if (direction == "north") return Direction::North;
    if (direction == "south") return Direction::South;
    if (direction == "west") return Direction::West;
    if (direction == "east") return Direction::East;
    if (direction == "up") return Direction::Up;
    if (direction == "down") return Direction::Down;
    return Direction::None;
}
Coordinates GetCoordinates(Direction direction)
{
    if (direction == Direction::North) return Coordinates(0, +1, 0);
    if (direction == Direction::South) return Coordinates(0, -1, 0);
    if (direction == Direction::West) return Coordinates(-1, 0, 0);
    if (direction == Direction::East) return Coordinates(+1, 0, 0);
    if (direction == Direction::Up) return Coordinates(0, 0, +1);
    if (direction == Direction::Down) return Coordinates(0, 0, -1);
    return Coordinates(0, 0, 0);
}

std::string GetDirectionName(Direction direction)
{
    std::string output;
    switch (direction)
    {
        case Direction::North:
            output = "north";
            break;
        case Direction::South:
            output = "south";
            break;
        case Direction::West:
            output = "west";
            break;
        case Direction::East:
            output = "east";
            break;
        case Direction::Up:
            output = "up";
            break;
        case Direction::Down:
            output = "down";
            break;
        case Direction::None:
        default:
            output = "No Direction";
    }
    return output;
}

std::string GetPostureName(CharacterPosture posture)
{
    std::string output;
    switch (posture)
    {
        case CharacterPosture::Stand:
            output = "standing";
            break;
        case CharacterPosture::Crouch:
            output = "crouched";
            break;
        case CharacterPosture::Sit:
            output = "sitting";
            break;
        case CharacterPosture::Prone:
            output = "prone";
            break;
        case CharacterPosture::Rest:
            output = "resting";
            break;
        case CharacterPosture::NoPosure:
            output = "noPosture";
            break;
    }
    return output;
}

std::string GetCharacterFlagName(CharacterFlag flag)
{
    switch (flag)
    {
        case CharacterFlag::None:
            return "None";
        case CharacterFlag::IsGod:
            return "IsGod";
        case CharacterFlag::Invisible:
            return "Invisible";
        default:
            return "None";
    }
}

std::string GetMaterialTypeName(MaterialType type)
{
    std::string output;
    switch (type)
    {
        case MaterialType::NoType:
            output = "No Material Type";
            break;
        case MaterialType::Metal:
            output = "Metal";
            break;
        case MaterialType::Stone:
            output = "Stone";
            break;
        case MaterialType::Wood:
            output = "Wood";
            break;
        case MaterialType::Skin:
            output = "Skin";
            break;
        case MaterialType::Cloth:
            output = "Cloth";
            break;
        case MaterialType::Vegetable:
            output = "Vegetable";
            break;
        case MaterialType::Meat:
            output = "Meat";
            break;
        case MaterialType::Glass:
            output = "Glass";
            break;
        case MaterialType::Paper:
            output = "Paper";
            break;
        case MaterialType::Coal:
            output = "Coal";
            break;
    }
    return output;
}

std::string GetItemQualityName(ItemQuality quality)
{
    std::string output;
    switch (quality)
    {
        case ItemQuality::None:
            output = "No Quality";
            break;
        case ItemQuality::Disastrous:
            output = "Disastrous";
            break;
        case ItemQuality::Poor:
            output = "Poor";
            break;
        case ItemQuality::Normal:
            output = "Normal";
            break;
        case ItemQuality::Fine:
            output = "Fine";
            break;
        case ItemQuality::Masterful:
            output = "Masterful";
            break;
    }
    return output;
}

std::string GetModelTypeName(ModelType type)
{
    std::string output;
    switch (type)
    {
        case ModelType::NoType:
            output = "No Model Type";
            break;
        case ModelType::Corpse:
            output = "Corpse";
            break;
        case ModelType::Weapon:
            output = "Weapon";
            break;
        case ModelType::Armor:
            output = "Armor";
            break;
        case ModelType::Shield:
            output = "Shield";
            break;
        case ModelType::Projectile:
            output = "Projectile";
            break;
        case ModelType::Container:
            output = "Container";
            break;
        case ModelType::LiqContainer:
            output = "LiqContainer";
            break;
        case ModelType::Tool:
            output = "Tool";
            break;
        case ModelType::Node:
            output = "Node";
            break;
        case ModelType::Resource:
            output = "Resource";
            break;
        case ModelType::Seed:
            output = "Seed";
            break;
        case ModelType::Key:
            output = "Key";
            break;
        case ModelType::Furniture:
            output = "Furniture";
            break;
        case ModelType::Food:
            output = "Food";
            break;
        case ModelType::Light:
            output = "Light";
            break;
        case ModelType::Vehicle:
            output = "Vehicle";
            break;
        case ModelType::Pen:
            output = "Pen";
            break;
        case ModelType::Book:
            output = "Book";
            break;
        case ModelType::Rope:
            output = "Rope";
            break;
        case ModelType::Trash:
            output = "Trash";
            break;
        case ModelType::Mechanism:
            output = "Mechanism";
            break;
        case ModelType::Currency:
            output = "Currency";
            break;
    }
    return output;
}

std::string GetEquipmentSlotName(EquipmentSlot slot)
{
    std::string output;
    switch (slot)
    {
        case EquipmentSlot::Head:
            output = "Head";
            break;
        case EquipmentSlot::Torso:
            output = "Torso";
            break;
        case EquipmentSlot::Back:
            output = "Back";
            break;
        case EquipmentSlot::Legs:
            output = "Legs";
            break;
        case EquipmentSlot::Feet:
            output = "Feet";
            break;
        case EquipmentSlot::RightHand:
            output = "Right Hand";
            break;
        case EquipmentSlot::LeftHand:
            output = "Left Hand";
            break;
        case EquipmentSlot::None:

        default:
            output = "No Equipment Slot";
            break;
    }
    return output;
}

std::string GetWeaponTypeName(WeaponType type)
{
    std::string output;
    switch (type)
    {
        case kPistol:
            output = "Pistol";
            break;
        case kRifle:
            output = "Rifle";
            break;
        case kShotgun:
            output = "Shotgun";
            break;
        case kHeavyWeapon:
            output = "Heavy Weapon";
            break;
        case kEnergyPistol:
            output = "Energy Pistol";
            break;
        case kEnergyRifle:
            output = "Energy Rifle";
            break;
        case kEnergyHeavyWeapon:
            output = "Energy Heavy Weapon";
            break;
        case kGranade:
            output = "Granade";
            break;
        case kPlaced:
            output = "Placed";
            break;
        case kBladed:
            output = "Bladed";
            break;
        case kBlunt:
            output = "Blunt";
            break;
        case kThrown:
            output = "Thrown";
            break;
        case kUnarmed:
            output = "Unarmed";
            break;
        default:
            output = "No Weapon Type";
            break;
    }
    return output;
}

std::string GetArmorSizeName(ArmorSize size)
{
    std::string output;
    switch (size)
    {
        case kASAll:
            output = "Universal Armor";
            break;
        case kASLight:
            output = "Light Armor";
            break;
        case kASMedium:
            output = "Medium Armor";
            break;
        case kASHeavy:
            output = "Heavy Armor";
            break;
        default:
            output = "No Armor Size";
            break;
    }
    return output;
}

std::string GetShieldSizeName(ShieldSize size)
{
    std::string output;
    switch (size)
    {
        case kSSLight:
            output = "Light Shield";
            break;
        case kSSMedium:
            output = "Medium Shield";
            break;
        case kSSHeavy:
            output = "Heavy Shield";
            break;
        default:
            output = "No Shield Size";
            break;
    }
    return output;
}

std::string GetToolTypeName(ToolType type)
{
    std::string output;
    switch (type)
    {
        case ToolType::NoType:
            output = "No Tool Type";
            break;
        case ToolType::Pickaxe:
            output = "Pickaxe";
            break;
        case ToolType::WoodcutterAxe:
            output = "Woodcutter's Axe";
            break;
        case ToolType::Saw:
            output = "Saw";
            break;
        case ToolType::PrecisionChisel:
            output = "Precision Chisel";
            break;
        case ToolType::PlaneChisel:
            output = "Plane Chisel";
            break;
        case ToolType::Hammer:
            output = "Hammer";
            break;
        case ToolType::Forge:
            output = "Forge";
            break;
        case ToolType::Anvil:
            output = "Anvil";
            break;
        case ToolType::BlacksmithHammer:
            output = "Blacksmith Hammer";
            break;
        case ToolType::Bellows:
            output = "Bellows";
            break;
        case ToolType::Crucible:
            output = "Crucible";
            break;
        case ToolType::Tinderbox:
            output = "Tinderbox";
            break;
        default:
            output = "No Tool Type";
            break;
    }
    return output;
}

std::string GetNodeTypeName(NodeType type)
{
    std::string output;
    switch (type)
    {
        case NodeType::NoType:
            output = "No Node Type";
            break;
        case NodeType::Metal:
            output = "Metal Node";
            break;
        case NodeType::Wood:
            output = "Wood Node";
            break;
        case NodeType::Stone:
            output = "Stone Node";
            break;
    }
    return output;
}

std::string GetResourceTypeName(ResourceType type)
{
    std::string output;
    switch (type)
    {
        case ResourceType::NoType:
            output = "No Resource Type";
            break;
        case ResourceType::Coal:
            output = "Natural Coal";
            break;
        case ResourceType::Ore:
            output = "Metal Ore";
            break;
        case ResourceType::Bar:
            output = "Metal Bar";
            break;
        case ResourceType::Log:
            output = "Wood Log";
            break;
        case ResourceType::Plank:
            output = "Wood Plank";
            break;
        case ResourceType::Tree:
            output = "Tree";
            break;
        case ResourceType::Fastener:
            output = "Fastener";
            break;
        case ResourceType::Leather:
            output = "Leather";
            break;
        case ResourceType::Cloth:
            output = "Cloth";
            break;
        case ResourceType::StoneBlock:
            output = "Stone Block";
            break;
        case ResourceType::MetalVein:
            output = "Metal Vein";
            break;
        case ResourceType::StoneMonolith:
            output = "Stone Monolith";
            break;
    }
    return output;
}

std::string GetSeedTypeName(SeedType type)
{
    std::string output;
    switch (type)
    {
        case kSeedTree:
            output = "Tree Seed";
            break;
        case kSeedPlant:
            output = "Plant Seed";
            break;
        default:
            output = "No Seed Type";
            break;
    }
    return output;
}

std::string GetMechanismTypeName(MechanismType type)
{
    std::string output;
    switch (type)
    {
        case MechanismType::Door:
            output = "Door";
            break;
        case MechanismType::Lock:
            output = "Lock";
            break;
        case MechanismType::Picklock:
            output = "Picklock";
            break;
        case MechanismType::Lever:
            output = "Lever";
            break;
        case MechanismType::None:
            output = "No Mechanism Type";
            break;
    }
    return output;
}

std::string GetActionTypeName(ActionType type)
{
    std::string output;
    switch (type)
    {
        case ActionType::NoAction:
            output = "No Action";
            break;
        case ActionType::Wait:
            output = "Waiting";
            break;
        case ActionType::Move:
            output = "Moving";
            break;
        case ActionType::Crafting:
            output = "Crafting";
            break;
        case ActionType::Building:
            output = "Building";
            break;
        case ActionType::Combat:
            output = "Fighting";
            break;
    }
    return output;
}

std::string GetModelFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, ModelFlag::Static))
    {
        flagList += "Static";
    }
    if (HasFlag(flags, ModelFlag::Invisible))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Invisible";
    }
    if (HasFlag(flags, ModelFlag::Unbreakable))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Unbreakable";
    }
    if (HasFlag(flags, ModelFlag::NoSaleable))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "NoSaleable";
    }
    if (HasFlag(flags, ModelFlag::TwoHand))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "TwoHand";
    }
    return flagList;
}

std::string GetRoomFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, RoomFlag::Rent))
    {
        flagList += "Rent";
    }
    if (HasFlag(flags, RoomFlag::Peaceful))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Peaceful";
    }
    if (HasFlag(flags, RoomFlag::TravelPoint))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "TravelPoint";
    }
    if (HasFlag(flags, RoomFlag::SpawnPoint))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "SpawnPoint";
    }
    return flagList;
}

std::string GetCharacterFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, CharacterFlag::IsGod))
    {
        flagList += "IsGod";
    }
    if (HasFlag(flags, CharacterFlag::Invisible))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Invisible";
    }
    return flagList;
}

std::string GetExitFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, ExitFlag::NoMob))
    {
        flagList += "NoMob";
    }
    if (HasFlag(flags, ExitFlag::Hidden))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Hidden";
    }
    if (HasFlag(flags, ExitFlag::Stairs))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Stairs";
    }
    return flagList;
}

std::string GetContainerFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, ContainerFlag::CanClose))
    {
        flagList += "CanClose";
    }
    if (HasFlag(flags, ContainerFlag::CanBurgle))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "CanBurgle";
    }
    if (HasFlag(flags, ContainerFlag::CanSee))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "CanSee";
    }
    return flagList;
}

std::string GetLiqContainerFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, LiqContainerFlag::Poisoned))
    {
        flagList += "Poisoned";
    }
    if (HasFlag(flags, LiqContainerFlag::Endless))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Endless";
    }
    if (HasFlag(flags, LiqContainerFlag::Destroy))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Destroy";
    }
    return flagList;
}

std::string GetFoodFlagString(int flags)
{
    std::string flagList;
    if (HasFlag(flags, FoodFlag::Poisoned))
    {
        flagList += "Poisoned";
    }
    if (HasFlag(flags, FoodFlag::Raw))
    {
        if (!flagList.empty()) flagList += " ";
        flagList += "Raw";
    }
    return flagList;
}
