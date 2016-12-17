/// @file   general.cpp
/// @brief  Implements <b>general commands</b> used by the player.
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

#include "general.hpp"

#include "moveAction.hpp"
#include "logger.hpp"
#include "mud.hpp"

void LoadGeneralCommands()
{
    {
        Mud::instance().addCommand(Command().setName("quit")
                                            .setHelp("Leave the game.")
                                            .setHndl(DoQuit)
                                            .setTypedCompletely(true));
    }
    {
        Mud::instance().addCommand(Command().setName("who")
                                            .setHelp("List all the character online.")
                                            .setHndl(DoWho));
    }
    {
        Command command;
        command.name = "set";
        command.help = "Set some character texts(eg. descr).";
        command.arguments = "(setting) (value)";
        command.hndl = DoSet;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "stop";
        command.help = "Stop the current character action.";
        command.arguments = "NONE";
        command.hndl = DoStop;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "look";
        command.help = "Look at something or someone.";
        command.arguments = "[(something) or (someone)]";
        command.hndl = DoLook;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "help";
        command.help = "Show the list of commands or show help for a given command.";
        command.arguments = "(command)";
        command.hndl = DoHelp;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "prompt";
        command.help = "Modify your prompt.";
        command.arguments = "(help)|(prompt definition)";
        command.hndl = DoPrompt;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "time";
        command.help = "Give the current day phase.";
        command.arguments = "NONE";
        command.hndl = DoTime;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "stand";
        command.help = "Make the player stand.";
        command.arguments = "NONE";
        command.hndl = DoStand;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "crouch";
        command.help = "The player crouches down himself, it's a good stance for hiding.";
        command.arguments = "NONE";
        command.hndl = DoCrouch;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "sit";
        command.help = "The player sits down, ideal for a quick break.";
        command.arguments = "NONE";
        command.hndl = DoSit;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "prone";
        command.help = "The player starts prone, a perfect position to shoot long distance.";
        command.arguments = "NONE";
        command.hndl = DoProne;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "rest";
        command.help = "The player lies down and begin to rest.";
        command.arguments = "NONE";
        command.hndl = DoRest;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "statistics";
        command.help = "Show player statistics.";
        command.arguments = "NONE";
        command.hndl = DoStatistics;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "rent";
        command.help = "Allow player to rent and disconnect.";
        command.arguments = "NONE";
        command.hndl = DoRent;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "skills";
        command.help = "Shows the playes skills and their level.";
        command.arguments = "NONE";
        command.hndl = DoSkills;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "server";
        command.help = "Shows the server statistics.";
        command.arguments = "NONE";
        command.hndl = DoServer;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }
    {
        Command command;
        command.name = "travel";
        command.help = "Allow the character to travel between areas.";
        command.arguments = "NONE";
        command.hndl = DoTravel;
        command.canUseInCombat = true;
        Mud::instance().addCommand(command);
    }

}

bool DoDirection(Character * character, Direction direction)
{
    // Stop any action the character is executing.
    StopAction(character);
    std::string error;
    if (!MoveAction::canMoveTo(character, direction, error, false))
    {
        character->sendMsg(error + "\n");
        return false;
    }
    // Get the destination.
    auto destination = character->room->findExit(direction)->destination;
    auto moveAction = std::make_shared<MoveAction>(character, destination, direction);
    // Check the new action.
    error = std::string();
    if (moveAction->check(error))
    {
        // Set the new action.
        character->setAction(moveAction);
        // Calculate the time needed to move.
        if (character->posture == CharacterPosture::Stand)
        {
            character->sendMsg("You start to go %s...\n", direction.toString());
        }
        else if (character->posture == CharacterPosture::Crouch)
        {
            character->sendMsg("You move crouching towards %s...\n", direction.toString());
        }
        else if (character->posture == CharacterPosture::Prone)
        {
            character->sendMsg("You begin to crawl to %s...\n", direction.toString());
        }
        return true;
    }
    character->sendMsg("%s\n", error);
    return false;
}

