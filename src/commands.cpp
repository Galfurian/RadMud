/// @file   commands.cpp
/// @brief  Implement the general methods needed to handle commands.
/// @author Enrico Fraccaroli
/// @date   Sep 8 2015
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
#include "commands.hpp"

#include <istream>

// Other Include.
#include "mud.hpp"

void ProcessCommand(Character * character, std::istream &sArgs)
{
    std::string command;

    // Get command, eat whitespace after it.
    sArgs >> command >> std::ws;

    if (command.empty())
    {
        throw std::runtime_error("Huh?\n");
    }
    // Check if it's a direction.
    Direction direction = Mud::getInstance().findDirection(command, false);

    if (direction != Direction::None)
    {
        DoDirection(character, direction);
    }
    else
    {
        bool found = false;
        // Check if it's a command.
        for (auto iterator : Mud::getInstance().mudCommands)
        {
            if (!BeginWith(iterator.name, command))
            {
                continue;
            }
            if (iterator.level == 1 && !HasFlag(character->flags, CharacterFlag::IsGod))
            {
                continue;
            }
            if (iterator.name == "shutdown" && command != "shutdown")
            {
                character->sendMsg("You have to type completly \"shutdown\".\n");
            }
            else if (iterator.name == "quit" && command != "quit")
            {
                character->sendMsg("You have to type completly \"quit\".\n");
            }
            else
            {
                iterator.hndl(character, sArgs);
                found = true;
                break;
            }
        }
        if (!found)
        {
            Profession * profession = Mud::getInstance().findProfession(command);

            if (profession != nullptr)
            {
                DoProfession(character, profession, sArgs);
            }
            else
            {
                throw std::runtime_error("Huh?\n");
            }
        }
    }
    character->sendMsg("\n");
}

