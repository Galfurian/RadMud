/// @file   commandGod.cpp
/// @brief  Implements the methods used by <b>gods</b>.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "commandGod.hpp"

#include "mud.hpp"
#include "commandGodMud.hpp"
#include "commandGodItem.hpp"
#include "commandGodMobile.hpp"
#include "commandGodLiquid.hpp"
#include "commandGodCreation.hpp"
#include "commandGodCharacter.hpp"
#include "commandGodStructure.hpp"

void LoadGodCommands()
{
    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD MUD
    Mud::instance().addCommand(std::make_shared<Command>(
        DoShutdown, "mud_shutdown", "",
        "Shut the MUD down.",
        true, true, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMudSave, "mud_save", "",
        "Save the MUD.",
        true, true, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoGoTo, "mud_goto", "(room vnum)",
        "Go to another room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoGenerateMap, "mud_generate_map", "",
        "Generate a map.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoShowGenerateMap, "mud_show_generated_map", "",
        "Shows a generated map.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoDeleteGenerateMap, "mud_delete_generated_map", "",
        "Deletes a generated map.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBuildGenerateMap, "mud_build_generated_map", "",
        "Builds a generated map.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFactionInfo, "faction_information", "(faction vnum)",
        "Provide all the information regarding the given faction.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFactionList, "faction_list", "",
        "List all the factions.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRaceList, "race_list", "",
        "List all the races.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSkillList, "skill_list", "",
        "List all the skills.",
        true, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD CHARACTER
    Mud::instance().addCommand(std::make_shared<Command>(
        DoTransfer, "char_transfer", "(character) [where]",
        "Transfer another character here, or to another room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoHurt, "char_hurt", "(character)",
        "Hurt the desired character.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoGodInfo, "char_info", "(character)",
        "Get information about a character.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoAggroList, "char_aggro", "(character)",
        "Provides the list of opponents of the given target.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFeast, "char_feast", "(character)",
        "Restores completely the health and the stamina of the target.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoInvisible, "char_invisible", "[character]",
        "Became invisible or make someone invisible.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoVisible, "char_visible", "[character]",
        "Return visible or make someone visible.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPlayerList, "player_list", "",
        "List all the players.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPlayerSetFlag, "player_set_flag", "(player) (flag)",
        "Sets the given flag to the player.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPlayerClearFlag, "player_rem_flag", "(player) (flag)",
        "Remove the given flag from the player.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPlayerModSkill, "player_mod_skill", "(player) (skill) (modifier)",
        "Modify the value of the player skill.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPlayerModAttr, "player_mod_attribute",
        "(player) (attribute) (modifier)",
        "Modify the value of the player attribute.",
        true, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD STRUCTURE
    Mud::instance().addCommand(std::make_shared<Command>(
        DoFindPath, "find_path", "(room vnum)",
        "Finds the path to the given room.",
        true, true, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomCreate, "room_create", "(direction)",
        "Create a room in the given direction.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomDelete, "room_delete", "(direction)",
        "Delete the room in the given direction.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomEdit, "room_edit", "(option) (value)",
        "Change room values.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomInfo, "room_information", "(room vnum)",
        "Show the information about a specific room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRoomList, "room_list", "",
        "List all the rooms.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoAreaInfo, "area_information", "(area vnum)",
        "Show the information about a specific area.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoAreaList, "area_list", "",
        "List all the areas.",
        true, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD ITEM
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemCreate, "item_create", "(model vnum) (material vnum) [quality]",
        "Create a new item, if not set the quality will be Normal.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemDestroy, "item_destroy", "(item vnum)",
        "Destroy the desired object.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemGet, "item_get", "(item vnum)",
        "Materialize the desired object.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemInfo, "item_information", "(item vnum)",
        "Show information about an item.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoItemList, "item_list", "Type --help for more information.",
        "List all the items.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoModelInfo, "model_info", "(model vnum)",
        "List all the information about a model.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoModelList, "model_list", "Type --help for more information.",
        "List all the models.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoWritingList, "writing_list", "",
        "List all the writings.",
        true, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD MOBILE
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileKill, "mob_kill", "(mobile name)",
        "Kill the desired mobile, in the same room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileList, "mob_list", "(mobile name)",
        "List all the mobiles.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileReload, "mob_reload", "(mobile name)",
        "Reload the lua script for the target mobile, in the same room.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileTrigger, "mob_trigger", "(mobile name) (event name)",
        "Trigger a specific event of the given mobile.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMobileLog, "mob_log", "(mobile name)",
        "Given a mobile id, it returns the corresponding mobile log.",
        true, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD CREATION
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMaterialInfo, "mat_information", "(material name)",
        "Show the information about a specific material.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoMaterialList, "mat_list", "",
        "List all the materials.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBuildingInfo, "building_information", "(building vnum)",
        "Provides information about a building.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBuildingList, "building_list", "",
        "List all the buildings.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProfessionInfo, "profession_information", "(profession command)",
        "Provide all the information regarding the given profession.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProfessionList, "profession_list", "",
        "Get the list of all the professions.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProductionInfo, "production_information", "(production vnum)",
        "Provide all the information regarding the given production.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoProductionList, "production_list", "",
        "Get the list of all the productions.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBodyPartList, "body_part_list", "",
        "Get the list of all the body parts.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoBodyPartInfo, "body_part_information", "",
        "Get information about a body part.",
        true, true, false));

    // ////////////////////////////////////////////////////////////////////////
    // COMMAND GOD LIQUID
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLiquidInfo, "liquid_information", "(liquid name)",
        "Show the information about a specific liquid.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLiquidList, "liquid_list", "",
        "List all the liquids.",
        true, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLiquidCreate, "liquid_create", "(container)(liquid vnum) (quantity)",
        "Materialize some liquid sinde a container.",
        true, true, false));
}