bool DoTravel(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any ongoing action.
    StopAction(character);
    // Check if the room is a travel point.
    if (!HasFlag(character->room->flags, RoomFlag::TravelPoint))
    {
        character->sendMsg("You can't travel from here.\n");
        return false;
    }
    auto destination = Mud::instance().findTravelPoint(character->room);
    if (destination == nullptr)
    {
        character->sendMsg("You can't find an exit from here.\n");
        return false;
    }
    auto msgDepart = character->getNameCapital() + " goes outside.\n";
    auto msgArrive = character->getNameCapital() + " enter the room.\n";
    auto msgChar = "You begin to travel...";
    // Move character.
    character->moveTo(destination, msgDepart, msgArrive, msgChar);
    return true;
}

bool DoQuit(Character * character, ArgumentHandler & /*args*/)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
    // Stop any ongoing action.
    StopAction(character);
    auto player = character->toPlayer();
    // If s/he finished connecting, tell others s/he has left.
    if (player->connectionState == ConnectionState::Playing)
    {
        // Say goodbye to player.
        player->sendMsg("See you next time!\n");
        if (player->room != nullptr)
        {
            // Send the message inside the room.
            player->room->sendToAll(
                "Player %s disappear in a puff of smoke.\n",
                {character},
                player->getName());
        }
        Logger::log(LogLevel::Global, "Player %s has left the game.", player->getName());
        // End of properly connected.
        player->closeConnection();
        return true;
    }
    return false;
}

bool DoWho(Character * character, ArgumentHandler & /*args*/)
{
    Table table = Table();
    table.addColumn("Player", StringAlign::Left);
    table.addColumn("Location", StringAlign::Left);
    std::string output;
    for (auto iterator : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        std::string location = "Nowhere";
        if (iterator->room != nullptr)
        {
            location = iterator->room->name;
        }
        table.addRow({iterator->getName(), location});
    }
    character->sendMsg(table.getTable());
    character->sendMsg(
        "# Total %sPlayers%s: %s\n",
        Formatter::yellow(),
        Formatter::reset(),
        ToString(table.getNumRows()));
    return true;
}

bool DoSet(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("What do you want to set?");
    }
    else
    {
        if (args[0].getContent() == "des")
        {
            auto newDescription = args.substr(1);
            if (newDescription.empty())
            {
                character->sendMsg("You can't set an empty description.\n");
            }
            else
            {
                character->description = newDescription;
                character->sendMsg("You had set your description:\n");
                character->sendMsg(character->description + "\n");
                return true;
            }
        }
    }
    character->sendMsg("Usage: set <opt> <arguments>  \n");
    character->sendMsg("       ---------------------  \n");
    character->sendMsg("            des  [description]\n");
    return false;
}

bool DoStop(Character * character, ArgumentHandler & /*args*/)
{
    if (character->getAction()->getType() != ActionType::Combat)
    {
        character->sendMsg(character->getAction()->stop() + "\n");
        character->popAction();
        return true;
    }
    return false;
}

bool DoLook(Character * character, ArgumentHandler & args)
{
    // If there are no arguments, show the room.
    if (args.empty())
    {
        character->sendMsg(character->room->getLook(character));
    }
    else if (args.size() == 1)
    {
        auto target = character->room->findCharacter(args[0].getContent(), args[0].getIndex(), {
            character});
        if (target)
        {
            if (character->canSee(target))
            {
                // Show the target.
                character->sendMsg(target->getLook());
                // Notify the target.
                if (target->canSee(character))
                {
                    // Notify to other character, that this one are looking at him.
                    target->sendMsg("%s looks at you.\n\n", character->getNameCapital());
                }
                return true;
            }
        }
        auto item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
        if (item != nullptr)
        {
            character->sendMsg(item->getLook());
            return true;
        }
        else
        {
            character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
        }
    }
    else if (args.size() == 2)
    {
        auto target = character->room->findCharacter(args[1].getContent(), args[1].getIndex(), {character});
        auto container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
        if (target != nullptr)
        {
            if (character->canSee(target))
            {
                auto item = target->findEquipmentItem(args[0].getContent(), args[0].getIndex());
                if (item != nullptr)
                {
                    character->sendMsg(item->getLook());
                    return true;
                }
                else
                {
                    character->sendMsg(
                        "You don't see %s on %s.\n",
                        args[0].getContent(),
                        target->getName());
                }
            }
            else
            {
                character->sendMsg(
                    "You don't see the container '%s' anywhere.\n",
                    args[1].getContent());
            }
        }
        else if (container)
        {
            if (container->isAContainer())
            {
                auto item = container->findContent(args[0].getContent(), args[0].getIndex());
                if (item)
                {
                    character->sendMsg(item->getLook());
                    return true;
                }
                else
                {
                    character->sendMsg("It's not insiede %s.\n", container->getName(true));
                }
            }
            else
            {
                character->sendMsg("%s is not a container.\n", container->getNameCapital(true));
            }
        }
        else
        {
            character->sendMsg(
                "You don't see the container '%s' anywhere.\n",
                args[1].getContent());
        }
    }
    else
    {
        character->sendMsg("You don't remeber how to look?\n");
    }
    return false;
}

