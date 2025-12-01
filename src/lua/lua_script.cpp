/// @file   lua_script.cpp
/// @brief  Implements all the methods used by lua to interact and communicate
///          with the mud.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
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

#include "lua/lua_script.hpp"
#include "item/subitem/armorItem.hpp"
#include "item/subitem/corpseItem.hpp"
#include "item/subitem/currencyItem.hpp"
#include "item/subitem/shopItem.hpp"
#include "model/submodel/toolModel.hpp"
#include "mud.hpp"
#include "structure/algorithms/AStar/aStar.hpp"
#include "structure/structureUtils.hpp"
#include "utilities/logger.hpp"

#include <LuaBridge/Vector.h>

namespace luabridge
{

/// @brief Class used to register an enumerator inside the lua environment.
template <class T>
class EnumToLua
{
protected:
    lua_State *const L;

public:
    /// @brief Constructor.
    EnumToLua(char const *name, lua_State *const L)
        : L(L)
    {
        // Get the entity inside the lua environment.
        lua_getglobal(L, name);
    }

    /// @brief Add an enumerator.
    template <class EnumValue>
    EnumToLua<T> &addEnum(char const *name, EnumValue value)
    {
        // Push the name of the enum.
        lua_pushstring(L, name);
        // Push the value.
        lua_pushinteger(L, value);
        lua_rawset(L, -3);
        return *this;
    }
};

template <class T>
EnumToLua<T> beginEnum(char const *name, lua_State *const L)
{
    return EnumToLua<T>(name, L);
}

} // namespace luabridge

void LuaLog(std::string message)
{
    if (!message.empty()) {
        Logger::log(LogLevel::Info, "[LUA]" + message);
    }
}

int LuaRandom(int min, int max)
{
    return TRand<int>(min, max);
}

void LuaStopScript()
{
    throw std::logic_error("Stopped Lua Script");
}

Item *LuaLoadItem(Character *character, int vnumModel, int vnumMaterial, unsigned int qualityValue)
{
    auto model = Mud::instance().findItemModel(vnumModel);
    if (model == nullptr) {
        Logger::log(LogLevel::Error, "Can't find model : %s", vnumModel);
        return nullptr;
    }
    auto composition = Mud::instance().findMaterial(vnumMaterial);
    if (composition == nullptr) {
        Logger::log(LogLevel::Error, "Can't find material :", vnumMaterial);
        return nullptr;
    }
    // Create the item.
    auto item = model->createItem(character->getName(), composition, true, ItemQuality(qualityValue));
    return item;
}

std::vector<Direction> LuaFindPath(
    Character *character,
    Room *destination)
{
    std::vector<Direction> path;
    if ((character == nullptr) || (destination == nullptr)) {
        return path;
    }
    if (character->room == nullptr) {
        return path;
    }
    auto RoomCheckFunction = [&](Room *from, Room *to) {
        // Preapre the options.
        MovementOptions options;
        options.character = character;
        // Prepare the error string.
        std::string error;
        return StructUtils::checkConnection(options, from, to, error);
    };
    // Find the path from the actor to the target.
    AStar<Room *> aStar(RoomCheckFunction, StructUtils::getRoomDistance, StructUtils::roomsAreEqual, StructUtils::getNeighbours);
    std::vector<Room *> visitedRooms;
    if (aStar.findPath(character->room, destination, visitedRooms)) {
        Coordinates previous = character->room->coord;
        for (auto node : visitedRooms) {
            path.emplace_back(StructUtils::getDirection(previous, node->coord));
            previous = node->coord;
        }
    }
    return path;
}

std::vector<Room *> LuaGetRoomsInSight(Character *character)
{
    std::vector<Room *> result;
    if (character == nullptr)
        return result;
    if (character->room == nullptr)
        return result;
    auto validCoordinates =
        character->room->area->fov(
            character->room->coord,
            character->getViewDistance());
    for (auto coordinates : validCoordinates) {
        result.emplace_back(character->room->area->getRoom(coordinates));
    }
    return result;
}

