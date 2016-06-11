/// @file   comm_general.cpp
/// @brief  Implements <b>general commands</b> used by the player.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include <istream>
#include <time.h>

// Other Include.
#include "mud.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "utilities/table.hpp"

using namespace std;

void DoDirection(Character * character, Direction direction)
{
    // Check if the player it's already doing something.
    StopAction(character);

    std::string error;
    Room * destination = character->canMoveTo(direction, error);
    if (destination == nullptr)
    {
        character->sendMsg(error + "\n");
        return;
    }

    // ////////////////////////////////////////////////////////////////////////
    unsigned int speed;
    // Calculate the time needed to move.
    if (character->posture == CharacterPosture::Stand)
    {
        character->sendMsg("You start to go " + GetDirectionName(direction) + "...\n");
        speed = 2;
    }
    else if (character->posture == CharacterPosture::Crouch)
    {
        character->sendMsg("You move crouching towards " + GetDirectionName(direction) + "...\n");
        speed = 4;
    }
    else if (character->posture == CharacterPosture::Prone)
    {
        character->sendMsg("You begin to crawl to " + GetDirectionName(direction) + "...\n");
        speed = 6;
    }
    else
    {
        character->sendMsg("You can't move!\n");
        return;
    }
    if (!character->getAction()->setMove(destination, direction, speed))
    {
        character->sendMsg("You can't move.\n");
        return;
    }
}

void DoTravel(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Stop any ongoing action.
    StopAction(character);

    // Check if the room is a travel point.
    if (!HasFlag(character->room->flags, RoomFlag::TravelPoint))
    {
        throw std::runtime_error("You can't travel from here.\n");
    }

    Room * destination = Mud::instance().findTravelPoint(character->room);
    if (destination == nullptr)
    {
        throw std::runtime_error("You can't find an exit from here.\n");
    }

    std::string msgDepart = character->getNameCapital() + " goes outside.\n";
    std::string msgArrive = character->getNameCapital() + " enter the room.\n";
    std::string msgChar = "You begin to travel...";

    // Move character.
    character->moveTo(destination, msgDepart, msgArrive, msgChar);
}

void DoSave(Character * character, std::istream & sArgs)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
    // Check no more input.
    NoMore(character, sArgs);
    // Stop any ongoing action.
    StopAction(character);

    Player * player = character->toPlayer();

    SQLiteDbms::instance().beginTransaction();
    if (player->updateOnDB())
    {
        SQLiteDbms::instance().endTransaction();
        player->sendMsg("Saved.\n");
    }
    else
    {
        SQLiteDbms::instance().rollbackTransection();
        player->sendMsg("Something goes wrong during save, re-try later...\n");
    }
}

void DoQuit(Character * character, std::istream & sArgs)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
    // Check no more input.
    NoMore(character, sArgs);
    // Stop any ongoing action.
    StopAction(character);

    Player * player = character->toPlayer();

    // If s/he finished connecting, tell others s/he has left.
    if (player->connection_state == ConnectionState::Playing)
    {
        // Say goodbye to player.
        player->sendMsg("See you next time!\n");

        if (player->room != nullptr)
        {
            // Set the list of exceptions.
            CharacterVector exceptions;
            exceptions.push_back(character);
            // Send the message inside the room.
            player->room->sendToAll("Player " + player->getName() + " disappear in a puff of smoke.\n", exceptions);
        }

        Logger::log(LogLevel::Global, "Player " + player->getName() + " has left the game.");

        // End of properly connected.
        player->closeConnection();
    }
}

void DoWho(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);

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
        string location("Nowhere");
        if (iterator->room != nullptr)
        {
            location = iterator->room->name;
        }
        table.addRow(
        { iterator->getName(), location });
    }
    output += table.getTable();
    output += "# Total " + Formatter::yellow() + "Players" + Formatter::reset() + " :" + ToString(table.getNumRows())
        + "\n";
    character->sendMsg(output);
}

void DoSet(Character * character, std::istream & sArgs)
{
    string input;
    // Get the setting that the player want to set.
    sArgs >> input >> std::ws;
    if (sArgs.fail())
    {
        character->sendMsg("Usage: set <opt> <arguments>  \n");
        character->sendMsg("       ---------------------  \n");
        character->sendMsg("            des  [description]\n");
        return;
    }
    if (input == "des")
    {
        // Get the description.
        getline(sArgs, input);
        if (sArgs.fail())
        {
            character->sendMsg("You can't set an empty description.\n");
        }
        else
        {
            character->description = input;

            character->sendMsg("You had set your description:\n");
            character->sendMsg(character->description + "\n");
        }
    }
    else
    {
        character->sendMsg("Usage: set <opt> <arguments>  \n");
        character->sendMsg("       ---------------------  \n");
        character->sendMsg("            des  [description]\n");
    }
}