bool DoHelp(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        size_t numColumns = 4;
        TableRow baseRow, godsRow;
        Table baseCommands("Commands"), godsCommands("Management");
        for (size_t it = 0; it < numColumns; ++it)
        {
            baseCommands.addColumn("", StringAlign::Left, 15);
            godsCommands.addColumn("", StringAlign::Left, 15);
        }
        for (auto it : Mud::instance().mudCommands)
        {
            if (it.canUse(character))
            {
                if (it.gods)
                {
                    godsRow.push_back(it.name);
                    if (godsRow.size() == numColumns)
                    {
                        godsCommands.addRow(godsRow);
                        godsRow.clear();
                    }
                }
                else
                {
                    baseRow.push_back(it.name);
                    if (baseRow.size() == numColumns)
                    {
                        baseCommands.addRow(baseRow);
                        baseRow.clear();
                    }
                }
            }
        }
        if (!godsRow.empty())
        {
            for (auto it = godsRow.size(); it < 4; ++it)
            {
                godsRow.emplace_back("");
            }
            godsCommands.addRow(godsRow);
        }
        if (!baseRow.empty())
        {
            for (auto it = baseRow.size(); it < 4; ++it)
            {
                baseRow.emplace_back("");
            }
            baseCommands.addRow(baseRow);
        }
        character->sendMsg(baseCommands.getTable(true));
        character->sendMsg(godsCommands.getTable(true));
    }
    else if (args.size() == 1)
    {
        auto command = args[0].getContent();
        for (auto iterator : Mud::instance().mudCommands)
        {
            if (iterator.canUse(character))
            {
                if (BeginWith(ToLower(iterator.name), command))
                {
                    std::string msg;
                    msg += "Showing help for command :" + iterator.name + "\n";
                    msg += Formatter::yellow() + " Command   : " + Formatter::reset()
                           + iterator.name + "\n";
                    msg += Formatter::yellow() + " Level     : " + Formatter::reset()
                           + ToString(iterator.gods) + "\n";
                    msg += Formatter::yellow() + " Arguments : " + Formatter::reset()
                           + iterator.arguments + "\n";
                    msg += Formatter::yellow() + " Help      : " + Formatter::reset()
                           + iterator.help + "\n";
                    character->sendMsg(msg);
                    return true;
                }
            }
        }
        character->sendMsg("There is no help for '%s'.\n", args.getOriginal());
        return false;
    }
    return true;
}

bool DoPrompt(Character * character, ArgumentHandler & args)
{
    NoMobile(character);
    auto player = character->toPlayer();
    if (args.empty())
    {
        character->sendMsg("Current prompt:\n");
        character->sendMsg("    %s\n", player->prompt);
        character->sendMsg("Type %sprompt help %sto read the guide.\n", Formatter::yellow(), Formatter::reset());
    }
    else
    {
        if (args[0].getContent() == "help")
        {
            std::string msg;
            std::string ITL = Formatter::italic(), RES = Formatter::reset();
            msg += Formatter::yellow() + "Prompt Help" + Formatter::reset() + "\n";
            msg += "You can set the prompt you prefer, respectfully to this constraints:\n";
            msg += " - Not more than 15 characters.\n";
            msg += "\n";
            msg += "You can use the following shortcuts in you prompt:\n";
            msg += "    " + ITL + "&n" + RES + " - Player name.\n";
            msg += "    " + ITL + "&N" + RES + " - Player name capitalized.\n";
            msg += "    " + ITL + "&h" + RES + " - Player current health.\n";
            msg += "    " + ITL + "&H" + RES + " - Player maximum health.\n";
            msg += "    " + ITL + "&s" + RES + " - Player current stamina.\n";
            msg += "    " + ITL + "&S" + RES + " - Player maximum stamina.\n";
            msg += "    " + ITL + "&T" + RES + " - Currently aimed character.\n";
            player->sendMsg(msg);
        }
        else
        {
            player->prompt = args.substr(0);
        }
    }
    return true;
}