std::vector<Character *> LuaGetCharactersInSight(Character *character)
{
    std::vector<Character *> result;
    if (character == nullptr)
        return result;
    if (character->room == nullptr)
        return result;
    CharacterVector exceptions;
    exceptions.emplace_back_character(character);
    auto charactersInSight =
        character->room->area->getCharactersInSight(
            exceptions,
            character->room->coord,
            character->getViewDistance());
    for (auto characterInSight : charactersInSight) {
        result.emplace_back(characterInSight);
    }
    return result;
}

std::vector<Item *> LuaGetItemsInSight(Character *character)
{
    std::vector<Item *> result;
    if (character == nullptr)
        return result;
    if (character->room == nullptr)
        return result;
    ItemVector exceptions;
    auto itemsInSight =
        character->room->area->getItemsInSight(
            exceptions,
            character->room->coord,
            character->getViewDistance());
    for (auto itemInSight : itemsInSight) {
        result.emplace_back(itemInSight);
    }
    return result;
}

void LoadLuaEnvironmet(lua_State *L, const std::string &scriptFile)
{
    // -------------------------------------------------------------------------
    // Open lua libraries.
    luaL_openlibs(L);
    // -------------------------------------------------------------------------
    // Register all the utility functions.
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Mud")
        .addFunction("log", LuaLog)
        .addFunction("random", LuaRandom)
        .addFunction("stop", LuaStopScript)
        .addFunction("loadItem", LuaLoadItem)
        .addFunction("findPath", LuaFindPath)
        .addFunction("getRoomsInSight", LuaGetRoomsInSight)
        .addFunction("getCharactersInSight", LuaGetCharactersInSight)
        .addFunction("getItemsInSight", LuaGetItemsInSight)
        .endNamespace();
    // -------------------------------------------------------------------------
    // Register all the STD structures.
    // luabridge::getGlobalNamespace(L)
    //     .beginStdVector<Direction>("STDDirectionVector")
    //     .endClass()
    //     .beginStdVector<std::shared_ptr<Exit>>("STDExitVector")
    //     .endClass()
    //     .beginStdVector<Room *>("STDRoomVector")
    //     .endClass()
    //     .beginStdVector<Item *>("STDItemVector")
    //     .endClass()
    //     .beginStdVector<Character *>("STDCharacterVector")
    //     .endClass()
    //     .beginStdMap<std::string, std::string>("STDStringToStringMap")
    //     .endClass();
    // -------------------------------------------------------------------------
    // CHARACTER
    luabridge::getGlobalNamespace(L)
        .beginClass<Character>("Character")
        .addData("name", &Character::name)
        .addData("description", &Character::description)
        .addData("gender", &Character::gender)
        .addData("weight", &Character::weight)
        .addData("level", &Character::level)
        .addData("flags", &Character::flags)
        .addData("race", &Character::race)
        .addData("faction", &Character::faction)
        .addData("health", &Character::health)
        .addData("stamina", &Character::stamina)
        .addData("hunger", &Character::hunger)
        .addData("thirst", &Character::thirst)
        .addData("room", &Character::room)
        .addData("inventory", &Character::inventory)
        .addData("equipment", &Character::equipment)
        .addData("posture", &Character::posture)
        .addFunction("getName", &Character::getName)
        .addFunction("getNameCapital", &Character::getNameCapital)
        .addFunction("inventoryAdd", &Character::luaAddInventory)
        .addFunction("inventoryRem", &Character::luaRemInventory)
        .addFunction("equipmentAdd", &Character::luaAddEquipment)
        .addFunction("equipmentRem", &Character::luaRemEquipment)
        .addFunction("doCommand", &Character::doCommand)
        .addFunction("isMobile", &Character::isMobile)
        .addFunction("isPlayer", &Character::isPlayer)
        .addFunction("toMobile", &Character::toMobile)
        .addFunction("toPlayer", &Character::toPlayer)
        .endClass()
        .deriveClass<Mobile, Character>("Mobile")
        .addData("id", &Mobile::id)
        .addData("spawnRoom", &Mobile::respawnRoom)
        .addData("shortdesc", &Mobile::shortdesc)
        .addData("staticdesc", &Mobile::staticdesc)
        .addData("message_buffer", &Mobile::message_buffer)
        .addData("controller", &Mobile::controller)
        .addFunction("isMobile", &Mobile::isMobile)
        .addFunction("isAlive", &Mobile::isAlive)
        .endClass()
        .deriveClass<Player, Character>("Player")
        .addData("age", &Player::age, false)
        .addData("experience", &Player::experience, false)
        .addData("prompt", &Player::prompt, false)
        .addData("rent_room", &Player::rent_room, false)
        .addData("remaining_points", &Player::remaining_points, false)
        .addData("luaVariables", &Player::luaVariables)
        .addFunction("isPlayer", &Player::isPlayer)
        .endClass();
    // -------------------------------------------------------------------------
    // CHARACTER_VECTOR derived from 'std::vector<Character *>'
    luabridge::getGlobalNamespace(L)
        .beginClass<CharacterVector>("CharacterVector")
        .endClass();
    // -------------------------------------------------------------------------
    // AREA
    luabridge::getGlobalNamespace(L)
        .beginClass<Area>("Area")
        .addData("vnum", &Area::vnum, false)
        .addData("name", &Area::name, false)
        .addData("builder", &Area::builder, false)
        .addData("width", &Area::width, false)
        .addData("height", &Area::height, false)
        .addData("elevation", &Area::elevation, false)
        .endClass();
    // -------------------------------------------------------------------------
    // FACTION
    luabridge::getGlobalNamespace(L)
        .beginClass<Faction>("Faction")
        .addData("vnum", &Faction::vnum)
        .addData("name", &Faction::name)
        .endClass();
    // -------------------------------------------------------------------------
    // ITEM MODEL
    luabridge::getGlobalNamespace(L)
        .beginClass<ItemModel>("ItemModel")
        .addData("vnum", &ItemModel::vnum)
        .addData("condition", &ItemModel::condition)
        .addFunction("getType", &ItemModel::getType)
        .addFunction("toTool", &ItemModel::toTool)
        .endClass()
        .deriveClass<ToolModel, ItemModel>("ToolModel")
        .addData("toolType", &ToolModel::toolType)
        .addFunction("getTypeName", &ToolModel::getTypeName)
        .endClass()
        .deriveClass<ArmorModel, ItemModel>("ArmorModel")
        .endClass()
        .deriveClass<BookModel, ItemModel>("BookModel")
        .endClass()
        .deriveClass<ContainerModel, ItemModel>("ContainerModel")
        .endClass()
        .deriveClass<CorpseModel, ItemModel>("CorpseModel")
        .endClass()
        .deriveClass<CurrencyModel, ItemModel>("CurrencyModel")
        .endClass()
        .deriveClass<FoodModel, ItemModel>("FoodModel")
        .endClass()
        .deriveClass<FurnitureModel, ItemModel>("FurnitureModel")
        .endClass()
        .deriveClass<KeyModel, ItemModel>("KeyModel")
        .endClass()
        .deriveClass<LightModel, ItemModel>("LightModel")
        .endClass()
        .deriveClass<LiquidContainerModel, ItemModel>("LiquidContainerModel")
        .endClass()
        .deriveClass<MagazineItem, ItemModel>("MagazineItem")
        .endClass()
        .deriveClass<MechanismModel, ItemModel>("MechanismModel")
        .endClass()
        .deriveClass<MeleeWeaponModel, ItemModel>("MeleeWeaponModel")
        .endClass()
        .deriveClass<NodeModel, ItemModel>("NodeModel")
        .endClass()
        .deriveClass<ProjectileModel, ItemModel>("ProjectileModel")
        .endClass()
        .deriveClass<RangedWeaponModel, ItemModel>("RangedWeaponModel")
        .endClass()
        .deriveClass<ResourceModel, ItemModel>("ResourceModel")
        .endClass()
        .deriveClass<RopeModel, ItemModel>("RopeModel")
        .endClass()
        .deriveClass<SeedModel, ItemModel>("SeedModel")
        .endClass()
        .deriveClass<ShieldModel, ItemModel>("ShieldModel")
        .endClass()
        .deriveClass<ShopModel, ItemModel>("ShopModel")
        .endClass()
        .deriveClass<VehicleModel, ItemModel>("VehicleModel")
        .endClass();
    // -------------------------------------------------------------------------
    // ITEM
    luabridge::getGlobalNamespace(L)
        .beginClass<Item>("Item")
        .addData("vnum", &Item::vnum)
        .addData("model", &Item::model)
        .addFunction("getName", &Item::getName)
        .addFunction("hasKey", &Item::hasKey)
        .addFunction("getType", &Item::getType)
        .addFunction("getTypeName", &Item::getTypeName)
        .addData("maker", &Item::maker)
        .addData("condition", &Item::condition)
        .addData("weight", &Item::weight)
        .addData("price", &Item::price)
        .addData("composition", &Item::composition)
        .addData("room", &Item::room)
        .addData("owner", &Item::owner)
        .addData("container", &Item::container)
        .endClass()
        .deriveClass<ArmorItem, Item>("ArmorItem")
        .addFunction("getAC", &ArmorItem::getArmorClass)
        .endClass()
        .deriveClass<CorpseItem, Item>("CorpseItem")
        .endClass()
        .deriveClass<CurrencyItem, Item>("CurrencyItem")
        .endClass()
        .deriveClass<ShopItem, Item>("ShopItem")
        .endClass()
        .deriveClass<MeleeWeaponItem, Item>("MeleeWeaponItem")
        .endClass()
        .deriveClass<RangedWeaponItem, Item>("RangedWeaponItem")
        .endClass();
    // -------------------------------------------------------------------------
    // ITEM_VECTOR derived from 'std::vector<Item *>'
    luabridge::getGlobalNamespace(L)
        .beginClass<ItemVector>("ItemVector")
        .endClass();
    // -------------------------------------------------------------------------
    // MATERIAL
    luabridge::getGlobalNamespace(L)
        .beginClass<Material>("Material")
        .addData("vnum", &Material::vnum, false)
        .addData("name", &Material::name, false)
        .addData("article", &Material::article, false)
        .addData("type", &Material::type, false)
        .addData("worth", &Material::worth, false)
        .addData("hardness", &Material::hardness, false)
        .addData("lightness", &Material::lightness, false)
        .endClass();
    // -------------------------------------------------------------------------
    // RACE
    luabridge::getGlobalNamespace(L)
        .beginClass<Race>("Race")
        .addData("vnum", &Race::vnum)
        .addData("name", &Race::name)
        .addFunction("getAbility", &Race::getAbility)
        .endClass();
    // -------------------------------------------------------------------------
    // COORDINATES
    luabridge::getGlobalNamespace(L)
        .beginClass<Coordinates>("Coordinates")
        .addFunction("toString", &Coordinates::toString)
        .addData("x", &Coordinates::x, false)
        .addData("y", &Coordinates::y, false)
        .addData("z", &Coordinates::z, false)
        .endClass();
    // -------------------------------------------------------------------------
    // EXIT
    luabridge::getGlobalNamespace(L)
        .beginClass<Exit>("Exit")
        .addData("source", &Exit::source)
        .addData("destination", &Exit::destination)
        .addData("direction", &Exit::direction)
        .endClass();
    // -------------------------------------------------------------------------
    // TERRAIN
    luabridge::getGlobalNamespace(L)
        .beginClass<Terrain>("Terrain")
        .addData("vnum", &Terrain::vnum, false)
        .addData("name", &Terrain::name, false)
        .addData("flags", &Terrain::flags, false)
        .addData("space", &Terrain::space, false)
        .endClass();
    // -------------------------------------------------------------------------
    // ROOM
    luabridge::getGlobalNamespace(L)
        .beginClass<Room>("Room")
        .addData("vnum", &Room::vnum, false)
        .addData("area", &Room::area, false)
        .addData("coord", &Room::coord, false)
        .addData("terrain", &Room::terrain, false)
        .addData("name", &Room::name, false)
        .addData("description", &Room::description, false)
        .addData("exits", &Room::exits, false)
        .addData("items", &Room::items, false)
        .addData("characters", &Room::characters, false)
        .addData("flags", &Room::flags, false)
        .endClass();
    // -------------------------------------------------------------------------
    // The Enumerators.
    // -------------------------------------------------------------------------
    // BASE_ENUMERATOR
    luabridge::getGlobalNamespace(L)
        .beginClass<BaseEnumerator>("BaseEnumerator")
        .addFunction("toUInt", &BaseEnumerator::toUInt)
        .addFunction("toString", &BaseEnumerator::toString)
        .endClass();
    // -------------------------------------------------------------------------
    // ABILITY
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Ability")
        .addConstant("None", static_cast<int>(Ability::None))
        .addConstant("Strength", static_cast<int>(Ability::Strength))
        .addConstant("Agility", static_cast<int>(Ability::Agility))
        .addConstant("Perception", static_cast<int>(Ability::Perception))
        .addConstant("Constitution", static_cast<int>(Ability::Constitution))
        .addConstant("Intelligence", static_cast<int>(Ability::Intelligence))
        .endNamespace();
    // -------------------------------------------------------------------------
    // CHARACTER_POSTURE
    luabridge::getGlobalNamespace(L)
        .beginNamespace("CharacterPosture")
        .addConstant("None", static_cast<int>(CharacterPosture::None))
        .addConstant("Stand", static_cast<int>(CharacterPosture::Stand))
        .addConstant("Crouch", static_cast<int>(CharacterPosture::Crouch))
        .addConstant("Sit", static_cast<int>(CharacterPosture::Sit))
        .addConstant("Prone", static_cast<int>(CharacterPosture::Prone))
        .addConstant("Rest", static_cast<int>(CharacterPosture::Rest))
        .addConstant("Sleep", static_cast<int>(CharacterPosture::Sleep))
        .endNamespace();
    // -------------------------------------------------------------------------
    // DIRECTION
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Direction")
        .addConstant("None", static_cast<int>(Direction::None))
        .addConstant("North", static_cast<int>(Direction::North))
        .addConstant("South", static_cast<int>(Direction::South))
        .addConstant("West", static_cast<int>(Direction::West))
        .addConstant("East", static_cast<int>(Direction::East))
        .addConstant("Up", static_cast<int>(Direction::Up))
        .addConstant("Down", static_cast<int>(Direction::Down))
        .endNamespace();
    // -------------------------------------------------------------------------
    // COMBAT_MODIFIER
    luabridge::getGlobalNamespace(L)
        .beginNamespace("CombatModifier")
        .addConstant("None", static_cast<int>(CombatModifier::None))
        .addConstant("UnarmedHitRoll", static_cast<int>(CombatModifier::UnarmedHitRoll))
        .addConstant("UnarmedDamage", static_cast<int>(CombatModifier::UnarmedDamage))
        .addConstant("MeleeWeaponHitRoll", static_cast<int>(CombatModifier::MeleeWeaponHitRoll))
        .addConstant("MeleeWeaponDamage", static_cast<int>(CombatModifier::MeleeWeaponDamage))
        .addConstant("RangedWeaponHitRoll", static_cast<int>(CombatModifier::RangedWeaponHitRoll))
        .addConstant("RangedWeaponDamage", static_cast<int>(CombatModifier::RangedWeaponDamage))
        .addConstant("RangedAimSpeed", static_cast<int>(CombatModifier::RangedAimSpeed))
        .addConstant("ArmorClass", static_cast<int>(CombatModifier::ArmorClass))
        .endNamespace();
    // -------------------------------------------------------------------------
    // ITEM_QUALITY
    luabridge::getGlobalNamespace(L)
        .beginNamespace("ItemQuality")
        .addConstant("None", static_cast<int>(ItemQuality::None))
        .addConstant("Disastrous", static_cast<int>(ItemQuality::Disastrous))
        .addConstant("Poor", static_cast<int>(ItemQuality::Poor))
        .addConstant("Normal", static_cast<int>(ItemQuality::Normal))
        .addConstant("Fine", static_cast<int>(ItemQuality::Fine))
        .addConstant("Masterful", static_cast<int>(ItemQuality::Masterful))
        .endNamespace();
    // -------------------------------------------------------------------------
    // KNOWLEDGE
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Knowledge")
        .addConstant("None", static_cast<int>(Knowledge::None))
        .addConstant("GatherHerbs", static_cast<int>(Knowledge::GatherHerbs))
        .addConstant("GatherPlant", static_cast<int>(Knowledge::GatherPlant))
        .addConstant("Butchery", static_cast<int>(Knowledge::Butchery))
        .addConstant("SkinAnimal", static_cast<int>(Knowledge::SkinAnimal))
        .addConstant("TanHide", static_cast<int>(Knowledge::TanHide))
        .addConstant("ReadBook", static_cast<int>(Knowledge::ReadBook))
        .addConstant("Climb", static_cast<int>(Knowledge::Climb))
        .addConstant("Run", static_cast<int>(Knowledge::Run))
        .addConstant("Dash", static_cast<int>(Knowledge::Dash))
        .addConstant("Woodcutting", static_cast<int>(Knowledge::Woodcutting))
        .addConstant("Carpentry", static_cast<int>(Knowledge::Carpentry))
        .addConstant("Woodcarving", static_cast<int>(Knowledge::Woodcarving))
        .addConstant("Mining", static_cast<int>(Knowledge::Mining))
        .addConstant("Smelting", static_cast<int>(Knowledge::Smelting))
        .addConstant("Blacksmithing", static_cast<int>(Knowledge::Blacksmithing))
        .addConstant("MetalWeaponCrafting", static_cast<int>(Knowledge::MetalWeaponCrafting))
        .addConstant("MetalArmorCrafting", static_cast<int>(Knowledge::MetalArmorCrafting))
        .addConstant("CraftSurvivalTool", static_cast<int>(Knowledge::CraftSurvivalTool))
        .addConstant("Scavenge", static_cast<int>(Knowledge::Scavenge))
        .addConstant("BasicArmorProficiency", static_cast<int>(Knowledge::BasicArmorProficiency))
        .endNamespace();
    // -------------------------------------------------------------------------
    // LIQUID_TYPE
    // Removed obsolete deriveClass for LiquidType (now enum class)
    luabridge::getGlobalNamespace(L)
        .beginNamespace("LiquidType")
        .addConstant("None", static_cast<int>(LiquidType::None))
        .addConstant("Normal", static_cast<int>(LiquidType::Normal))
        .addConstant("Alcohol", static_cast<int>(LiquidType::Alcohol))
        .addConstant("Poison", static_cast<int>(LiquidType::Poison))
        .addConstant("Blood", static_cast<int>(LiquidType::Blood))
        .addConstant("Lava", static_cast<int>(LiquidType::Lava))
        .endNamespace();
    // -------------------------------------------------------------------------
    // MATERIAL_TYPE
    luabridge::getGlobalNamespace(L)
        .beginNamespace("MaterialType")
        .addConstant("None", static_cast<int>(MaterialType::None))
        .addConstant("Metal", static_cast<int>(MaterialType::Metal))
        .addConstant("Stone", static_cast<int>(MaterialType::Stone))
        .addConstant("Wood", static_cast<int>(MaterialType::Wood))
        .addConstant("Skin", static_cast<int>(MaterialType::Skin))
        .addConstant("Cloth", static_cast<int>(MaterialType::Cloth))
        .addConstant("Vegetable", static_cast<int>(MaterialType::Vegetable))
        .addConstant("Meat", static_cast<int>(MaterialType::Meat))
        .addConstant("Glass", static_cast<int>(MaterialType::Glass))
        .addConstant("Paper", static_cast<int>(MaterialType::Paper))
        .addConstant("Coal", static_cast<int>(MaterialType::Coal))
        .addConstant("Bone", static_cast<int>(MaterialType::Bone))
        .endNamespace();
    // -------------------------------------------------------------------------
    // MODEL_TYPE
    luabridge::getGlobalNamespace(L)
        .beginNamespace("ModelType")
            .addConstant("None", static_cast<int>(ModelType::None))
            .addConstant("Corpse", static_cast<int>(ModelType::Corpse))
            .addConstant("MeleeWeapon", static_cast<int>(ModelType::MeleeWeapon))
            .addConstant("RangedWeapon", static_cast<int>(ModelType::RangedWeapon))
            .addConstant("Armor", static_cast<int>(ModelType::Armor))
            .addConstant("Shield", static_cast<int>(ModelType::Shield))
            .addConstant("Projectile", static_cast<int>(ModelType::Projectile))
            .addConstant("Container", static_cast<int>(ModelType::Container))
            .addConstant("LiquidContainer", static_cast<int>(ModelType::LiquidContainer))
            .addConstant("Tool", static_cast<int>(ModelType::Tool))
            .addConstant("Node", static_cast<int>(ModelType::Node))
            .addConstant("Resource", static_cast<int>(ModelType::Resource))
            .addConstant("Seed", static_cast<int>(ModelType::Seed))
            .addConstant("Key", static_cast<int>(ModelType::Key))
            .addConstant("Furniture", static_cast<int>(ModelType::Furniture))
            .addConstant("Food", static_cast<int>(ModelType::Food))
            .addConstant("Light", static_cast<int>(ModelType::Light))
            .addConstant("Vehicle", static_cast<int>(ModelType::Vehicle))
            .addConstant("Book", static_cast<int>(ModelType::Book))
            .addConstant("Rope", static_cast<int>(ModelType::Rope))
            .addConstant("Mechanism", static_cast<int>(ModelType::Mechanism))
            .addConstant("Currency", static_cast<int>(ModelType::Currency))
            .addConstant("Shop", static_cast<int>(ModelType::Shop))
            .addConstant("Magazine", static_cast<int>(ModelType::Magazine))
            .addFunction("toString", &model_type_to_string)
            .addFunction("fromString", &string_to_model_type)
        .endNamespace();
    // -------------------------------------------------------------------------
    // RESOURCE_TYPE
    luabridge::getGlobalNamespace(L)
        .beginNamespace("ResourceType")
            .addConstant("None", static_cast<int>(ResourceType::None))
            .addConstant("Coal", static_cast<int>(ResourceType::Coal))
            .addConstant("Ore", static_cast<int>(ResourceType::Ore))
            .addConstant("Bar", static_cast<int>(ResourceType::Bar))
            .addConstant("Log", static_cast<int>(ResourceType::Log))
            .addConstant("Plank", static_cast<int>(ResourceType::Plank))
            .addConstant("Tree", static_cast<int>(ResourceType::Tree))
            .addConstant("Fastener", static_cast<int>(ResourceType::Fastener))
            .addConstant("Leather", static_cast<int>(ResourceType::Leather))
            .addConstant("Cloth", static_cast<int>(ResourceType::Cloth))
            .addConstant("StoneBlock", static_cast<int>(ResourceType::StoneBlock))
            .addConstant("MetalVein", static_cast<int>(ResourceType::MetalVein))
            .addConstant("StoneMonolith", static_cast<int>(ResourceType::StoneMonolith))
            .addConstant("Pen", static_cast<int>(ResourceType::Pen))
            .addConstant("Trash", static_cast<int>(ResourceType::Trash))
            .addConstant("Meat", static_cast<int>(ResourceType::Meat))
            .addConstant("Bone", static_cast<int>(ResourceType::Bone))
            .addConstant("Skull", static_cast<int>(ResourceType::Skull))
            .addConstant("Nail", static_cast<int>(ResourceType::Nail))
            .addFunction("toString", &resource_type_to_string)
            .addFunction("fromString", &string_to_resource_type)
        .endNamespace();
    // -------------------------------------------------------------------------
    // TOOL_TYPE
    luabridge::getGlobalNamespace(L)
        .deriveClass<ToolType, BaseEnumerator>("ToolType")
        .endClass();
    luabridge::beginEnum<ToolType>("ToolType", L)
        .addEnum("None", ToolType::None)
        .addEnum("Pickaxe", ToolType::Pickaxe)
        .addEnum("WoodcutterAxe", ToolType::WoodcutterAxe)
        .addEnum("Saw", ToolType::Saw)
        .addEnum("PrecisionChisel", ToolType::PrecisionChisel)
        .addEnum("Hammer", ToolType::Hammer)
        .addEnum("PlaneChisel", ToolType::PlaneChisel)
        .addEnum("Forge", ToolType::Forge)
        .addEnum("Anvil", ToolType::Anvil)
        .addEnum("BlacksmithHammer", ToolType::BlacksmithHammer)
        .addEnum("Bellows", ToolType::Bellows)
        .addEnum("Crucible", ToolType::Crucible)
        .addEnum("Firelighter", ToolType::Firelighter);
    // -------------------------------------------------------------------------
    // Load the script.
    auto path = Mud::instance().getMudSystemDirectory() + "lua/" + scriptFile;
    if (luaL_dofile(L, path.c_str()) != LUABRIDGE_LUA_OK) {
        Logger::log(LogLevel::Error, "Can't open script %s.", scriptFile);
        Logger::log(LogLevel::Error, "Error :%s", std::string(lua_tostring(L, -1)));
    }
}