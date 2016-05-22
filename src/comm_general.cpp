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

#include "commands.hpp"
#include "constants.hpp"
#include "mud.hpp"
#include "utilities/table.hpp"
// Other Include.
#include "utils.hpp"

using namespace std;

void NoMobile(Character * character)
{
    if (character->isMobile())
    {
        throw std::runtime_error("Npcs are not allowed to execute this command.\n");
    }
}

void NoMore(Character * character, std::istream & sArgs)
{
    std::string sLine;
    getline(sArgs, sLine);
    if (!sLine.empty())
    {
        character->sendMsg("Unexpected input :'" + sLine + "'.\n");
        throw std::runtime_error("");
    }
}

void StopAction(Character * character)
{
    if ((character->action.getType() != ActionType::Wait) && (character->action.getType() != ActionType::NoAction))
    {
        character->doCommand("stop");
    }
}

ArgumentList ParseArgs(std::istream & sArgs)
{
    ArgumentList arguments;
    vector<string> words;

    // Get the words from the input line.
    string argumentsLine;
    getline(sArgs, argumentsLine);
    words = GetWords(argumentsLine);
    for (auto it : words)
    {
        // Set one by default.
        int number = 1;
        // Extract the number.
        ExtractNumber(it, number);
        // Add the argument.
        arguments.push_back(std::make_pair(it, number));
    }
    return arguments;
}

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
    unsigned int speed = 2;
    // Calculate the time needed to move.
    switch (character->posture)
    {
        case CharacterPosture::Stand:
            character->sendMsg("You start to go " + GetDirectionName(direction) + "...\n");
            //character->action.cooldown = 30;
            speed = 2;
            break;
        case CharacterPosture::Crouch:
            character->sendMsg("You move crouching towards " + GetDirectionName(direction) + "...\n");
            speed = 4;
            break;
        case CharacterPosture::Prone:
            character->sendMsg("You begin to crawl to " + GetDirectionName(direction) + "...\n");
            speed = 6;
            break;
        case CharacterPosture::NoPosure:
            LogWarning("No posture set.");
            break;
        case CharacterPosture::Sit:
        case CharacterPosture::Rest:
            character->sendMsg("You can't move!\n");
            return;
    }
    if (!character->action.setMove(destination, direction, speed))
    {
        character->sendMsg("You can't move.\n");
        return;
    }
    // ////////////////////////////////////////////////////////////////////////
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

    Room * destination = Mud::getInstance().findTravelPoint(character->room);
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

    Mud::getInstance().getDbms().beginTransaction();
    if (player->updateOnDB())
    {
        Mud::getInstance().getDbms().endTransaction();
        player->sendMsg("Saved.\n");
    }
    else
    {
        Mud::getInstance().getDbms().rollbackTransection();
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

        // End of properly connected.
        player->closeConnection();

        LogMessage(kMPla, "Player " + player->getName() + " has left the game.");

        if (player->room != nullptr)
        {
            // Send to the other player in the room a message.
            player->room->sendToAll("Player " + player->getName() + " disappear in a puff of smoke.\n", player);
        }
    }
}

void DoWho(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);

    Table table = Table();
    table.addColumn("Player", kAlignLeft);
    table.addColumn("Location", kAlignLeft);

    std::string output;
    for (auto iterator : Mud::getInstance().mudPlayers)
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
    output += "# Total " + Telnet::yellow() + "Players" + Telnet::reset() + " :" + ToString(table.getNumRows()) + "\n";
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
    character->sendMsg(character->action.stop() + "\n");
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
        for (auto it : Mud::getInstance().mudCommands)
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
            commandTable.addColumn("", kAlignLeft);
        }
        TableRow row;
        for (auto it : Mud::getInstance().mudCommands)
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
        for (auto iterator : Mud::getInstance().mudCommands)
        {
            if (iterator.level == 1 && !HasFlag(character->flags, CharacterFlag::IsGod))
            {
                continue;
            }
            if (BeginWith(ToLower(iterator.name), command))
            {
                std::string msg;
                msg += "Showing help for command :" + iterator.name + "\n";
                msg += Telnet::yellow() + " Command   : " + Telnet::reset() + iterator.name + "\n";
                msg += Telnet::yellow() + " Level     : " + Telnet::reset() + ToString(iterator.level) + "\n";
                msg += Telnet::yellow() + " Arguments : " + Telnet::reset() + iterator.args + "\n";
                msg += Telnet::yellow() + " Help      : " + Telnet::reset() + iterator.help + "\n";
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
        player->sendMsg("Type " + Telnet::yellow() + "prompt help" + Telnet::reset() + " to read the guide.\n");
        return;
    }

    if (GetWords(prompt)[0] == "help")
    {
        player->sendMsg(Telnet::yellow() + "Prompt Help" + Telnet::reset() + "\n");
        player->sendMsg("You can set the prompt you prefer, respectfully to this constraints:\n");
        player->sendMsg(" - Not more than 15 characters.\n");
        player->sendMsg("\n");
        player->sendMsg("You can use the following shortcuts in you prompt:\n");
        player->sendMsg("    " + Telnet::italic() + "&n" + Telnet::reset() + " - Replace with player name.\n");
        player->sendMsg(
            "    " + Telnet::italic() + "&N" + Telnet::reset() + " - Replace with player name capitalized.\n");
        player->sendMsg(
            "    " + Telnet::italic() + "&h" + Telnet::reset() + " - Replace with player current health.\n");
        player->sendMsg("    " + Telnet::italic() + "&H" + Telnet::reset() + " - Replace with player max health.\n");
        player->sendMsg(
            "    " + Telnet::italic() + "&s" + Telnet::reset() + " - Replace with player current stamina.\n");
        player->sendMsg("    " + Telnet::italic() + "&S" + Telnet::reset() + " - Replace with player max stamina.\n");
        return;
    }
    player->prompt = prompt;
}