bool DoTime(Character * character, ArgumentHandler & /*args*/)
{
    if (MudUpdater::instance().getDayPhase() == DayPhase::Morning)
    {
        character->sendMsg("%sThe sun has just risen.%s\n", Formatter::yellow(), Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Day)
    {
        character->sendMsg("%sThe sun is high in the sky.%s\n", Formatter::yellow(), Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Dusk)
    {
        character->sendMsg("%sThe sun is setting, the shadows begin to prevail.%s\n",
                           Formatter::cyan(), Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Night)
    {
        character->sendMsg("%sThe darkness surrounds you.%s\n", Formatter::blue(), Formatter::reset());
    }
    return true;
}

bool DoStand(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Stand)
    {
        character->sendMsg("You are already standing.\n");
        return false;
    }
    character->posture = CharacterPosture::Stand;
    character->sendMsg("You stand up.\n");
    return true;
}

bool DoCrouch(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Crouch)
    {
        character->sendMsg("You are already crouched.\n");
        return false;
    }
    character->posture = CharacterPosture::Crouch;
    character->sendMsg("You put yourself crouched.\n");
    return true;
}

bool DoSit(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Sit)
    {
        character->sendMsg("You are already seated.\n");
        return false;
    }
    character->posture = CharacterPosture::Sit;
    character->sendMsg("You sit down.\n");
    return true;
}

bool DoProne(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Prone)
    {
        character->sendMsg("You are already prone.\n");
        return false;
    }
    character->posture = CharacterPosture::Prone;
    character->sendMsg("You put yourself prone.\n");
    return true;
}

bool DoRest(Character * character, ArgumentHandler & /*args*/)
{
    // Stop any action the character is executing.
    StopAction(character);
    if (character->posture == CharacterPosture::Rest)
    {
        character->sendMsg("You are already resting.\n");
        return false;
    }
    character->posture = CharacterPosture::Rest;
    character->sendMsg("You lie down.\n");
    return true;
}

bool DoStatistics(Character * character, ArgumentHandler & /*args*/)
{
    NoMobile(character);
    auto player = character->toPlayer();

    std::string msg;
    std::string MAG = Formatter::magenta();
    std::string BLD = Formatter::bold();
    std::string RES = Formatter::reset();
    msg += MAG + "Name        : " + RES + player->getName() + " ";
    msg += MAG + "Gender : " + RES + GetGenderTypeName(player->gender) + " ";
    msg += MAG + "Race : " + RES + player->race->name + "\n";
    msg += MAG + "Weight      : " + RES + ToString(player->weight) + " " + Mud::instance().getWeightMeasure() + "\n";
    msg += MAG + "Affiliation : " + RES + player->faction->name + "\n";
    msg += MAG + "Experience  : " + RES + ToString(player->experience) + " px\n";
    msg += MAG + "    Str       " + RES;
    msg += ToString(player->getAbility(Ability::Strength, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Strength)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Strength)) + "]\n";
    msg += MAG + "    Agi       " + RES;
    msg += ToString(player->getAbility(Ability::Agility, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Agility)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Agility)) + "]\n";
    msg += MAG + "    Per       " + RES;
    msg += ToString(player->getAbility(Ability::Perception, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Perception)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Perception)) + "]\n";
    msg += MAG + "    Con       " + RES;
    msg += ToString(player->getAbility(Ability::Constitution, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Constitution)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Constitution)) + "]\n";
    msg += MAG + "    Int       " + RES;
    msg += ToString(player->getAbility(Ability::Intelligence, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Intelligence)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Intelligence)) + "]\n";
    msg += MAG + "    Health    " + RES;
    msg += ToString(player->health) + "/" + ToString(player->getMaxHealth());
    msg += "(" + ToString(player->effects.getHealthMod()) + ")\n";
    msg += MAG + "    Stamina   " + RES;
    msg += ToString(player->stamina) + "/" + ToString(player->getMaxStamina());
    msg += "(" + ToString(player->effects.getHealthMod()) + ")\n";
    msg += MAG + "Armor Class : " + RES;
    msg += ToString(player->getArmorClass()) + "\n";
    msg += "You " + BLD + player->getHealthCondition(true) + RES + ".\n";
    msg += "You " + BLD + player->getStaminaCondition() + RES + ".\n";
    msg += "You " + BLD + player->getHungerCondition() + RES + ".\n";
    msg += "You " + BLD + player->getThirstCondition() + RES + ".\n";
    msg += "You are " + BLD + player->posture.toString() + RES + ".\n\n";
    if (player->getAction()->getType() != ActionType::Wait)
    {
        msg += "You are " + BLD + player->getAction()->getDescription() + RES + ".\n";
    }
    if (player->combatHandler.getPredefinedTarget() != nullptr)
    {
        msg += "You are fighting with " + BLD + player->combatHandler.getPredefinedTarget()->getName() + RES + ".\n";
    }
    if (player->combatHandler.getAimedTarget() != nullptr)
    {
        msg += "You are aiming at " + BLD + player->combatHandler.getAimedTarget()->getName() + RES + ".\n";
    }
    player->sendMsg(msg);
    return true;
}

