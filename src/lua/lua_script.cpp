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

#include "lua_script.hpp"
#include "roomUtilityFunctions.hpp"
#include "armorItem.hpp"
#include "corpseItem.hpp"
#include "currencyItem.hpp"
#include "shopItem.hpp"
#include "logger.hpp"
#include "aStar.hpp"
#include "mud.hpp"

void LuaLog(std::string message)
{
    if (!message.empty())
    {
        Logger::log(LogLevel::Info, "[LUA]" + message);
    }
}

void LuaSleep(int sleepTime)
{
    if ((sleepTime > 0) && (sleepTime <= 20))
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }
}

int LuaRandom(int min, int max)
{
    return TRandInteger<int>(min, max);
}

void LuaStopScript()
{
    throw std::logic_error("Stopped Lua Script");
}

Item * LuaLoadItem(Character * character,
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
    Character * character,
    Room * destination)
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
    auto RoomCheckFunction = [&](Room * from, Room * to)
    {
        // Preapre the options.
        MovementOptions options;
        options.character = character;
        // Prepare the error string.
        std::string error;
        return CheckConnection(options, from, to, error);
    };
    // Find the path from the actor to the target.
    AStar<Room *> aStar(RoomCheckFunction,
                        RoomGetDistance,
                        RoomAreEqual,
                        RoomGetNeighbours);
    std::vector<Room *> visitedRooms;
    if (aStar.findPath(character->room, destination, visitedRooms))
    {
        Coordinates previous = character->room->coord;
        for (auto node : visitedRooms)
        {
            path.emplace_back(Area::getDirection(previous, node->coord));
            previous = node->coord;
        }
    }
    return path;
}