void DoStop(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    character->sendMsg(character->getAction()->stop() + "\n");
}

void DoLook(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // If there are no arguments, show the room.
    if (arguments.empty())
    {
        character->sendMsg(character->room->getLook(character));
    }
    else if (arguments.size() == 1)
    {
        Character * target = character->room->findCharacter(arguments[0].first, arguments[0].second, character);
        if (target)
        {
            if (character->canSee(target))
            {
                character->sendMsg(character->getLook(target));
                if (target->canSee(character))
                {
                    // Notify to other character, that this one are looking at him.
                    target->sendMsg(character->getNameCapital() + " look at you.\n\n");
                }
                return;
            }
        }
        Item * item = character->findNearbyItem(arguments[0].first, arguments[0].second);
        if (item)
        {
            character->sendMsg(item->getLook());
            return;
        }
        character->sendMsg("You don't see '" + arguments[0].first + "' anywhere.\n");
    }
    else if (arguments.size() == 2)
    {
        Character * target = character->room->findCharacter(arguments[1].first, arguments[1].second, character);
        if (target)
        {
            if (character->canSee(target))
            {
                Item * item = target->findEquipmentItem(arguments[0].first, arguments[0].second);
                if (item)
                {
                    character->sendMsg(item->getLook());
                }
                else
                {
                    character->sendMsg("You don't see " + arguments[0].first + " on " + target->getName() + ".\n");
                }
                return;
            }
        }
        Item * container = character->findNearbyItem(arguments[1].first, arguments[1].second);
        if (container)
        {
            if (container->model->type == ModelType::Container)
            {
                Item * item = container->findContent(arguments[0].first, arguments[0].second);
                if (item)
                {
                    character->sendMsg(item->getLook());
                }
                else
                {
                    character->sendMsg("It's not insiede " + container->getName() + ".\n");
                    return;
                }
            }
            else
            {
                character->sendMsg(container->getNameCapital() + " is not a container.\n");
                return;
            }
        }
        character->sendMsg("You don't see the container '" + arguments[1].first + "' anywhere.\n");
    }
    else
    {
        character->sendMsg("You don't remeber how to look?\n");
    }
}

void DoHelp(Character * character, std::istream & sArgs)
{
    // Get the command.
    string command;
    sArgs >> command;
    // Check no more input.
    NoMore(character, sArgs);

    if (command.empty())
    {
        size_t totalCommands = 0, numColumns = 0;
        // Count the total number of command which can be displayed.
        for (auto it : Mud::instance().mudCommands)
        {
            if ((it.level == 1) && !HasFlag(character->flags, CharacterFlag::IsGod))
            {
                continue;
            }
            totalCommands++;
        }
        // Find the number of columns.
        for (numColumns = 3; numColumns < totalCommands; ++numColumns)
        {
            if ((totalCommands % numColumns) == 0)
            {
                break;
            }
        }
        Table commandTable = Table("Commands");
        for (size_t it = 0; it < numColumns; ++it)
        {
            commandTable.addColumn("", StringAlign::Left);
        }
        TableRow row;
        for (auto it : Mud::instance().mudCommands)
        {
            if ((it.level == 0) || !HasFlag(character->flags, CharacterFlag::IsGod))
            {
                row.push_back(it.name);
            }
            if (row.size() == numColumns)
            {
                commandTable.addRow(row);
                row.clear();
            }
        }
        character->sendMsg(commandTable.getTable(true));
    }
    else
    {
        for (auto iterator : Mud::instance().mudCommands)
        {
            if (iterator.level == 1 && !HasFlag(character->flags, CharacterFlag::IsGod))
            {
                continue;
            }
            if (BeginWith(ToLower(iterator.name), command))
            {
                std::string msg;
                msg += "Showing help for command :" + iterator.name + "\n";
                msg += Formatter::yellow() + " Command   : " + Formatter::reset() + iterator.name + "\n";
                msg += Formatter::yellow() + " Level     : " + Formatter::reset() + ToString(iterator.level) + "\n";
                msg += Formatter::yellow() + " Arguments : " + Formatter::reset() + iterator.args + "\n";
                msg += Formatter::yellow() + " Help      : " + Formatter::reset() + iterator.help + "\n";
                character->sendMsg(msg);
                return;
            }
        }
        character->sendMsg("There is no help for '" + command + ".\n");
    }
}

