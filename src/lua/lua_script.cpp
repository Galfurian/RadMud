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
#include "structure/structureUtils.hpp"
#include "item/subitem/armorItem.hpp"
#include "item/subitem/corpseItem.hpp"
#include "item/subitem/currencyItem.hpp"
#include "model/submodel/toolModel.hpp"
#include "item/subitem/shopItem.hpp"
#include "utilities/logger.hpp"
#include "structure/algorithms/AStar/aStar.hpp"
#include "mud.hpp"

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
        EnumToLua(char const *name, lua_State *const L) : L(L)
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
    if (!message.empty())
    {
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

Item *LuaLoadItem(Character *character,
                  int vnumModel,
                  int vnumMaterial,
                  unsigned int qualityValue)
{
    auto model = Mud::instance().findItemModel(vnumModel);
    if (model == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find model : %s", vnumModel);
        return nullptr;
    }
    auto composition = Mud::instance().findMaterial(vnumMaterial);
    if (composition == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find material :", vnumMaterial);
        return nullptr;
    }
    // Create the item.
    auto item = model->createItem(character->getName(),
                                  composition,
                                  true,
                                  ItemQuality(qualityValue));
    return item;
}

std::vector<Direction> LuaFindPath(
    Character *character,
    Room *destination)
{
    std::vector<Direction> path;
    if ((character == nullptr) || (destination == nullptr))
    {
        return path;
    }
    if (character->room == nullptr)
    {
        return path;
    }
    auto RoomCheckFunction = [&](Room *from, Room *to)
    {
        // Preapre the options.
        MovementOptions options;
        options.character = character;
        // Prepare the error string.
        std::string error;
        return StructUtils::checkConnection(options, from, to, error);
    };
    // Find the path from the actor to the target.
    AStar<Room *> aStar(RoomCheckFunction,
                        StructUtils::getRoomDistance,
                        StructUtils::roomsAreEqual,
                        StructUtils::getNeighbours);
    std::vector<Room *> visitedRooms;
    if (aStar.findPath(character->room, destination, visitedRooms))
    {
        Coordinates previous = character->room->coord;
        for (auto node : visitedRooms)
        {
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
    for (auto coordinates : validCoordinates)
    {
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
    for (auto characterInSight : charactersInSight)
    {
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
    for (auto itemInSight : itemsInSight)
    {
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
        .deriveClass<Ability, BaseEnumerator>("Ability")
        .endClass();
    luabridge::beginEnum<Ability>("Ability", L)
        .addEnum("Strength", Ability::Strength)
        .addEnum("Agility", Ability::Agility)
        .addEnum("Perception", Ability::Perception)
        .addEnum("Constitution", Ability::Constitution)
        .addEnum("Intelligence", Ability::Intelligence);
    // -------------------------------------------------------------------------
    // CHARACTER_POSTURE
    luabridge::getGlobalNamespace(L)
        .deriveClass<CharacterPosture, BaseEnumerator>("CharacterPosture")
        .endClass();
    luabridge::beginEnum<CharacterPosture>("CharacterPosture", L)
        .addEnum("None", CharacterPosture::None)
        .addEnum("Stand", CharacterPosture::Stand)
        .addEnum("Crouch", CharacterPosture::Crouch)
        .addEnum("Sit", CharacterPosture::Sit)
        .addEnum("Prone", CharacterPosture::Prone)
        .addEnum("Rest", CharacterPosture::Rest)
        .addEnum("Sleep", CharacterPosture::Sleep);
    // -------------------------------------------------------------------------
    // DIRECTION
    luabridge::getGlobalNamespace(L)
        .deriveClass<Direction, BaseEnumerator>("Direction")
        .endClass();
    luabridge::beginEnum<Direction>("Direction", L)
        .addEnum("None", Direction::None)
        .addEnum("North", Direction::North)
        .addEnum("South", Direction::South)
        .addEnum("West", Direction::West)
        .addEnum("East", Direction::East)
        .addEnum("Up", Direction::Up)
        .addEnum("Down", Direction::Down);
    // -------------------------------------------------------------------------
    // ITEM_QUALITY
    luabridge::getGlobalNamespace(L)
        .deriveClass<ItemQuality, BaseEnumerator>("ItemQuality")
        .endClass();
    luabridge::beginEnum<ItemQuality>("ItemQuality", L)
        .addEnum("None", ItemQuality::None)
        .addEnum("Disastrous", ItemQuality::Disastrous)
        .addEnum("Poor", ItemQuality::Poor)
        .addEnum("Normal", ItemQuality::Normal)
        .addEnum("Fine", ItemQuality::Fine)
        .addEnum("Masterful", ItemQuality::Masterful);
    // -------------------------------------------------------------------------
    // KNOWLEDGE
    luabridge::getGlobalNamespace(L)
        .deriveClass<Knowledge, BaseEnumerator>("Knowledge")
        .endClass();
    luabridge::beginEnum<Knowledge>("Knowledge", L)
        .addEnum("None", Knowledge::None)
        .addEnum("GatherHerbs", Knowledge::GatherHerbs)
        .addEnum("GatherPlant", Knowledge::GatherPlant)
        .addEnum("Butchery", Knowledge::Butchery)
        .addEnum("SkinAnimal", Knowledge::SkinAnimal)
        .addEnum("TanHide", Knowledge::TanHide)
        .addEnum("ReadBook", Knowledge::ReadBook)
        .addEnum("Climb", Knowledge::Climb)
        .addEnum("Run", Knowledge::Run)
        .addEnum("Dash", Knowledge::Dash)
        .addEnum("Carpentry", Knowledge::Carpentry)
        .addEnum("Woodcutting", Knowledge::Woodcutting)
        .addEnum("Woodcarving", Knowledge::Woodcarving)
        .addEnum("Mining", Knowledge::Mining)
        .addEnum("Smelting", Knowledge::Smelting)
        .addEnum("Blacksmithing", Knowledge::Blacksmithing)
        .addEnum("MetalWeaponCrafting", Knowledge::MetalWeaponCrafting)
        .addEnum("MetalArmorCrafting", Knowledge::MetalArmorCrafting)
        .addEnum("CraftSurvivalTool", Knowledge::CraftSurvivalTool)
        .addEnum("Scavenge", Knowledge::Scavenge)
        .addEnum("BasicArmorProficiency", Knowledge::BasicArmorProficiency);
    // -------------------------------------------------------------------------
    // LIQUID_TYPE
    luabridge::getGlobalNamespace(L)
        .deriveClass<LiquidType, BaseEnumerator>("LiquidType")
        .endClass();
    luabridge::beginEnum<LiquidType>("LiquidType", L)
        .addEnum("None", LiquidType::None)
        .addEnum("Normal", LiquidType::Normal)
        .addEnum("Alcohol", LiquidType::Alcohol)
        .addEnum("Poison", LiquidType::Poison)
        .addEnum("Blood", LiquidType::Blood)
        .addEnum("Lava", LiquidType::Lava);
    // -------------------------------------------------------------------------
    // MATERIAL_TYPE
    luabridge::getGlobalNamespace(L)
        .deriveClass<MaterialType, BaseEnumerator>("MaterialType")
        .endClass();
    luabridge::beginEnum<MaterialType>("MaterialType", L)
        .addEnum("None", MaterialType::None)
        .addEnum("Metal", MaterialType::Metal)
        .addEnum("Stone", MaterialType::Stone)
        .addEnum("Wood", MaterialType::Wood)
        .addEnum("Skin", MaterialType::Skin)
        .addEnum("Cloth", MaterialType::Cloth)
        .addEnum("Vegetable", MaterialType::Vegetable)
        .addEnum("Meat", MaterialType::Meat)
        .addEnum("Glass", MaterialType::Glass)
        .addEnum("Paper", MaterialType::Paper)
        .addEnum("Coal", MaterialType::Coal)
        .addEnum("Bone", MaterialType::Bone);
    // -------------------------------------------------------------------------
    // MODEL_TYPE
    luabridge::getGlobalNamespace(L)
        .deriveClass<ModelType, BaseEnumerator>("ModelType")
        .endClass();
    luabridge::beginEnum<ModelType>("ModelType", L)
        .addEnum("None", ModelType::None)
        .addEnum("Corpse", ModelType::Corpse)
        .addEnum("MeleeWeapon", ModelType::MeleeWeapon)
        .addEnum("RangedWeapon", ModelType::RangedWeapon)
        .addEnum("Armor", ModelType::Armor)
        .addEnum("Shield", ModelType::Shield)
        .addEnum("Projectile", ModelType::Projectile)
        .addEnum("Container", ModelType::Container)
        .addEnum("LiquidContainer", ModelType::LiquidContainer)
        .addEnum("Tool", ModelType::Tool)
        .addEnum("Node", ModelType::Node)
        .addEnum("Resource", ModelType::Resource)
        .addEnum("Seed", ModelType::Seed)
        .addEnum("Key", ModelType::Key)
        .addEnum("Furniture", ModelType::Furniture)
        .addEnum("Food", ModelType::Food)
        .addEnum("Light", ModelType::Light)
        .addEnum("Vehicle", ModelType::Vehicle)
        .addEnum("Book", ModelType::Book)
        .addEnum("Rope", ModelType::Rope)
        .addEnum("Mechanism", ModelType::Mechanism)
        .addEnum("Currency", ModelType::Currency)
        .addEnum("Shop", ModelType::Shop)
        .addEnum("Magazine", ModelType::Magazine);
    // -------------------------------------------------------------------------
    // RESOURCE_TYPE
    luabridge::getGlobalNamespace(L)
        .deriveClass<ResourceType, BaseEnumerator>("ResourceType")
        .endClass();
    luabridge::beginEnum<ResourceType>("ResourceType", L)
        .addEnum("None", ResourceType::None)
        .addEnum("Coal", ResourceType::Coal)
        .addEnum("Ore", ResourceType::Ore)
        .addEnum("Bar", ResourceType::Bar)
        .addEnum("Log", ResourceType::Log)
        .addEnum("Plank", ResourceType::Plank)
        .addEnum("Tree", ResourceType::Tree)
        .addEnum("Fastener", ResourceType::Fastener)
        .addEnum("Leather", ResourceType::Leather)
        .addEnum("Cloth", ResourceType::Cloth)
        .addEnum("StoneBlock", ResourceType::StoneBlock)
        .addEnum("MetalVein", ResourceType::MetalVein)
        .addEnum("StoneMonolith", ResourceType::StoneMonolith)
        .addEnum("Pen", ResourceType::Pen)
        .addEnum("Trash", ResourceType::Trash)
        .addEnum("Meat", ResourceType::Meat)
        .addEnum("Bone", ResourceType::Bone)
        .addEnum("Skull", ResourceType::Skull);
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
    if (luaL_dofile(L, path.c_str()) != LUABRIDGE_LUA_OK)
    {
        Logger::log(LogLevel::Error,
                    "Can't open script %s.", scriptFile);
        Logger::log(LogLevel::Error,
                    "Error :%s",
                    std::string(lua_tostring(L, -1)));
    }
}