std::vector<Room *> LuaGetRoomsInSight(Character * character)
{
    std::vector<Room *> result;
    if (character == nullptr) return result;
    if (character->room == nullptr) return result;
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

std::vector<Character *> LuaGetCharactersInSight(Character * character)
{
    std::vector<Character *> result;
    if (character == nullptr) return result;
    if (character->room == nullptr) return result;
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

std::vector<Item *> LuaGetItemsInSight(Character * character)
{
    std::vector<Item *> result;
    if (character == nullptr) return result;
    if (character->room == nullptr) return result;
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

void LoadLuaEnvironmet(lua_State * L, const std::string & scriptFile)
{
    // -------------------------------------------------------------------------
    // Open lua libraries.
    luaL_openlibs(L);
    // -------------------------------------------------------------------------
    // Register all the utility functions.
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Mud")
        .addFunction("log", LuaLog)
        .addFunction("sleep", LuaSleep)
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
    luabridge::getGlobalNamespace(L)
        .beginStdVector<Direction>("STDDirectionVector")
        .endClass()
        .beginStdVector<std::shared_ptr<Exit>>("STDExitVector")
        .endClass()
        .beginStdVector<Room *>("STDRoomVector")
        .endClass()
        .beginStdVector<Item *>("STDItemVector")
        .endClass()
        .beginStdVector<Character *>("STDCharacterVector")
        .endClass()
        .beginStdMap<std::string, std::string>("STDStringToStringMap")
        .endClass();
    // -------------------------------------------------------------------------
    // CHARACTER_VECTOR derived from 'std::vector<Character *>'
    luabridge::getGlobalNamespace(L)
        .deriveClass<CharacterVector,
            std::vector<Character *>>("CharacterVector")
        .endClass();
    // -------------------------------------------------------------------------
    // ITEM_VECTOR derived from 'std::vector<Item *>'
    luabridge::getGlobalNamespace(L)
        .deriveClass<ItemVector,
            std::vector<Item *>>("ItemVector")
        .endClass();
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
        .beginWSPtrClass<ItemModel>("ItemModel")
        .addData("vnum", &ItemModel::vnum)
        .addData("condition", &ItemModel::condition)
        .addFunction("getType", &ItemModel::getType)
        .addFunction("toTool", &ItemModel::toTool)
        .endClass()
        .deriveWSPtrClass<ToolModel, ItemModel>("ToolModel")
        .addData("toolType", &ToolModel::toolType)
        .addFunction("getTypeName", &ToolModel::getTypeName)
        .endClass()
        .deriveWSPtrClass<ArmorModel, ItemModel>("ArmorModel")
        .endClass()
        .deriveWSPtrClass<BookModel, ItemModel>("BookModel")
        .endClass()
        .deriveWSPtrClass<ContainerModel, ItemModel>("ContainerModel")
        .endClass()
        .deriveWSPtrClass<CorpseModel, ItemModel>("CorpseModel")
        .endClass()
        .deriveWSPtrClass<CurrencyModel, ItemModel>("CurrencyModel")
        .endClass()
        .deriveWSPtrClass<FoodModel, ItemModel>("FoodModel")
        .endClass()
        .deriveWSPtrClass<FurnitureModel, ItemModel>("FurnitureModel")
        .endClass()
        .deriveWSPtrClass<KeyModel, ItemModel>("KeyModel")
        .endClass()
        .deriveWSPtrClass<LightModel, ItemModel>("LightModel")
        .endClass()
        .deriveWSPtrClass<LiquidContainerModel, ItemModel>(
            "LiquidContainerModel")
        .endClass()
        .deriveWSPtrClass<MagazineItem, ItemModel>("MagazineItem")
        .endClass()
        .deriveWSPtrClass<MechanismModel, ItemModel>("MechanismModel")
        .endClass()
        .deriveWSPtrClass<MeleeWeaponModel, ItemModel>("MeleeWeaponModel")
        .endClass()
        .deriveWSPtrClass<NodeModel, ItemModel>("NodeModel")
        .endClass()
        .deriveWSPtrClass<ProjectileModel, ItemModel>("ProjectileModel")
        .endClass()
        .deriveWSPtrClass<RangedWeaponModel, ItemModel>("RangedWeaponModel")
        .endClass()
        .deriveWSPtrClass<ResourceModel, ItemModel>("ResourceModel")
        .endClass()
        .deriveWSPtrClass<RopeModel, ItemModel>("RopeModel")
        .endClass()
        .deriveWSPtrClass<SeedModel, ItemModel>("SeedModel")
        .endClass()
        .deriveWSPtrClass<ShieldModel, ItemModel>("ShieldModel")
        .endClass()
        .deriveWSPtrClass<ShopModel, ItemModel>("ShopModel")
        .endClass()
        .deriveWSPtrClass<VehicleModel, ItemModel>("VehicleModel")
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
        .beginWSPtrClass<Exit>("Exit")
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
    // DIRECTION
    luabridge::getGlobalNamespace(L)
        .beginClass<Direction>("Direction")
        .addFunction("toUInt", &Direction::toUInt)
        .addFunction("toString", &Direction::toString)
        .endClass()
        .beginEnum<Direction>("Direction")
        .addEnum("North", Direction::North)
        .addEnum("South", Direction::South)
        .addEnum("West", Direction::West)
        .addEnum("East", Direction::East)
        .addEnum("Up", Direction::Up)
        .addEnum("Down", Direction::Down)
        .endEnum();
    // -------------------------------------------------------------------------
    // MODEL_TYPE
    luabridge::getGlobalNamespace(L)
        .beginClass<ModelType>("ModelType")
        .addFunction("toUInt", &ModelType::toUInt)
        .addFunction("toString", &ModelType::toString)
        .endClass()
        .beginEnum<ModelType>("ModelType")
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
        .addEnum("Magazine", ModelType::Magazine)
        .endEnum();
    // -------------------------------------------------------------------------
    // TOOL_TYPE
    luabridge::getGlobalNamespace(L)
        .beginClass<ToolType>("ToolType")
        .addFunction("toUInt", &ToolType::toUInt)
        .addFunction("toString", &ToolType::toString)
        .endClass()
        .beginEnum<ToolType>("ToolType")
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
        .addEnum("Firelighter", ToolType::Firelighter)
        .endEnum();
    // -------------------------------------------------------------------------
    // RESOURCE_TYPE
    luabridge::getGlobalNamespace(L)
        .beginClass<ResourceType>("ResourceType")
        .addFunction("toUInt", &ResourceType::toUInt)
        .addFunction("toString", &ResourceType::toString)
        .endClass()
        .beginEnum<ResourceType>("ResourceType")
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
        .addEnum("Skull", ResourceType::Skull)
        .endEnum();
    // -------------------------------------------------------------------------
    // LIQUID_TYPE
    luabridge::getGlobalNamespace(L)
        .beginClass<LiquidType>("LiquidType")
        .addFunction("toUInt", &LiquidType::toUInt)
        .addFunction("toString", &LiquidType::toString)
        .endClass()
        .beginEnum<LiquidType>("LiquidType")
        .addEnum("None", LiquidType::None)
        .addEnum("Normal", LiquidType::Normal)
        .addEnum("Alcohol", LiquidType::Alcohol)
        .addEnum("Poison", LiquidType::Poison)
        .addEnum("Blood", LiquidType::Blood)
        .addEnum("Lava", LiquidType::Lava)
        .endEnum();
    // -------------------------------------------------------------------------
    // CHARACTER_POSTURE
    luabridge::getGlobalNamespace(L)
        .beginClass<CharacterPosture>("CharacterPosture")
        .addFunction("toUInt", &CharacterPosture::toUInt)
        .addFunction("toString", &CharacterPosture::toString)
        .endClass()
        .beginEnum<CharacterPosture>("CharacterPosture")
        .addEnum("None", CharacterPosture::None)
        .addEnum("Stand", CharacterPosture::Stand)
        .addEnum("Crouch", CharacterPosture::Crouch)
        .addEnum("Sit", CharacterPosture::Sit)
        .addEnum("Prone", CharacterPosture::Prone)
        .addEnum("Rest", CharacterPosture::Rest)
        .addEnum("Sleep", CharacterPosture::Sleep)
        .endEnum();
    // -------------------------------------------------------------------------
    // Load the script.
    auto path = Mud::instance().getMudSystemDirectory() + "lua/" + scriptFile;
    if (luaL_dofile(L, path.c_str()) != LUABRIDGE_LUA_OK)
    {
        Logger::log(LogLevel::Error,
                    "Can't open script %s.", scriptFile);
        Logger::log(LogLevel::Error,
                    "Error :%s",
                    std::string(lua_tostring(L, -1))
        );
    }
}