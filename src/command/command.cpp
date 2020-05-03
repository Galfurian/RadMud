/// @file   command.cpp
/// @brief  Implement the general methods needed to handle commands.
/// @author Enrico Fraccaroli
/// @date   Sep 8 2015
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
#include "command/command.hpp"
#include "command/combat.hpp"
#include "command/communication.hpp"
#include "command/crafting.hpp"
#include "command/general.hpp"
#include "command/manager.hpp"
#include "command/movement.hpp"
#include "command/god/commandGodMud.hpp"
#include "command/god/commandGodItem.hpp"
#include "command/god/commandGodMobile.hpp"
#include "command/god/commandGodLiquid.hpp"
#include "command/god/commandGodCreation.hpp"
#include "command/god/commandGodCharacter.hpp"
#include "command/god/commandGodStructure.hpp"
#include "command/object/commandObjectLightSource.hpp"
#include "command/object/commandObjectManagement.hpp"
#include "command/object/commandObjectContainer.hpp"
#include "command/object/commandObjectCharacter.hpp"
#include "command/object/commandObjectProcess.hpp"
#include "command/object/commandObjectLiquids.hpp"
#include "command/object/commandObjectShop.hpp"
#include "command/object/commandObjectFood.hpp"
#include "mud.hpp"

Command::Command() :
	handler(), name(), arguments(), help(), gods(), canUseInCombat(), typedCompletely()
{
	// Nothing to do.
}

Command::Command(std::function<bool(Character *character, ArgumentHandler &args)> _handler,
				 std::string _name, std::string _arguments, std::string _help, bool _gods,
				 bool _canUseInCombat, bool _typedCompletely) :
	handler(std::move(_handler)),
	name(std::move(_name)),
	arguments(std::move(_arguments)),
	help(std::move(_help)),
	gods(_gods),
	canUseInCombat(_canUseInCombat),
	typedCompletely(_typedCompletely)
{
	// Nothing to do.
}

bool Command::canUse(Character *character) const
{
	return (gods && HasFlag(character->flags, CharacterFlag::IsGod)) || (!gods);
}

void StopAction(Character *character)
{
	if ((character->getAction() != ActionType::Wait)) {
		if (character->getAction() != ActionType::Combat) {
			character->doCommand("stop");
		}
	}
}