void DoPrompt(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    Player * player = character->toPlayer();
    string prompt;

    getline(sArgs, prompt);

    if (prompt.empty())
    {
        player->sendMsg("Current prompt:\n");
        player->sendMsg(player->prompt + "\n");
        player->sendMsg("Type " + Formatter::yellow() + "prompt help" + Formatter::reset() + " to read the guide.\n");
        return;
    }

    if (GetWords(prompt)[0] == "help")
    {
        player->sendMsg(Formatter::yellow() + "Prompt Help" + Formatter::reset() + "\n");
        player->sendMsg("You can set the prompt you prefer, respectfully to this constraints:\n");
        player->sendMsg(" - Not more than 15 characters.\n");
        player->sendMsg("\n");
        player->sendMsg("You can use the following shortcuts in you prompt:\n");
        player->sendMsg("    " + Formatter::italic() + "&n" + Formatter::reset() + " - Replace with player name.\n");
        player->sendMsg(
            "    " + Formatter::italic() + "&N" + Formatter::reset() + " - Replace with player name capitalized.\n");
        player->sendMsg(
            "    " + Formatter::italic() + "&h" + Formatter::reset() + " - Replace with player current health.\n");
        player->sendMsg(
            "    " + Formatter::italic() + "&H" + Formatter::reset() + " - Replace with player max health.\n");
        player->sendMsg(
            "    " + Formatter::italic() + "&s" + Formatter::reset() + " - Replace with player current stamina.\n");
        player->sendMsg(
            "    " + Formatter::italic() + "&S" + Formatter::reset() + " - Replace with player max stamina.\n");
        return;
    }
    player->prompt = prompt;
}