bool DoRent(Character * character, ArgumentHandler & /*args*/)
{
    NoMobile(character);
    auto player = character->toPlayer();
    // Check if the room allow to rent.
    if (!HasFlag(player->room->flags, RoomFlag::Rent))
    {
        character->sendMsg("You can't rent here.\n");
        return false;
    }
    else
    {
        player->rent_room = player->room->vnum;
        player->doCommand("quit");
    }
    return true;
}

bool DoSkills(Character * character, ArgumentHandler & /*args*/)
{
    NoMobile(character);
    auto player = character->toPlayer();
    Table table = Table();
    table.addColumn("LvL", StringAlign::Left);
    table.addColumn("Skill", StringAlign::Left);
    for (auto iterator : player->skills)
    {
        Skill * skill = Mud::instance().findSkill(iterator.first);
        if (skill)
        {
            table.addRow({skill->name, ToString(iterator.second)});
        }
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoServer(Character * character, ArgumentHandler & /*args*/)
{
    std::string msg = "    Mud         : RadMud.\n";
    msg += "    Version     : ";
    msg += ToString(RADMUD_MAJOR_VERSION) + ToString(RADMUD_MINOR_VERSION) + ToString(RADMUD_VERSION) + "\n";
    msg += "    Uptime      : " + ToString(Mud::instance().getUpTime()) + " s\n";
    msg += "    Players     : " + ToString(Mud::instance().mudPlayers.size()) + "\n";
    msg += "    Mobiles     : " + ToString(Mud::instance().mudMobiles.size()) + "\n";
    msg += "    Models      : " + ToString(Mud::instance().mudItemModels.size()) + "\n";
    msg += "    Items       : " + ToString(Mud::instance().mudItems.size()) + "\n";
    msg += "    Corpses     : " + ToString(Mud::instance().mudCorpses.size()) + "\n";
    msg += "    Continents  : " + ToString(Mud::instance().mudContinents.size()) + "\n";
    msg += "    Areas       : " + ToString(Mud::instance().mudAreas.size()) + "\n";
    msg += "    Rooms       : " + ToString(Mud::instance().mudRooms.size()) + "\n";
    msg += "    Races       : " + ToString(Mud::instance().mudRaces.size()) + "\n";
    msg += "    Factions    : " + ToString(Mud::instance().mudFactions.size()) + "\n";
    msg += "    Skills      : " + ToString(Mud::instance().mudSkills.size()) + "\n";
    msg += "    Writings    : " + ToString(Mud::instance().mudWritings.size()) + "\n";
    msg += "    Materials   : " + ToString(Mud::instance().mudMaterials.size()) + "\n";
    msg += "    Professions : " + ToString(Mud::instance().mudProfessions.size()) + "\n";
    msg += "    Productions : " + ToString(Mud::instance().mudProductions.size()) + "\n";
    msg += "    Schematics  : " + ToString(Mud::instance().mudBuildings.size()) + "\n";
    msg += "    Liquids     : " + ToString(Mud::instance().mudLiquids.size()) + "\n";
    msg += "    News        : " + ToString(Mud::instance().mudNews.size()) + "\n";
    msg += "    Commands    : " + ToString(Mud::instance().mudCommands.size()) + "\n";
    character->sendMsg(msg);
    return true;
}