bool Mud::initCommands()
{
	// Beware the order is important.
	// Changing the order will not compromise the correct
	//  execution of the Mud, but...
	// If a player just types 'l' or 'lo' in order to 'look'
	//  the first command which get hit is the 'load' command.

	// == MOVEMENT ============================================================

	this->addCommand(DoTravel, "travel", "", "Allow the character to travel between areas.", false,
					 true, false);
	this->addCommand(DoStop, "stop", "", "Stop the current character action.", false, true, false);
	this->addCommand(DoStand, "stand", "", "Stand up.", false, true, false);
	this->addCommand(DoCrouch, "crouch", "",
					 "The character crouches down himself, it's a good stance for hiding.", false,
					 true, false);
	this->addCommand(DoSit, "sit", "", "The player sits down, ideal for a quick break.", false,
					 false, false);
	this->addCommand(DoProne, "prone", "",
					 "The player starts prone, a perfect position to shoot long distance.", false,
					 true, false);
	this->addCommand(DoRest, "rest", "", "The player lies down and begin to rest.", false, false,
					 false);
	this->addCommand(DoSleep, "sleep", "", "The player lies down, closes its eyes and sleep.",
					 false, false, false);
	this->addCommand(DoWake, "wake", "", "The player wake up.", false, false, false);

	// == GENERAL =============================================================
	this->addCommand(DoQuit, "quit", "", "Leave the game.", false, false, true);
	this->addCommand(DoWho, "who", "", "List all the character online.", false, false, false);
	this->addCommand(DoSet, "set", "(setting) (value)", "Set some character texts(eg. descr).",
					 false, false, false);
	this->addCommand(DoLook, "look", "[(something) or (someone)]", "Look at something or someone.",
					 false, true, false);
	this->addCommand(DoHelp, "help", "(command)",
					 "Show the list of commands or show help for a given command.", false, true,
					 false);
	this->addCommand(DoPrompt, "prompt", "(help)|(prompt definition)", "Modify your prompt.", false,
					 false, false);
	this->addCommand(DoTime, "time", "", "Give the current day phase.", false, true, false);
	this->addCommand(DoStatistics, "statistics", "", "Show player statistics.", false, true, false);
	this->addCommand(DoEffects, "effects", "", "Show player active effects.", false, true, false);
	this->addCommand(DoRent, "rent", "", "Allow player to rent and disconnect.", false, false,
					 false);
	this->addCommand(DoSkills, "skills", "", "Shows the playes skills and their level.", false,
					 true, false);
	this->addCommand(DoActions, "actions", "", "Shows the list of actions.", false, true, false);
	this->addCommand(DoServer, "server", "", "Shows the server statistics.", false, true, false);
	this->addCommand(DoGenerateName, "generate_name", "", "Generates a random name.", false, true,
					 false);

	// == OBJECT ==============================================================

	this->addCommand(DoTake, "take", "(item|all) [(container)]",
					 "Take something from the ground or from a container.", false, true, false);
	this->addCommand(DoDrop, "drop", "(item|all)", "Drop an object.", false, true, false);
	this->addCommand(DoPut, "put", "(something) (container)", "Put something inside a container.",
					 false, true, false);
	this->addCommand(DoGive, "give", "(item|all) (someone)", "Give an object to someone.", false,
					 false, false);
	this->addCommand(DoDrink, "drink", "(liquid container)", "Drink from a container of liquids.",
					 false, false, false);
	this->addCommand(DoFill, "fill", "(liquid container) (source of liquid)",
					 "Fill a container of liquids from a source of liquid.", false, false, false);
	this->addCommand(DoPour, "pour", "(liquid container) [liquid container]",
					 "Pour the content of the container into another one or on the ground.", false,
					 false, false);
	this->addCommand(DoEat, "eat", "(food)", "Eats the provided food.", false, false, false);
	this->addCommand(DoOrganize, "organize", "(name|weight|price) [(container|room|inventory)]",
					 "Order the desired container or if no target is passed, the room.", false,
					 false, false);
	this->addCommand(DoOpen, "open", "(container)|(direction)",
					 "Open a door in a given direction or a container.", false, true, false);
	this->addCommand(DoClose, "close", "(container)|(direction)",
					 "Close a door in a given direction or a container.", false, true, false);
	this->addCommand(DoEquipments, "equipments", "", "List all the items you are wearing.", false,
					 true, false);
	this->addCommand(DoWield, "wield", "(item)", "Wield a weapon, a shield or maybe a tool.", false,
					 true, false);
	this->addCommand(DoWear, "wear", "(item)", "Puts on a piece of equipment.", false, false,
					 false);
	this->addCommand(DoRemove, "remove", "(item)", "Remove a worn or wielded item.", false, true,
					 false);
	this->addCommand(DoInventory, "inventory", "", "Show character's inventory.", false, true,
					 false);
	this->addCommand(DoDeposit, "deposit", "(coin) (shop)", "Deposit a coin inside a shop.", false,
					 false, false);
	this->addCommand(DoSell, "sell", "(item) (shop)", "Sell an item to a shop keeper.", false,
					 false, false);
	this->addCommand(DoBuy, "buy", "(item) (shop)", "Allows to buy an item from a shop.", false,
					 false, false);
	this->addCommand(DoBalance, "balance", "", "Shows the character's balance.", false, false,
					 false);
	this->addCommand(DoTurn, "turn", "(item)", "Allows to turn on and off an actionable item.",
					 false, true, false);
	this->addCommand(DoKindle, "kindle", "(item) (ignition source)", "Allows to kindle a fire.",
					 false, true, false);
	this->addCommand(DoRefill, "refill", "(light source) (fuel)",
					 "Allows to refill a light source.", false, true, false);
	this->addCommand(DoDismember, "dismember", "(corpse)", "Allows to dismember a corpse.", false,
					 true, false);

	// == COMMUNICATION =======================================================

	this->addCommand(DoSay, "say", "[someone] (something)", "Talk to people in the current room.",
					 false, true, false);
	this->addCommand(DoWhisper, "whisper", "(someone) (something)",
					 "Whisper secretly to a single character.", false, true, false);
	this->addCommand(DoEmote, "emote", "(emotion)", "Express an emotion.", false, true, false);
	this->addCommand(DoBug, "bug", "(message)",
					 "Report a bug, your character's name, location and date will be saved.", false,
					 true, false);
	this->addCommand(DoIdea, "idea", "(message)", "Send an idea, try to be as clear as possible.",
					 false, true, false);
	this->addCommand(DoTypo, "typo", "(message)", "Report a typo.", false, true, false);

	// == CRAFTING ============================================================
	this->addCommand(DoBuild, "build", "(item)", "Build something.", false, false, false);
	this->addCommand(DoDeconstruct, "deconstruct", "(building)", "Deconstruct a building.", false,
					 false, false);
	this->addCommand(DoRead, "read", "(item)", "Read an inscription from an item.", false, false,
					 false);

	// == COMBAT ==============================================================

	this->addCommand(DoKill, "kill", "(target)", "Engage in combat the desired target.", false,
					 true, false);
	this->addCommand(DoFlee, "flee", "", "Try to flee from combat.", false, true, false);
	this->addCommand(DoScout, "scout", "", "Provides information about the surrounding area.",
					 false, true, false);
	this->addCommand(DoLoad, "load", "(magazine) (projectiles)",
					 "Allows to load a magazine with projectiles.", false, true, false);
	this->addCommand(DoUnload, "unload", "(magazine)", "Allows to unload an magazine.", false, true,
					 false);
	this->addCommand(DoReload, "reload", "(firearm) (magazine)",
					 "Allows to reload a firearm with a magazine.", false, true, false);
	this->addCommand(DoAim, "aim", "(target)",
					 "Allows to aim a target in sight.\n"
					 "If the target is not inside the same room,"
					 "you have first to scout the area.\n"
					 "If no arguments is provided it lists the available targets.",
					 false, true, false);
	this->addCommand(DoFire, "fire", "(firearm) (magazine)",
					 "Allows to fire with an equipped ranged weapon to an aimed target.", false,
					 true, false);

	// == MANAGER =============================================================

	this->addCommand(DoAssign, "assign", "(mobile) (building)",
					 "Allows to assign a mobile to a task/building.", false, false, false);

	// == GOD =================================================================

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD MUD
	this->addCommand(DoShutdown, "@shutdown", "", "Shut the MUD down.", true, true, true);
	this->addCommand(DoMudSave, "@save", "", "Save the MUD.", true, true, true);
	this->addCommand(DoGoTo, "@goto", "(room vnum)", "Go to another room.", true, true, false);
	this->addCommand(DoFactionInfo, "@faction_info", "(faction vnum)",
					 "Provide all the information regarding the given faction.", true, true, false);
	this->addCommand(DoFactionList, "@faction_list", "", "List all the factions.", true, true,
					 false);
	this->addCommand(DoRaceInfo, "@race_info", "",
					 "Provide all the information regarding the given race.", true, true, false);
	this->addCommand(DoRaceList, "@race_list", "", "List all the races.", true, true, false);
	this->addCommand(DoSkillList, "@skill_list", "", "List all the skills.", true, true, false);

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD CHARACTER
	this->addCommand(DoTransfer, "@transfer", "(character) [where]",
					 "Transfer another character here, or to another room.", true, true, false);
	this->addCommand(DoHurt, "@hurt", "(character)", "Hurt the desired character.", true, true,
					 false);
	this->addCommand(DoGodInfo, "@char_info", "(character)", "Get information about a character.",
					 true, true, false);
	this->addCommand(DoAggroList, "@char_aggro", "(character)",
					 "Provides the list of opponents of the given target.", true, true, false);
	this->addCommand(DoFeast, "@char_feast", "(character)",
					 "Restores completely the health and the stamina of the target.", true, true,
					 false);
	this->addCommand(DoInvisible, "@char_invisible", "[character]",
					 "Became invisible or make someone invisible.", true, true, false);
	this->addCommand(DoVisible, "@char_visible", "[character]",
					 "Return visible or make someone visible.", true, true, false);
	this->addCommand(DoPlayerList, "@player_list", "", "List all the players.", true, true, false);
	this->addCommand(DoPlayerSetFlag, "@player_set_flag", "(player) (flag)",
					 "Sets the given flag to the player.", true, true, false);
	this->addCommand(DoPlayerClearFlag, "@player_rem_flag", "(player) (flag)",
					 "Remove the given flag from the player.", true, true, false);
	this->addCommand(DoPlayerModSkill, "@player_mod_skill", "(player) (skill) (modifier)",
					 "Modify the value of the player skill.", true, true, false);
	this->addCommand(DoPlayerModAttr, "@player_mod_attribute", "(player) (attribute) (modifier)",
					 "Modify the value of the player attribute.", true, true, false);

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD STRUCTURE
	this->addCommand(DoFindPath, "@find_path", "(room vnum)", "Finds the path to the given room.",
					 true, true, true);
	this->addCommand(DoRoomCreate, "@room_create", "(direction)",
					 "Create a room in the given direction.", true, true, false);
	this->addCommand(DoRoomDelete, "@room_delete", "(direction)",
					 "Delete the room in the given direction.", true, true, false);
	this->addCommand(DoRoomEdit, "@room_edit", "(option) (value)", "Change room values.", true,
					 true, false);
	this->addCommand(DoRoomInfo, "@room_info", "(room vnum)",
					 "Show the information about a specific room.", true, true, false);
	this->addCommand(DoRoomList, "@room_list", "", "List all the rooms.", true, true, false);
	this->addCommand(DoAreaInfo, "@area_info", "(area vnum)",
					 "Show the information about a specific area.", true, true, false);
	this->addCommand(DoAreaList, "@area_list", "", "List all the areas.", true, true, false);
	this->addCommand(DoAreaMap, "@area_map", "(area vnum)(level)",
					 "Shows the map of the level of an area.", true, true, false);

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD ITEM
	this->addCommand(DoItemCreate, "@item_create", "(model vnum) (material vnum) [quality]",
					 "Create a new item, if not set the quality will be Normal.", true, true,
					 false);
	this->addCommand(DoItemDestroy, "@item_destroy", "(item vnum)", "Destroy the desired object.",
					 true, true, false);
	this->addCommand(DoItemGet, "@item_get", "(item vnum)", "Materialize the desired object.", true,
					 true, false);
	this->addCommand(DoItemInfo, "@item_info", "(item vnum)", "Show information about an item.",
					 true, true, false);
	this->addCommand(DoItemList, "@item_list", "Type --help for more information.",
					 "List all the items.", true, true, false);
	this->addCommand(DoModelInfo, "@model_info", "(model vnum)",
					 "List all the information about a model.", true, true, false);
	this->addCommand(DoModelList, "@model_list", "Type --help for more information.",
					 "List all the models.", true, true, false);
	this->addCommand(DoWritingList, "@writing_list", "", "List all the writings.", true, true,
					 false);

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD MOBILE
	this->addCommand(DoMobileModelList, "@mob_model_list", "", "List all the mobiles.", true, true,
					 false);
	this->addCommand(DoMobileSpawn, "@mob_spawn", "", "Spawn the given mobile in the current room.",
					 true, true, false);
	this->addCommand(DoMobileSave, "@mob_spawn_save", "",
					 "Save the spawn point for the given mobile.", true, true, false);
	this->addCommand(DoMobileKill, "@mob_kill", "(mobile name)",
					 "Kill the desired mobile, in the same room.", true, true, false);
	this->addCommand(DoMobileList, "@mob_list", "(mobile name)", "List all the mobiles.", true,
					 true, false);
	this->addCommand(DoMobileReload, "@mob_reload", "(mobile name)",
					 "Reload the lua script for the target mobile, in the same room.", true, true,
					 false);
	this->addCommand(DoMobileTrigger, "@mob_trigger", "(mobile name) (event name)",
					 "Trigger a specific event of the given mobile.", true, true, false);
	this->addCommand(DoMobileLog, "@mob_log", "(mobile name)",
					 "Given a mobile id, it returns the corresponding mobile log.", true, true,
					 false);

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD CREATION
	this->addCommand(DoMaterialInfo, "@material_info", "(material name)",
					 "Show the information about a specific material.", true, true, false);
	this->addCommand(DoMaterialList, "@material_list", "", "List all the materials.", true, true,
					 false);
	this->addCommand(DoBuildingInfo, "@building_info", "(building vnum)",
					 "Provides information about a building.", true, true, false);
	this->addCommand(DoBuildingList, "@building_list", "", "List all the buildings.", true, true,
					 false);
	this->addCommand(DoProfessionInfo, "@profession_info", "(profession command)",
					 "Provide all the information regarding the given profession.", true, true,
					 false);
	this->addCommand(DoProfessionList, "@profession_list", "",
					 "Get the list of all the professions.", true, true, false);
	this->addCommand(DoProductionInfo, "@production_info", "(production vnum)",
					 "Provide all the information regarding the given production.", true, true,
					 false);
	this->addCommand(DoProductionList, "@production_list", "",
					 "Get the list of all the productions.", true, true, false);
	this->addCommand(DoBodyPartList, "@body_part_list", "", "Get the list of all the body parts.",
					 true, true, false);
	this->addCommand(DoBodyPartInfo, "@body_part_info", "", "Get information about a body part.",
					 true, true, false);

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND GOD LIQUID
	this->addCommand(DoLiquidInfo, "@liquid_info", "(liquid name)",
					 "Show the information about a specific liquid.", true, true, false);
	this->addCommand(DoLiquidList, "@liquid_list", "", "List all the liquids.", true, true, false);
	this->addCommand(DoLiquidCreate, "@liquid_create", "(container)(liquid vnum) (quantity)",
					 "Materialize some liquid sinde a container.", true, true, false);
	return true;
}

void LoadCommands()
{
}