void LoadCommands()
{
    Command command;
    command.level = 0;
    {
        command.name = "save";
        command.help = "Save character into the database.";
        command.args = "";
        command.hndl = DoSave;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "quit";
        command.help = "Leave the game.";
        command.args = "";
        command.hndl = DoQuit;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "who";
        command.help = "List all the character online.";
        command.args = "";
        command.hndl = DoWho;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "set";
        command.help = "Set some character texts(eg. descr).";
        command.args = "(setting) (value)";
        command.hndl = DoSet;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "stop";
        command.help = "Stop the current character action.";
        command.args = "";
        command.hndl = DoStop;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "look";
        command.help = "Look at something or someone.";
        command.args = "[(something) or (someone)]";
        command.hndl = DoLook;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "help";
        command.help = "Show the list of commands or show help for a given command.";
        command.args = "(command)";
        command.hndl = DoHelp;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "prompt";
        command.help = "Modify your prompt.";
        command.args = "(help)|(prompt definition)";
        command.hndl = DoPrompt;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "time";
        command.help = "Give the current day phase.";
        command.args = "";
        command.hndl = DoTime;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "stand";
        command.help = "Make the player stand.";
        command.args = "";
        command.hndl = DoStand;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "crouch";
        command.help = "The player crouches down himself, it's a good stance for hiding.";
        command.args = "";
        command.hndl = DoCrouch;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "sit";
        command.help = "The player sits down, ideal for a quick break.";
        command.args = "";
        command.hndl = DoSit;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "prone";
        command.help = "The player starts prone, a perfect position to shoot long distance.";
        command.args = "";
        command.hndl = DoProne;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "rest";
        command.help = "The player lies down and begin to rest.";
        command.args = "";
        command.hndl = DoRest;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "statistics";
        command.help = "Show player statistics.";
        command.args = "";
        command.hndl = DoStatistics;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "rent";
        command.help = "Allow player to rent and disconnect.";
        command.args = "";
        command.hndl = DoRent;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "skills";
        command.help = "Shows the playes abilities and their level.";
        command.args = "";
        command.hndl = DoSkills;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "server";
        command.help = "Shows the server statistics.";
        command.args = "";
        command.hndl = DoServer;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "travel";
        command.help = "Allow the character to travel.";
        command.args = "";
        command.hndl = DoTravel;
        Mud::getInstance().addCommand(command);
    }

    // Inventory and Equipment.
    {
        command.name = "take";
        command.help = "Take something from the ground or from a container.";
        command.args = "(item) [(container)]";
        command.hndl = DoTake;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "drop";
        command.help = "Drop an object.";
        command.args = "(item)";
        command.hndl = DoDrop;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "give";
        command.help = "Give an object to someone.";
        command.args = "(item) (someone)";
        command.hndl = DoGive;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "equipments";
        command.help = "List all the items you are wearing.";
        command.args = "";
        command.hndl = DoEquipments;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "wield";
        command.help = "Wield a weapon, a shield or maybe a tool.";
        command.args = "(item)";
        command.hndl = DoWield;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "wear";
        command.help = "Puts on a piece of equipment.";
        command.args = "(item)";
        command.hndl = DoWear;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "remove";
        command.help = "Remove a weared or wielded item.";
        command.args = "(item)";
        command.hndl = DoRemove;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "inventory";
        command.help = "Show character's inventory.";
        command.args = "";
        command.hndl = DoInventory;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "organize";
        command.help = "Order the desired container or if no target is passed, the room.";
        command.args = "(name|weight) [(target)]";
        command.hndl = DoOrganize;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "open";
        command.help = "Open a door in a given direction.";
        command.args = "(item)|(direction)";
        command.hndl = DoOpen;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "close";
        command.help = "Close a door in a given direction.";
        command.args = "(item)|(direction)";
        command.hndl = DoClose;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "put";
        command.help = "Put something inside a container.";
        command.args = "(something) (container)";
        command.hndl = DoPut;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "drink";
        command.help = "Drink from a container of liquids.";
        command.args = "(container)";
        command.hndl = DoDrink;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "fill";
        command.help = "Fill a container of liquids from a source of liquid.";
        command.args = "(container) (source)";
        command.hndl = DoFill;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "pour";
        command.help = "Pour the content of the container into another one or on the ground.";
        command.args = "(container) [container]";
        command.hndl = DoPour;
        Mud::getInstance().addCommand(command);
    }

    // Comunication.
    {
        command.name = "say";
        command.help = "Talk to people in the current room.";
        command.args = "[someone] (something)";
        command.hndl = DoSay;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "whisper";
        command.help = "Whisper secretly to a single character.";
        command.args = "(someone) (something)";
        command.hndl = DoWhisper;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "emote";
        command.help = "Execute and emote.";
        command.args = "(emotion)";
        command.hndl = DoEmote;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "bug";
        command.help = "Report a bug, your character's name, location and date will be saved.";
        command.args = "(message)";
        command.hndl = DoBug;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "idea";
        command.help = "Send an idea, try to be as clear as possible.";
        command.args = "(message)";
        command.hndl = DoIdea;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "typo";
        command.help = "Report a typo.";
        command.args = "(message)";
        command.hndl = DoTypo;
        Mud::getInstance().addCommand(command);
    }

    // Gathering.
    //{
    //command.name = "mine";
    //command.help = "Gather some metal from a node.";
    //command.args = "(node)";
    //command.hndl = DoMine;
    //Mud::getInstance().addCommand(command);
    //}
    //{
    //command.name = "chop";
    //command.help = "Gather some wood branches.";
    //command.args = "(node)";
    //command.hndl = DoChop;
    //Mud::getInstance().addCommand(command);
    //}

    // Crafting.
    {
        command.name = "build";
        command.help = "Build something.";
        command.args = "(item)";
        command.hndl = DoBuild;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "deconstruct";
        command.help = "Deconstruct a building.";
        command.args = "(building)";
        command.hndl = DoDeconstruct;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "read";
        command.help = "Read an inscription from an item.";
        command.args = "(item)";
        command.hndl = DoRead;
        Mud::getInstance().addCommand(command);
    }

    // Combat.
    {
        command.name = "kill";
        command.help = "Engage in combat the desired target.";
        command.args = "(target)";
        command.hndl = DoKill;
        Mud::getInstance().addCommand(command);
    }

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // GODS COMMANDS
    // ////////////////////////////////////////////////////////////////////////////////////////////

    {
        command.name = "shutdown";
        command.help = "Shut the MUD down.";
        command.args = "";
        command.hndl = DoShutdown;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "goto";
        command.help = "Go to another room.";
        command.args = "(Room.vnum)";
        command.hndl = DoGoTo;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "hurt";
        command.help = "Hurt the desired target.";
        command.args = "(Target)";
        command.hndl = DoHurt;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "invisibility";
        command.help = "Became invisible.";
        command.args = "";
        command.hndl = DoInvisibility;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "visible";
        command.help = "Return visible.";
        command.args = "";
        command.hndl = DoVisible;
        Mud::getInstance().addCommand(command);
    }

    // Interaction with Players.
    {
        command.name = "pinfo";
        command.help = "Return all the information concerning a player.";
        command.args = "(Target)";
        command.hndl = DoPlayerInfo;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "transfer";
        command.help = "Transfer another character here, or to another room.";
        command.args = "(Target) [Where]";
        command.hndl = DoTransfer;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "sflag";
        command.help = "Sets a flag for a character.";
        command.args = "(Target) (Flag)";
        command.hndl = DoSetFlag;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "cflag";
        command.help = "Remove a flag from a character.";
        command.args = "(Target) (Flag)";
        command.hndl = DoClearFlag;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "modskill";
        command.help = "Modify the value of the player skill.";
        command.args = "(Target) (Skill) (Value)";
        command.hndl = DoModSkill;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "modattribute";
        command.help = "Modify the value of the player attribute.";
        command.args = "(Target) (Attribute) (Value)";
        command.hndl = DoModAttr;
        Mud::getInstance().addCommand(command);
    }

    // Item's Model.
    {
        command.name = "imodel";
        command.help = "List all the information about a model.";
        command.args = "(Model.vnum)";
        command.hndl = DoModelInfo;
        Mud::getInstance().addCommand(command);
    }

    // Items.
    {
        command.name = "iitem";
        command.help = "Show information about an item.";
        command.args = "(Item.vnum)";
        command.hndl = DoItemInfo;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "citem";
        command.help = "Create a new item, if not set the quality will be Normal.";
        command.args = "(Model.vnum)(Material.vnum)[Quality]";
        command.hndl = DoItemCreate;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "ditem";
        command.help = "Destroy the desired object.";
        command.args = "(Item.vnum)";
        command.hndl = DoItemDestroy;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "gitem";
        command.help = "Materialize the desired object.";
        command.args = "(Item.vnum)";
        command.hndl = DoItemGet;
        Mud::getInstance().addCommand(command);
    }

    // Areas.
    {
        command.name = "iarea";
        command.help = "Show the informations about a specific area.";
        command.args = "(Area.vnum)";
        command.hndl = DoAreaInfo;
        Mud::getInstance().addCommand(command);
    }

    // Rooms.
    {
        command.name = "iroom";
        command.help = "Show the informations about a specific room.";
        command.args = "(Room.vnum)";
        command.hndl = DoRoomInfo;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "croom";
        command.help = "Create a room in the given direction.";
        command.args = "(Direction)";
        command.hndl = DoRoomCreate;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "droom";
        command.help = "Delete the room in the given direction.";
        command.args = "(Direction)";
        command.hndl = DoRoomDelete;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "eroom";
        command.help = "Change room values.";
        command.args = "(Option) (Value)";
        command.hndl = DoRoomEdit;
        Mud::getInstance().addCommand(command);
    }

    // Mobiles.
    {
        command.name = "imobile";
        command.help = "Get information about a mobile.";
        command.args = "(Mobile.id)";
        command.hndl = DoMobileInfo;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "kmobile";
        command.help = "Kill the desired mobile, in the same room.";
        command.args = "(Mobile.name)";
        command.hndl = DoMobileKill;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "rmobile";
        command.help = "Reload the lua script for the target mobile, in the same room.";
        command.args = "(Mobile.name)";
        command.hndl = DoMobileReload;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "mobilelog";
        command.help = "Given a mobile id, it returns the curresponding mobile log.";
        command.args = "(Mobile.id)";
        command.hndl = DoMobileLog;
        Mud::getInstance().addCommand(command);
    }

    // Materials.
    {
        command.name = "imaterial";
        command.help = "Show the informations about a specific material.";
        command.args = "(Material.vnum)";
        command.hndl = DoMaterialInfo;
        Mud::getInstance().addCommand(command);
    }

    // Liquids.
    {
        command.name = "iliquid";
        command.help = "Show the informations about a specific liquid.";
        command.args = "(Liquid.vnum)";
        command.hndl = DoLiquidInfo;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "cliquid";
        command.help = "Materialize some liquid sinde a container.";
        command.args = "(Container.name)(Liquid.vnum)(Quantity)";
        command.hndl = DoLiquidCreate;
        Mud::getInstance().addCommand(command);
    }

    // Productions.
    {
        command.name = "iproduction";
        command.help = "Provide all the information regarding the given production.";
        command.args = "(Production.vnum)";
        command.hndl = DoProductionInfo;
        Mud::getInstance().addCommand(command);
    }

    // Profession.
    {
        command.name = "iprofession";
        command.help = "Provide all the information regarding the given profession.";
        command.args = "(Profession.command)";
        command.hndl = DoProfessionInfo;
        Mud::getInstance().addCommand(command);
    }

    //
    // LISTS
    //
    {
        command.name = "lmodel";
        command.help = "List all the models for the items.";
        command.args = "";
        command.hndl = DoModelList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "litem";
        command.help = "List all the items.";
        command.args = "";
        command.hndl = DoItemList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lmobile";
        command.help = "List all the mobiles.";
        command.args = "";
        command.hndl = DoMobileList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lplayer";
        command.help = "List all the players.";
        command.args = "";
        command.hndl = DoPlayerList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "larea";
        command.help = "List all the areas.";
        command.args = "";
        command.hndl = DoAreaList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lroom";
        command.help = "List all the rooms.";
        command.args = "";
        command.hndl = DoRoomList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lrace";
        command.help = "List all the races.";
        command.args = "";
        command.hndl = DoRaceList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lfaction";
        command.help = "List all the factions.";
        command.args = "";
        command.hndl = DoFactionList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lskill";
        command.help = "List all the skills.";
        command.args = "";
        command.hndl = DoSkillList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lwriting";
        command.help = "List all the writings.";
        command.args = "";
        command.hndl = DoWritingList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lcorpse";
        command.help = "List all the corpses.";
        command.args = "";
        command.hndl = DoCorpseList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lcontinent";
        command.help = "List all the continents.";
        command.args = "";
        command.hndl = DoContinentList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lmaterial";
        command.help = "List all the materials.";
        command.args = "";
        command.hndl = DoMaterialList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lprofession";
        command.help = "Get the list of all the professions.";
        command.args = "";
        command.hndl = DoProfessionList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lproduction";
        command.help = "Get the list of all the productions.";
        command.args = "";
        command.hndl = DoProductionList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lliquid";
        command.help = "List all the liquids.";
        command.args = "";
        command.hndl = DoLiquidList;
        Mud::getInstance().addCommand(command);
    }
    {
        command.name = "lbuild";
        command.help = "List all the buildings.";
        command.args = "";
        command.hndl = DoBuildingList;
        Mud::getInstance().addCommand(command);
    }
}
