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
#include "structure/algorithms/AStar/aStar.hpp"
#include "mud.hpp"

#include <LuaBridge/Vector.h>

void LuaLog(std::string message)
{
	if (!message.empty()) {
		MudLog(LogLevel::Info, "[LUA] %s", message);
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

Item *LuaLoadItem(Character *character, unsigned int vnumModel, unsigned int vnumMaterial,
				  unsigned int qualityValue)
{
	auto model = Mud::instance().findItemModel(vnumModel);
	if (model == nullptr) {
		MudLog(LogLevel::Error, "Can't find model : %s", vnumModel);
		return nullptr;
	}
	auto composition = Mud::instance().findMaterial(vnumMaterial);
	if (composition == nullptr) {
		MudLog(LogLevel::Error, "Can't find material :", vnumMaterial);
		return nullptr;
	}
	// Create the item.
	auto item =
		model->createItem(character->getName(), composition, true, ItemQuality(qualityValue));
	return item;
}

std::vector<Direction> LuaFindPath(Character *character, Room *destination)
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
	AStar<Room *> aStar(RoomCheckFunction, StructUtils::getRoomDistance, StructUtils::roomsAreEqual,
						StructUtils::getNeighbours);
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
	if (character->room->area == nullptr)
		return result;
	auto validCoordinates = StructUtils::fov(character->room->coord, character->getViewDistance(),
											 character->room->area);
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
	auto charactersInSight = character->room->area->getCharactersInSight(
		exceptions, character->room->coord, character->getViewDistance());
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
	auto itemsInSight = character->room->area->getItemsInSight(exceptions, character->room->coord,
															   character->getViewDistance());
	for (auto itemInSight : itemsInSight) {
		result.emplace_back(itemInSight);
	}
	return result;
}

int SetLuaPath(lua_State *L, const char *path)
{
	lua_getglobal(L, "package");
	// Get field "path" from table at top of stack (-1)
	lua_getfield(L, -1, "path");
	// Grab path string from top of stack
	std::string cur_path = lua_tostring(L, -1);
	// Do your path magic here
	MudLog(LogLevel::Debug, "Lua old package.path : %s", cur_path);
	cur_path.push_back(';');
	cur_path.append(path);
	MudLog(LogLevel::Debug, "Lua new package.path : %s", cur_path);
	// Get rid of the string on the stack we just pushed on line 5
	lua_pop(L, 1);
	// Push the new one
	lua_pushstring(L, cur_path.c_str());
	// Set the field "path" in table at -2 with value at top of stack
	lua_setfield(L, -2, "path");
	// Get rid of package table from top of stack
	lua_pop(L, 1);
	return 0;
}

bool LoadLuaEnvironmet(lua_State *L, const std::string &scriptFile)
{
	// -------------------------------------------------------------------------
	// Open lua libraries.
	luaL_openlibs(L);

	// -------------------------------------------------------------------------
	// Update package.path
	SetLuaPath(L, "../system/lua/lib/?.lua");

	// -------------------------------------------------------------------------
	// Register all the utility functions.
	luabridge::getGlobalNamespace(L)
		.beginNamespace("MudFunction")
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
	// CHARACTER_VECTOR derived from 'std::vector<Character *>'
	luabridge::getGlobalNamespace(L)
		.beginClass<CharacterVector>("CharacterVector")
		.endClass();
	// -------------------------------------------------------------------------
	// ITEM_VECTOR derived from 'std::vector<Item *>'
	luabridge::getGlobalNamespace(L)
		.beginClass<ItemVector>("ItemVector")
		.endClass();
	// -------------------------------------------------------------------------
	// CHARACTER
	luabridge::getGlobalNamespace(L)
		.beginClass<Character>("Character")
		.addProperty("name", &Character::name)
		.addProperty("description", &Character::description)
		.addProperty("gender", &Character::gender)
		.addProperty("weight", &Character::weight)
		.addProperty("level", &Character::level)
		.addProperty("flags", &Character::flags)
		.addProperty("race", &Character::race)
		.addProperty("faction", &Character::faction)
		.addProperty("health", &Character::health)
		.addProperty("stamina", &Character::stamina)
		.addProperty("hunger", &Character::hunger)
		.addProperty("thirst", &Character::thirst)
		.addProperty("room", &Character::room)
		.addProperty("inventory", &Character::inventory)
		.addProperty("equipment", &Character::equipment)
		.addProperty("posture", &Character::posture)
		.addFunction("getName", &Character::getName)
		.addFunction("getNameCapital", &Character::getNameCapital)
		.addFunction("inventoryAdd",
					 [](Character *character, Item *item) { character->addInventoryItem(item); })
		.addFunction("inventoryRem", &Character::remInventoryItem)
		.addFunction("equipmentAdd",
					 [](Character *character, Item *item) {
						 std::string error;
						 auto occupiedBodyParts = character->canWear(item, error);
						 if (occupiedBodyParts.empty()) {
							 MudLog(LogLevel::Error, "The mobile %s cannot equip %s.",
									character->getName(), item->getName());
							 MudLog(LogLevel::Error, "Error: %s", error);
							 return;
						 } else {
							 item->setOccupiedBodyParts(occupiedBodyParts);
							 character->addEquipmentItem(item);
						 }
					 })
		.addFunction("equipmentRem", &Character::remEquipmentItem)
		.addFunction("doCommand", &Character::doCommand)
		.addFunction("isMobile", &Character::isMobile)
		.addFunction("isPlayer", &Character::isPlayer)
		.addFunction("toMobile", &Character::toMobile)
		.addFunction("toPlayer", &Character::toPlayer)
		.endClass()
		.deriveClass<Mobile, Character>("Mobile")
		.addProperty("vnum", &Mobile::vnum)
		.addProperty("spawnRoom", &Mobile::respawnRoom)
		.addProperty("shortdesc", &Mobile::shortdesc)
		.addProperty("staticdesc", &Mobile::staticdesc)
		.addProperty("controller", &Mobile::controller)
		.addFunction("isMobile", &Mobile::isMobile)
		.addFunction("isAlive", &Mobile::isAlive)
		.endClass()
		.deriveClass<Player, Character>("Player")
		.addProperty("age", &Player::age)
		.addProperty("experience", &Player::experience)
		.addProperty("prompt", &Player::prompt)
		.addProperty("rent_room", &Player::rent_room)
		.addProperty("remaining_points", &Player::remaining_points)
		.addProperty("luaVariables", &Player::luaVariables)
		.addFunction("isPlayer", &Player::isPlayer)
		.endClass();
	// -------------------------------------------------------------------------
	// AREA
	luabridge::getGlobalNamespace(L)
		.beginClass<Area>("Area")
		.addProperty("vnum", &Area::vnum)
		.addProperty("name", &Area::name)
		.addProperty("builder", &Area::builder)
		.addProperty("width", &Area::width)
		.addProperty("height", &Area::height)
		.addProperty("elevation", &Area::elevation)
		.endClass();
	// -------------------------------------------------------------------------
	// FACTION
	luabridge::getGlobalNamespace(L)
		.beginClass<Faction>("Faction")
		.addProperty("vnum", &Faction::vnum)
		.addProperty("name", &Faction::name)
		.endClass();
	// -------------------------------------------------------------------------
	// ITEM MODEL
	luabridge::getGlobalNamespace(L)
		.beginClass<ItemModel>("ItemModel")
		.addProperty("vnum", &ItemModel::vnum)
		.addFunction("getType", &ItemModel::getType)
		.addFunction("toTool", &ItemModel::to<ToolModel>)
		.endClass()
		.deriveClass<ToolModel, ItemModel>("ToolModel")
		.addProperty("toolType", &ToolModel::toolType)
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
		.addProperty("vnum", &Item::vnum)
		.addProperty("model", &Item::model)
		.addFunction("getName", &Item::getName)
		.addFunction("hasKey", &Item::hasKey)
		.addFunction("getType", &Item::getType)
		.addFunction("getTypeName", &Item::getTypeName)
		.addProperty("maker", &Item::maker)
		.addProperty("condition", &Item::condition)
		.addProperty("composition", &Item::composition)
		.addProperty("room", &Item::room)
		.addProperty("owner", &Item::owner)
		.addProperty("container", &Item::container)
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
		.addProperty("vnum", &Material::vnum)
		.addProperty("name", &Material::name)
		.addProperty("article", &Material::article)
		.addProperty("type", &Material::type)
		.addProperty("worth", &Material::worth)
		.addProperty("hardness", &Material::hardness)
		.addProperty("lightness", &Material::lightness)
		.endClass();
	// -------------------------------------------------------------------------
	// RACE
	luabridge::getGlobalNamespace(L)
		.beginClass<Race>("Race")
		.addProperty("vnum", &Race::vnum)
		.addProperty("name", &Race::name)
		.addFunction("getAbility", &Race::getAbility)
		.endClass();
	// -------------------------------------------------------------------------
	// COORDINATES
	luabridge::getGlobalNamespace(L)
		.beginClass<Coordinates>("Coordinates")
		.addFunction("toString", &Coordinates::toString)
		.addProperty("x", &Coordinates::x)
		.addProperty("y", &Coordinates::y)
		.addProperty("z", &Coordinates::z)
		.endClass();
	// -------------------------------------------------------------------------
	// EXIT
	luabridge::getGlobalNamespace(L)
		.beginClass<Exit>("Exit")
		.addConstructorFrom<std::shared_ptr<Exit>, void(), void()>()
		.addConstructorFrom<std::shared_ptr<Exit>, void(),
							void(Room *, Room *, Direction, unsigned int)>()
		.addProperty("source", &Exit::source)
		.addProperty("destination", &Exit::destination)
		.addProperty("direction", &Exit::direction)
		.endClass();
	// -------------------------------------------------------------------------
	// TERRAIN
	luabridge::getGlobalNamespace(L)
		.beginClass<Terrain>("Terrain")
		.addProperty("vnum", &Terrain::vnum)
		.addProperty("name", &Terrain::name)
		.addProperty("flags", &Terrain::flags)
		.addProperty("space", &Terrain::space)
		.endClass();
	// -------------------------------------------------------------------------
	// ROOM
	luabridge::getGlobalNamespace(L)
		.beginClass<Room>("Room")
		.addProperty("vnum", &Room::vnum)
		.addProperty("area", &Room::area)
		.addProperty("coord", &Room::coord)
		.addProperty("terrain", &Room::terrain)
		.addProperty("name", &Room::name)
		.addProperty("description", &Room::description)
		.addProperty("exits", &Room::exits)
		.addProperty("items", &Room::items)
		.addProperty("characters", &Room::characters)
		.addProperty("flags", &Room::flags)
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
		.endClass()
		.beginNamespace("Ability")
		.addVariable("Strength", Ability::Strength)
		.addVariable("Agility", Ability::Agility)
		.addVariable("Perception", Ability::Perception)
		.addVariable("Constitution", Ability::Constitution)
		.addVariable("Intelligence", Ability::Intelligence)
		.endNamespace();
	// -------------------------------------------------------------------------
	// CHARACTER_POSTURE
	luabridge::getGlobalNamespace(L)
		.deriveClass<CharacterPosture, BaseEnumerator>("CharacterPosture")
		.endClass()
		.beginNamespace("CharacterPosture")
		.addVariable("None", CharacterPosture::None)
		.addVariable("Stand", CharacterPosture::Stand)
		.addVariable("Crouch", CharacterPosture::Crouch)
		.addVariable("Sit", CharacterPosture::Sit)
		.addVariable("Prone", CharacterPosture::Prone)
		.addVariable("Rest", CharacterPosture::Rest)
		.addVariable("Sleep", CharacterPosture::Sleep)
		.endNamespace();
	// -------------------------------------------------------------------------
	// DIRECTION
	luabridge::getGlobalNamespace(L)
		.deriveClass<Direction, BaseEnumerator>("Direction")
		.endClass()
		.beginNamespace("Direction")
		.addVariable("None", Direction::None)
		.addVariable("North", Direction::North)
		.addVariable("South", Direction::South)
		.addVariable("West", Direction::West)
		.addVariable("East", Direction::East)
		.addVariable("Up", Direction::Up)
		.addVariable("Down", Direction::Down)
		.endNamespace();
	// -------------------------------------------------------------------------
	// ITEM_QUALITY
	luabridge::getGlobalNamespace(L)
		.deriveClass<ItemQuality, BaseEnumerator>("ItemQuality")
		.endClass()
		.beginNamespace("ItemQuality")
		.addVariable("None", ItemQuality::None)
		.addVariable("Disastrous", ItemQuality::Disastrous)
		.addVariable("Poor", ItemQuality::Poor)
		.addVariable("Normal", ItemQuality::Normal)
		.addVariable("Fine", ItemQuality::Fine)
		.addVariable("Masterful", ItemQuality::Masterful)
		.endNamespace();
	// -------------------------------------------------------------------------
	// KNOWLEDGE
	luabridge::getGlobalNamespace(L)
		.deriveClass<Knowledge, BaseEnumerator>("Knowledge")
		.endClass()
		.beginNamespace("Knowledge")
		.addVariable("None", Knowledge::None)
		.addVariable("GatherHerbs", Knowledge::GatherHerbs)
		.addVariable("GatherPlant", Knowledge::GatherPlant)
		.addVariable("Butchery", Knowledge::Butchery)
		.addVariable("SkinAnimal", Knowledge::SkinAnimal)
		.addVariable("TanHide", Knowledge::TanHide)
		.addVariable("ReadBook", Knowledge::ReadBook)
		.addVariable("Climb", Knowledge::Climb)
		.addVariable("Run", Knowledge::Run)
		.addVariable("Dash", Knowledge::Dash)
		.addVariable("Carpentry", Knowledge::Carpentry)
		.addVariable("Woodcutting", Knowledge::Woodcutting)
		.addVariable("Woodcarving", Knowledge::Woodcarving)
		.addVariable("Mining", Knowledge::Mining)
		.addVariable("Smelting", Knowledge::Smelting)
		.addVariable("Blacksmithing", Knowledge::Blacksmithing)
		.addVariable("MetalWeaponCrafting", Knowledge::MetalWeaponCrafting)
		.addVariable("MetalArmorCrafting", Knowledge::MetalArmorCrafting)
		.addVariable("CraftSurvivalTool", Knowledge::CraftSurvivalTool)
		.addVariable("Scavenge", Knowledge::Scavenge)
		.addVariable("BasicArmorProficiency", Knowledge::BasicArmorProficiency)
		.endNamespace();
	// -------------------------------------------------------------------------
	// LIQUID_TYPE
	luabridge::getGlobalNamespace(L)
		.deriveClass<LiquidType, BaseEnumerator>("LiquidType")
		.endClass()
		.beginNamespace("LiquidType")
		.addVariable("None", LiquidType::None)
		.addVariable("Normal", LiquidType::Normal)
		.addVariable("Alcohol", LiquidType::Alcohol)
		.addVariable("Poison", LiquidType::Poison)
		.addVariable("Blood", LiquidType::Blood)
		.addVariable("Lava", LiquidType::Lava)
		.endNamespace();
	// -------------------------------------------------------------------------
	// MATERIAL_TYPE
	luabridge::getGlobalNamespace(L)
		.deriveClass<MaterialType, BaseEnumerator>("MaterialType")
		.endClass()
		.beginNamespace("MaterialType")
		.addVariable("None", MaterialType::None)
		.addVariable("Metal", MaterialType::Metal)
		.addVariable("Stone", MaterialType::Stone)
		.addVariable("Wood", MaterialType::Wood)
		.addVariable("Skin", MaterialType::Skin)
		.addVariable("Cloth", MaterialType::Cloth)
		.addVariable("Vegetable", MaterialType::Vegetable)
		.addVariable("Meat", MaterialType::Meat)
		.addVariable("Glass", MaterialType::Glass)
		.addVariable("Paper", MaterialType::Paper)
		.addVariable("Coal", MaterialType::Coal)
		.addVariable("Bone", MaterialType::Bone)
		.endNamespace();
	// -------------------------------------------------------------------------
	// MODEL_TYPE
	luabridge::getGlobalNamespace(L)
		.deriveClass<ModelType, BaseEnumerator>("ModelType")
		.endClass()
		.beginNamespace("ModelType")
		.addVariable("None", ModelType::None)
		.addVariable("Corpse", ModelType::Corpse)
		.addVariable("MeleeWeapon", ModelType::MeleeWeapon)
		.addVariable("RangedWeapon", ModelType::RangedWeapon)
		.addVariable("Armor", ModelType::Armor)
		.addVariable("Shield", ModelType::Shield)
		.addVariable("Projectile", ModelType::Projectile)
		.addVariable("Container", ModelType::Container)
		.addVariable("LiquidContainer", ModelType::LiquidContainer)
		.addVariable("Tool", ModelType::Tool)
		.addVariable("Node", ModelType::Node)
		.addVariable("Resource", ModelType::Resource)
		.addVariable("Seed", ModelType::Seed)
		.addVariable("Key", ModelType::Key)
		.addVariable("Furniture", ModelType::Furniture)
		.addVariable("Food", ModelType::Food)
		.addVariable("Light", ModelType::Light)
		.addVariable("Vehicle", ModelType::Vehicle)
		.addVariable("Book", ModelType::Book)
		.addVariable("Rope", ModelType::Rope)
		.addVariable("Mechanism", ModelType::Mechanism)
		.addVariable("Currency", ModelType::Currency)
		.addVariable("Shop", ModelType::Shop)
		.addVariable("Magazine", ModelType::Magazine)
		.endNamespace();
	// -------------------------------------------------------------------------
	// RESOURCE_TYPE
	luabridge::getGlobalNamespace(L)
		.deriveClass<ResourceType, BaseEnumerator>("ResourceType")
		.endClass()
		.beginNamespace("ResourceType")
		.addVariable("None", ResourceType::None)
		.addVariable("Coal", ResourceType::Coal)
		.addVariable("Ore", ResourceType::Ore)
		.addVariable("Bar", ResourceType::Bar)
		.addVariable("Log", ResourceType::Log)
		.addVariable("Plank", ResourceType::Plank)
		.addVariable("Tree", ResourceType::Tree)
		.addVariable("Fastener", ResourceType::Fastener)
		.addVariable("Leather", ResourceType::Leather)
		.addVariable("Cloth", ResourceType::Cloth)
		.addVariable("StoneBlock", ResourceType::StoneBlock)
		.addVariable("MetalVein", ResourceType::MetalVein)
		.addVariable("StoneMonolith", ResourceType::StoneMonolith)
		.addVariable("Pen", ResourceType::Pen)
		.addVariable("Trash", ResourceType::Trash)
		.addVariable("Meat", ResourceType::Meat)
		.addVariable("Bone", ResourceType::Bone)
		.addVariable("Skull", ResourceType::Skull)
		.endNamespace();
	// -------------------------------------------------------------------------
	// TOOL_TYPE
	luabridge::getGlobalNamespace(L)
		.deriveClass<ToolType, BaseEnumerator>("ToolType")
		.endClass()
		.beginNamespace("ToolType")
		.addVariable("None", ToolType::None)
		.addVariable("Pickaxe", ToolType::Pickaxe)
		.addVariable("WoodcutterAxe", ToolType::WoodcutterAxe)
		.addVariable("Saw", ToolType::Saw)
		.addVariable("PrecisionChisel", ToolType::PrecisionChisel)
		.addVariable("Hammer", ToolType::Hammer)
		.addVariable("PlaneChisel", ToolType::PlaneChisel)
		.addVariable("Forge", ToolType::Forge)
		.addVariable("Anvil", ToolType::Anvil)
		.addVariable("BlacksmithHammer", ToolType::BlacksmithHammer)
		.addVariable("Bellows", ToolType::Bellows)
		.addVariable("Crucible", ToolType::Crucible)
		.addVariable("Firelighter", ToolType::Firelighter)
		.endNamespace();
	// -------------------------------------------------------------------------
	// Load the script.
	auto path = Mud::instance().getMudSystemDirectory() + "lua/" + scriptFile;
	if (luaL_dofile(L, path.c_str()) != LUABRIDGE_LUA_OK) {
		MudLog(LogLevel::Error, "Can't open script %s.", scriptFile);
		MudLog(LogLevel::Error, "Error :%s", lua_tostring(L, -1));
		return false;
	}
	return true;
}