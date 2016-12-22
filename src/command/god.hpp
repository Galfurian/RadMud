/// @file   god.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2016
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

#include "argumentHandler.hpp"
#include "character.hpp"

/// Loads all the commands used by gods.
void LoadGodCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player
///         can execute.
/// @{

/// Shutdown the Mud.
bool DoShutdown(Character * character, ArgumentHandler & args);

/// Save the Mud.
bool DoMudSave(Character * character, ArgumentHandler & args);

/// Go to the desired room.
bool DoGoTo(Character * character, ArgumentHandler & args);

/// Get the path to the given room.
bool DoFindPath(Character * character, ArgumentHandler & args);

/// Transfer a character from room to room.
bool DoTransfer(Character * character, ArgumentHandler & args);

/// Regain completely health and stamina.
bool DoFeast(Character * character, ArgumentHandler & args);

/// Set a flag to a character.
bool DoSetFlag(Character * character, ArgumentHandler & args);
/// Remove a flag from the character.
bool DoClearFlag(Character * character, ArgumentHandler & args);

/// Generate a new item from the vnum of a model.
bool DoItemCreate(Character * character, ArgumentHandler & args);

/// Destroy an item.
bool DoItemDestroy(Character * character, ArgumentHandler & args);

/// Materialize an item from everywere.
bool DoItemGet(Character * character, ArgumentHandler & args);

/// Get information about an item.
bool DoItemInfo(Character * character, ArgumentHandler & args);

/// List all the items in the Mud.
bool DoItemList(Character * character, ArgumentHandler & args);

/// Create a room in the given direction.
bool DoRoomCreate(Character * character, ArgumentHandler & args);

/// Delete a room in the given direction.
bool DoRoomDelete(Character * character, ArgumentHandler & args);

/// Edit a room name or description.
bool DoRoomEdit(Character * character, ArgumentHandler & args);

/// Kill the desired mobile, in the same room.
bool DoMobileKill(Character * character, ArgumentHandler & args);

/// Reload the lua script for the target mobile, in the same room.
bool DoMobileReload(Character * character, ArgumentHandler & args);

/// Trigger the main behaviour of a mobile.
bool DoMobileTrigger(Character * character, ArgumentHandler & args);

/// Show the mobile's log.
bool DoMobileLog(Character * character, ArgumentHandler & args);

/// Hurt the desired target.
bool DoHurt(Character * character, ArgumentHandler & args);

/// Became invisible.
bool DoInvisibility(Character * character, ArgumentHandler & args);

/// Return visible.
bool DoVisible(Character * character, ArgumentHandler & args);

/// Modify the value of the player skill.
bool DoModSkill(Character * character, ArgumentHandler & args);

/// Modify the value of the player attribute.
bool DoModAttr(Character * character, ArgumentHandler & args);

/// Create the item with the given liquid inside a container.
bool DoLiquidCreate(Character * character, ArgumentHandler & args);


/// Get the list of buildings.
bool DoBuildingList(Character * character, ArgumentHandler & args);

/// Provides information about a building.
bool DoBuildingInfo(Character * character, ArgumentHandler & args);


/// Get information about a character.
bool DoGodInfo(Character * character, ArgumentHandler & args);

/// Show all the information fo the desired model.
bool DoModelInfo(Character * character, ArgumentHandler & args);

/// Show the informations about an area.
bool DoAreaInfo(Character * character, ArgumentHandler & args);

/// Show the informations about a room.
bool DoRoomInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given material.
bool DoMaterialInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given liquid.
bool DoLiquidInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given production.
bool DoProductionInfo(Character * character, ArgumentHandler & args);

/// Provide all the information regarding the given profession.
bool DoProfessionInfo(Character * character, ArgumentHandler & args);

/// Shows the infos about a faction.
bool DoFactionInfo(Character * character, ArgumentHandler & args);

/// Provides the list of opponents of the given target.
bool DoAggroList(Character * character, ArgumentHandler & args);

/// List all the model used to define items.
bool DoModelList(Character * character, ArgumentHandler & args);

/// List all the mobiles.
bool DoMobileList(Character * character, ArgumentHandler & args);

/// List all the players.
bool DoPlayerList(Character * character, ArgumentHandler & args);

/// List all the areas.
bool DoAreaList(Character * character, ArgumentHandler & args);

/// List all the rooms.
bool DoRoomList(Character * character, ArgumentHandler & args);

/// List all the races.
bool DoRaceList(Character * character, ArgumentHandler & args);

/// List all the factions.
bool DoFactionList(Character * character, ArgumentHandler & args);

/// List all the skills.
bool DoSkillList(Character * character, ArgumentHandler & args);

/// List all the writings.
bool DoWritingList(Character * character, ArgumentHandler & args);

/// List all the continents.
bool DoContinentList(Character * character, ArgumentHandler & args);

/// Get the list of materials.
bool DoMaterialList(Character * character, ArgumentHandler & args);

/// Get the list of all the professions.
bool DoProfessionList(Character * character, ArgumentHandler & args);

/// Get the list of all the productions.
bool DoProductionList(Character * character, ArgumentHandler & args);

/// Get the list of liquids.
bool DoLiquidList(Character * character, ArgumentHandler & args);

///@}
