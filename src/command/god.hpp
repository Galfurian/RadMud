/// @file   god.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "argumentHandler.hpp"
#include "character.hpp"

/// Loads all the commands used by gods.
void LoadGodCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// Shutdown the Mud.
void DoShutdown(Character * character, ArgumentHandler & args);

/// Save the Mud.
void DoMudSave(Character * character, ArgumentHandler & args);

/// Go to the desired room.
void DoGoTo(Character * character, ArgumentHandler & args);

/// Get the path to the given room.
void DoFindPath(Character * character, ArgumentHandler & args);

/// Transfer a character from room to room.
void DoTransfer(Character * character, ArgumentHandler & args);

/// Regain completely health and stamina.
void DoFeast(Character * character, ArgumentHandler & args);

/// Set a flag to a character.
void DoSetFlag(Character * character, ArgumentHandler & args);

/// Remove a flag from the character.
void DoClearFlag(Character * character, ArgumentHandler & args);

/// Generate a new item from the vnum of a model.
void DoItemCreate(Character * character, ArgumentHandler & args);

/// Materialize an item from everywere.
void DoItemGet(Character * character, ArgumentHandler & args);

/// Destroy an item.
void DoItemDestroy(Character * character, ArgumentHandler & args);

/// Create a room in the given direction.
void DoRoomCreate(Character * character, ArgumentHandler & args);

/// Delete a room in the given direction.
void DoRoomDelete(Character * character, ArgumentHandler & args);

/// Edit a room name or description.
void DoRoomEdit(Character * character, ArgumentHandler & args);

/// Kill the desired mobile, in the same room.
void DoMobileKill(Character * character, ArgumentHandler & args);

/// Reload the lua script for the target mobile, in the same room.
void DoMobileReload(Character * character, ArgumentHandler & args);

/// Show the mobile's log.
void DoMobileLog(Character * character, ArgumentHandler & args);

/// Hurt the desired target.
void DoHurt(Character * character, ArgumentHandler & args);

/// Became invisible.
void DoInvisibility(Character * character, ArgumentHandler & args);

/// Return visible.
void DoVisible(Character * character, ArgumentHandler & args);

/// Modify the value of the player skill.
void DoModSkill(Character * character, ArgumentHandler & args);

/// Modify the value of the player attribute.
void DoModAttr(Character * character, ArgumentHandler & args);

/// Create the item with the given liquid inside a container.
void DoLiquidCreate(Character * character, ArgumentHandler & args);

/// Get information about a character.
void DoGodInfo(Character * character, ArgumentHandler & args);

/// Show all the information fo the desired model.
void DoModelInfo(Character * character, ArgumentHandler & args);

/// Get information about an item.
void DoItemInfo(Character * character, ArgumentHandler & args);

/// Show the informations about an area.
void DoAreaInfo(Character * character, ArgumentHandler & args);

/// Show the informations about a room.
void DoRoomInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given material.
void DoMaterialInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given liquid.
void DoLiquidInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given production.
void DoProductionInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given profession.
void DoProfessionInfo(Character * character, ArgumentHandler & args);

/// Shows the infos about a faction.
void DoFactionInfo(Character * character, ArgumentHandler & args);

/// Provides the list of opponents of the given target.
void DoAggroList(Character * character, ArgumentHandler & args);

/// List all the model used to define items.
void DoModelList(Character * character, ArgumentHandler & args);

/// List all the items in the Mud.
void DoItemList(Character * character, ArgumentHandler & args);

/// List all the mobiles.
void DoMobileList(Character * character, ArgumentHandler & args);

/// List all the players.
void DoPlayerList(Character * character, ArgumentHandler & args);

/// List all the areas.
void DoAreaList(Character * character, ArgumentHandler & args);

/// List all the rooms.
void DoRoomList(Character * character, ArgumentHandler & args);

/// List all the races.
void DoRaceList(Character * character, ArgumentHandler & args);

/// List all the factions.
void DoFactionList(Character * character, ArgumentHandler & args);

/// List all the skills.
void DoSkillList(Character * character, ArgumentHandler & args);

/// List all the writings.
void DoWritingList(Character * character, ArgumentHandler & args);

/// List all the corpses
void DoCorpseList(Character * character, ArgumentHandler & args);

/// List all the continents.
void DoContinentList(Character * character, ArgumentHandler & args);

/// Get the list of materials.
void DoMaterialList(Character * character, ArgumentHandler & args);

/// Get the list of all the professions.
void DoProfessionList(Character * character, ArgumentHandler & args);

/// Get the list of all the productions.
void DoProductionList(Character * character, ArgumentHandler & args);

/// Get the list of liquids.
void DoLiquidList(Character * character, ArgumentHandler & args);

/// Get the list of buildings.
void DoBuildingList(Character * character, ArgumentHandler & args);

///@}