void DoTime(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);

    if (MudUpdater::instance().getDayPhase() == DayPhase::Morning)
    {
        character->sendMsg(Formatter::yellow() + "The sun has just risen.\n" + Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Day)
    {
        character->sendMsg(Formatter::yellow() + "The sun is high in the sky.\n" + Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Dusk)
    {
        character->sendMsg(
            Formatter::cyan() + "The sun is setting, the shadows begin to prevail.\n" + Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Night)
    {
        character->sendMsg(Formatter::blue() + "The darkness surrounds you.\n" + Formatter::reset());
    }
}

void DoStand(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    if (character->posture == CharacterPosture::Stand)
    {
        character->sendMsg("You are already standing.\n");
        return;
    }
    character->posture = CharacterPosture::Stand;
    character->sendMsg("You stand up.\n");
}

void DoCrouch(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    if (character->posture == CharacterPosture::Crouch)
    {
        character->sendMsg("You are already crouched.\n");
        return;
    }
    character->posture = CharacterPosture::Crouch;
    character->sendMsg("You put yourself crouched.\n");
}

void DoSit(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    if (character->posture == CharacterPosture::Sit)
    {
        character->sendMsg("You are already seated.\n");
        return;
    }
    character->posture = CharacterPosture::Sit;
    character->sendMsg("You sit down.\n");
}

void DoProne(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    if (character->posture == CharacterPosture::Prone)
    {
        character->sendMsg("You are already prone.\n");
        return;
    }
    character->posture = CharacterPosture::Prone;
    character->sendMsg("You put yourself prone.\n");
}

void DoRest(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    if (character->posture == CharacterPosture::Rest)
    {
        character->sendMsg("You are already resting.\n");
        return;
    }
    character->posture = CharacterPosture::Rest;
    character->sendMsg("You lie down.\n");
}

void DoStatistics(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    NoMore(character, sArgs);
    Player * player = character->toPlayer();

    std::string msg;
    msg += Formatter::magenta() + "Name: " + Formatter::reset() + player->getName();
    msg += Formatter::magenta() + " Race: " + Formatter::reset() + player->race->name + "\n";
    msg += Formatter::magenta() + "Gender: " + Formatter::reset() + GetGenderTypeName(player->gender) + "\n";
    msg += Formatter::magenta() + "Affiliation: " + Formatter::reset() + player->faction->name + "\n";
    msg += Formatter::magenta() + "Experience: " + Formatter::reset() + ToString(player->experience) + " px\n";
    msg += Formatter::magenta() + "Str " + Formatter::reset() + ToString(player->getStrength());
    msg += Formatter::magenta() + "Agi " + Formatter::reset() + ToString(player->getAgility());
    msg += Formatter::magenta() + "Per " + Formatter::reset() + ToString(player->getPerception());
    msg += Formatter::magenta() + "Con " + Formatter::reset() + ToString(player->getConstitution());
    msg += Formatter::magenta() + "Int " + Formatter::reset() + ToString(player->getIntelligence());
    msg += Formatter::magenta() + "Health " + Formatter::reset() + ToString(player->health) + "/"
        + ToString(player->getMaxHealth());
    msg += "(" + ToString(player->effects.getHealthMod()) + ") ";
    msg += Formatter::magenta() + "Stamina " + Formatter::reset() + ToString(player->stamina) + "/"
        + ToString(player->getMaxStamina());
    msg += "(" + ToString(player->effects.getHealthMod()) + ")\n";
    msg += Formatter::magenta() + "Armor Class " + Formatter::reset() + ToString(player->getArmorClass()) + "\n";
    msg += "You " + player->getHunger();
    msg += "You " + player->getThirst();
    msg += "You are " + GetPostureName(player->posture) + ".\n";
    player->sendMsg(msg);
}

void DoRent(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    Player * player = character->toPlayer();

    // Get the current room.
    Room * room = player->room;

    // Check no more input.
    NoMore(character, sArgs);

    // Check if the room allow to rent.
    if (!HasFlag(room->flags, RoomFlag::Rent))
    {
        character->sendMsg("You can't rent here.\n");
        return;
    }
    else
    {
        player->rent_room = room->vnum;
        player->doCommand("quit");
    }
}

void DoSkills(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    Player * player = character->toPlayer();

    string output;

    // Check no more input.
    NoMore(character, sArgs);

    player->sendMsg(
        "     ##    " + Formatter::yellow() + "LvL" + Formatter::green() + "    Skill" + Formatter::reset() + "\n");
    for (auto iterator : player->skills)
    {
        Skill * skill = Mud::instance().findSkill(iterator.first);
        if (skill != nullptr)
        {
            output = "     ";
            if (skill->vnum < 10) output += " ";
            output += ToString(skill->vnum);
            output += "    ";
            if (iterator.second < 10) output += "  ";
            else if (iterator.second < 100) output += " ";
            output += ToString(iterator.second);
            output += "    " + skill->name + "\n";
            player->sendMsg(output);
        }
    }
}

void DoServer(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    character->sendMsg("    Mud         : RadMud.\n");
    character->sendMsg("    Version     : " + kVersion + "\n");
    character->sendMsg("    Uptime      : " + ToString(Mud::instance().getUpTime()) + " s\n");
    character->sendMsg("\n");
    character->sendMsg("    Players     : " + ToString(Mud::instance().mudPlayers.size()) + "\n");
    character->sendMsg("    Mobiles     : " + ToString(Mud::instance().mudMobiles.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Models      : " + ToString(Mud::instance().mudModels.size()) + "\n");
    character->sendMsg("    Items       : " + ToString(Mud::instance().mudItems.size()) + "\n");
    character->sendMsg("    Corpses     : " + ToString(Mud::instance().mudCorpses.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Continents  : " + ToString(Mud::instance().mudContinents.size()) + "\n");
    character->sendMsg("    Areas       : " + ToString(Mud::instance().mudAreas.size()) + "\n");
    character->sendMsg("    Rooms       : " + ToString(Mud::instance().mudRooms.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Races       : " + ToString(Mud::instance().mudRaces.size()) + "\n");
    character->sendMsg("    Factions    : " + ToString(Mud::instance().mudFactions.size()) + "\n");
    character->sendMsg("    Skills      : " + ToString(Mud::instance().mudSkills.size()) + "\n");
    character->sendMsg("    Writings    : " + ToString(Mud::instance().mudWritings.size()) + "\n");
    character->sendMsg("    Materials   : " + ToString(Mud::instance().mudMaterials.size()) + "\n");
    character->sendMsg("    Professions : " + ToString(Mud::instance().mudProfessions.size()) + "\n");
    character->sendMsg("    Productions : " + ToString(Mud::instance().mudProductions.size()) + "\n");
    character->sendMsg("    Schematics  : " + ToString(Mud::instance().mudBuildings.size()) + "\n");
    character->sendMsg("    Liquids     : " + ToString(Mud::instance().mudLiquids.size()) + "\n");
    character->sendMsg("    News        : " + ToString(Mud::instance().mudNews.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Commands    : " + ToString(Mud::instance().mudCommands.size()) + "\n");

}