void DoTime(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);

    if (Mud::getInstance().getUpdater().mudDayPhase == 0)
    {
        character->sendMsg(Telnet::yellow() + "The sun has just risen.\n" + Telnet::reset());
    }
    else if (Mud::getInstance().getUpdater().mudDayPhase == 1)
    {
        character->sendMsg(Telnet::yellow() + "The sun is high in the sky.\n" + Telnet::reset());
    }
    else if (Mud::getInstance().getUpdater().mudDayPhase == 2)
    {
        character->sendMsg(Telnet::cyan() + "The sun is setting, the shadows begin to prevail.\n" + Telnet::reset());
    }
    else if (Mud::getInstance().getUpdater().mudDayPhase == 3)
    {
        character->sendMsg(Telnet::blue() + "The darkness surrounds you.\n" + Telnet::reset());
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
    msg += Telnet::magenta() + "Name: " + Telnet::reset() + player->getName();
    msg += Telnet::magenta() + " Race: " + Telnet::reset() + player->race->name + " (" + player->getSexAsString()
        + ")\n";
    msg += Telnet::magenta() + "Affiliation: " + Telnet::reset() + player->faction->name + "\n";
    msg += Telnet::magenta() + "Experience: " + Telnet::reset() + ToString(player->experience) + " px\n";
    msg += Telnet::magenta() + "Str " + Telnet::reset() + ToString(player->strength);
    msg += "(" + ToString(player->effects.getStrMod()) + ") | ";
    msg += Telnet::magenta() + "Agi " + Telnet::reset() + ToString(player->agility);
    msg += "(" + ToString(player->effects.getAgiMod()) + ") | ";
    msg += Telnet::magenta() + "Per " + Telnet::reset() + ToString(player->perception);
    msg += "(" + ToString(player->effects.getPerMod()) + ") | ";
    msg += Telnet::magenta() + "Con " + Telnet::reset() + ToString(player->constitution);
    msg += "(" + ToString(player->effects.getConMod()) + ") | ";
    msg += Telnet::magenta() + "Int " + Telnet::reset() + ToString(player->intelligence);
    msg += "(" + ToString(player->effects.getIntMod()) + ") \n";
    msg += Telnet::magenta() + "Health " + Telnet::reset() + ToString(player->health) + "/"
        + ToString(player->getMaxHealth());
    msg += "(" + ToString(player->effects.getHealthMod()) + ") ";
    msg += Telnet::magenta() + "Stamina " + Telnet::reset() + ToString(player->stamina) + "/"
        + ToString(player->getMaxStamina());
    msg += "(" + ToString(player->effects.getHealthMod()) + ")\n";
    msg += Telnet::magenta() + "Armor Class " + Telnet::reset() + ToString(player->getArmorClass()) + "\n";
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

    player->sendMsg("     ##    " + Telnet::yellow() + "LvL" + Telnet::green() + "    Skill" + Telnet::reset() + "\n");
    for (auto iterator : player->skills)
    {
        Skill * skill = Mud::getInstance().findSkill(iterator.first);
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
    character->sendMsg(
        "    Uptime      : " + ToString(difftime(time(NULL), Mud::getInstance().getBootTime())) + " s\n");
    character->sendMsg("\n");
    character->sendMsg("    Players     : " + ToString(Mud::getInstance().mudPlayers.size()) + "\n");
    character->sendMsg("    Mobiles     : " + ToString(Mud::getInstance().mudMobiles.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Models      : " + ToString(Mud::getInstance().mudModels.size()) + "\n");
    character->sendMsg("    Items       : " + ToString(Mud::getInstance().mudItems.size()) + "\n");
    character->sendMsg("    Corpses     : " + ToString(Mud::getInstance().mudCorpses.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Continents  : " + ToString(Mud::getInstance().mudContinents.size()) + "\n");
    character->sendMsg("    Areas       : " + ToString(Mud::getInstance().mudAreas.size()) + "\n");
    character->sendMsg("    Rooms       : " + ToString(Mud::getInstance().mudRooms.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Races       : " + ToString(Mud::getInstance().mudRaces.size()) + "\n");
    character->sendMsg("    Factions    : " + ToString(Mud::getInstance().mudFactions.size()) + "\n");
    character->sendMsg("    Skills      : " + ToString(Mud::getInstance().mudSkills.size()) + "\n");
    character->sendMsg("    Writings    : " + ToString(Mud::getInstance().mudWritings.size()) + "\n");
    character->sendMsg("    Materials   : " + ToString(Mud::getInstance().mudMaterials.size()) + "\n");
    character->sendMsg("    Professions : " + ToString(Mud::getInstance().mudProfessions.size()) + "\n");
    character->sendMsg("    Productions : " + ToString(Mud::getInstance().mudProductions.size()) + "\n");
    character->sendMsg("    Schematics  : " + ToString(Mud::getInstance().mudBuildings.size()) + "\n");
    character->sendMsg("    Liquids     : " + ToString(Mud::getInstance().mudLiquids.size()) + "\n");
    character->sendMsg("    News        : " + ToString(Mud::getInstance().mudNews.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Commands    : " + ToString(Mud::getInstance().mudCommands.size()) + "\n");

}
