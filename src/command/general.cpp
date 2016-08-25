/// @file   general.cpp
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

#include "command.hpp"
// Other Include.
#include "../mud.hpp"
#include "../constants.hpp"
#include "../action/moveAction.hpp"
#include "../utilities/table.hpp"
#include "../utilities/logger.hpp"

#include "argumentHandler.hpp"

using namespace std;

void DoDirection(Character * character, Direction direction)
{
    // Check if the player it's already doing something.
    StopAction(character);

    std::string error;
    if (!character->canMoveTo(direction, error))
    {
        character->sendMsg(error + "\n");
        return;
    }

    // ////////////////////////////////////////////////////////////////////////
    unsigned int speed;
    // Calculate the time needed to move.
    if (character->posture == CharacterPosture::Stand)
    {
        character->sendMsg("You start to go %s...\n", direction.toString());
        speed = 2;
    }
    else if (character->posture == CharacterPosture::Crouch)
    {
        character->sendMsg("You move crouching towards %s...\n", direction.toString());
        speed = 4;
    }
    else if (character->posture == CharacterPosture::Prone)
    {
        character->sendMsg("You begin to crawl to %s...\n", direction.toString());
        speed = 6;
    }
    else
    {
        character->sendMsg("You can't move!\n");
        return;
    }

    std::shared_ptr<MoveAction> moveAction = std::make_shared<MoveAction>(
        character,
        character->room->findExit(direction)->destination,
        direction,
        speed);
    // Check the new action.
    if (!moveAction->check())
    {
        character->sendMsg("You can't move.\n");
        return;
    }
    // Set the new action.
    character->setAction(moveAction);
}

void DoTravel(Character * character, ArgumentHandler & /*args*/)
{
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

void DoQuit(Character * character, ArgumentHandler & /*args*/)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
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
            // Send the message inside the room.
            player->room->sendToAll(
                "Player %s disappear in a puff of smoke.\n",
                { character },
                player->getName());
        }

        Logger::log(LogLevel::Global, "Player %s has left the game.", player->getName());

        // End of properly connected.
        player->closeConnection();
    }
}

void DoWho(Character * character, ArgumentHandler & /*args*/)
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
        string location("Nowhere");
        if (iterator->room != nullptr)
        {
            location = iterator->room->name;
        }
        table.addRow( { iterator->getName(), location });
    }
    output += table.getTable();
    output += "# Total " + Formatter::yellow() + "Players" + Formatter::reset() + " :"
        + ToString(table.getNumRows()) + "\n";
    character->sendMsg(output);
}

void DoSet(Character * character, ArgumentHandler & args)
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
            }
        }
    }
    character->sendMsg("Usage: set <opt> <arguments>  \n");
    character->sendMsg("       ---------------------  \n");
    character->sendMsg("            des  [description]\n");
}

void DoStop(Character * character, ArgumentHandler & /*args*/)
{
    if (character->getAction()->getType() != ActionType::Combat)
    {
        character->sendMsg(character->getAction()->stop() + "\n");
        character->popAction();
    }
}

void DoLook(Character * character, ArgumentHandler & args)
{
    // If there are no arguments, show the room.
    if (args.empty())
    {
        character->sendMsg(character->room->getLook(character));
    }
    else if (args.size() == 1)
    {
        Character * target = character->room->findCharacter(
            args[0].getContent(),
            args[0].getIndex(),
            { character });
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
                return;
            }
        }
        Item * item = character->findNearbyItem(args[0].getContent(), args[0].getIndex());
        if (item)
        {
            character->sendMsg(item->getLook());
        }
        else
        {
            character->sendMsg("You don't see '%s' anywhere.\n", args[0].getContent());
        }
    }
    else if (args.size() == 2)
    {
        Character * target = character->room->findCharacter(
            args[1].getContent(),
            args[1].getIndex(),
            { character });
        Item * container = character->findNearbyItem(args[1].getContent(), args[1].getIndex());
        if (target)
        {
            if (character->canSee(target))
            {
                Item * item = target->findEquipmentItem(args[0].getContent(), args[0].getIndex());
                if (item)
                {
                    character->sendMsg(item->getLook());
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
                Item * item = container->findContent(args[0].getContent(), args[0].getIndex());
                if (item)
                {
                    character->sendMsg(item->getLook());
                }
                else
                {
                    character->sendMsg("It's not insiede %s.\n", container->getName());
                }
            }
            else
            {
                character->sendMsg("%s is not a container.\n", container->getNameCapital());
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
}

void DoHelp(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        size_t numColumns = 4;
        Table commandTable = Table("Commands");
        for (size_t it = 0; it < numColumns; ++it)
        {
            commandTable.addColumn("", StringAlign::Left);
        }

        TableRow row;
        for (auto it : Mud::instance().mudCommands)
        {
            if (it.level == 0)
            {
                if (it.canUse(character))
                {
                    row.push_back(it.name);
                }
                if (row.size() == numColumns)
                {
                    commandTable.addRow(row);
                    row.clear();
                }
            }
        }
        for (auto it : Mud::instance().mudCommands)
        {
            if (it.level > 0)
            {
                if (it.canUse(character))
                {
                    row.push_back(it.name);
                }
                if (row.size() == numColumns)
                {
                    commandTable.addRow(row);
                    row.clear();
                }
            }
        }
        character->sendMsg(commandTable.getTable(true));
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
                        + ToString(iterator.level) + "\n";
                    msg += Formatter::yellow() + " Arguments : " + Formatter::reset()
                        + iterator.args + "\n";
                    msg += Formatter::yellow() + " Help      : " + Formatter::reset()
                        + iterator.help + "\n";
                    character->sendMsg(msg);
                    return;
                }
            }
        }
    }
    character->sendMsg("There is no help for '%s'.\n", args.getOriginal());
}

void DoPrompt(Character * character, ArgumentHandler & args)
{
    NoMobile(character);
    Player * player = character->toPlayer();
    if (args.empty())
    {
        character->sendMsg("Current prompt:\n");
        character->sendMsg("    %s\n", player->prompt);
        character->sendMsg(
            "Type %sprompt help %sto read the guide.\n",
            Formatter::yellow(),
            Formatter::reset());
        return;
    }
    else if (args[0].getContent() == "help")
    {
        character->sendMsg(Formatter::yellow() + "Prompt Help" + Formatter::reset() + "\n");
        character->sendMsg(
            "You can set the prompt you prefer, respectfully to this constraints:\n");
        character->sendMsg(" - Not more than 15 characters.\n");
        character->sendMsg("\n");
        character->sendMsg("You can use the following shortcuts in you prompt:\n");
        character->sendMsg("    %s&n%s - Player name.\n", Formatter::italic(), Formatter::reset());
        character->sendMsg(
            "    %s&N%s - Player name capitalized.\n",
            Formatter::italic(),
            Formatter::reset());
        character->sendMsg(
            "    %s&h%s - Player current health.\n",
            Formatter::italic(),
            Formatter::reset());
        character->sendMsg(
            "    %s&H%s - Player max health.\n",
            Formatter::italic(),
            Formatter::reset());
        character->sendMsg(
            "    %s&s%s - Player current stamina.\n",
            Formatter::italic(),
            Formatter::reset());
        character->sendMsg(
            "    %s&S%s - Player max stamina.\n",
            Formatter::italic(),
            Formatter::reset());
        return;
    }
    player->prompt = args.getOriginal();
}

void DoTime(Character * character, ArgumentHandler & /*args*/)
{
    if (MudUpdater::instance().getDayPhase() == DayPhase::Morning)
    {
        character->sendMsg(
            "%sThe sun has just risen.%s\n",
            Formatter::yellow(),
            Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Day)
    {
        character->sendMsg(
            "%sThe sun is high in the sky.%s\n",
            Formatter::yellow(),
            Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Dusk)
    {
        character->sendMsg(
            "%sThe sun is setting, the shadows begin to prevail.%s\n",
            Formatter::cyan(),
            Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Night)
    {
        character->sendMsg(
            "%sThe darkness surrounds you.%s\n",
            Formatter::blue(),
            Formatter::reset());
    }
}

void DoStand(Character * character, ArgumentHandler & /*args*/)
{
    if (character->posture == CharacterPosture::Stand)
    {
        character->sendMsg("You are already standing.\n");
        return;
    }
    character->posture = CharacterPosture::Stand;
    character->sendMsg("You stand up.\n");
}

void DoCrouch(Character * character, ArgumentHandler & /*args*/)
{
    if (character->posture == CharacterPosture::Crouch)
    {
        character->sendMsg("You are already crouched.\n");
        return;
    }
    character->posture = CharacterPosture::Crouch;
    character->sendMsg("You put yourself crouched.\n");
}

void DoSit(Character * character, ArgumentHandler & /*args*/)
{
    if (character->posture == CharacterPosture::Sit)
    {
        character->sendMsg("You are already seated.\n");
        return;
    }
    character->posture = CharacterPosture::Sit;
    character->sendMsg("You sit down.\n");
}

void DoProne(Character * character, ArgumentHandler & /*args*/)
{
    if (character->posture == CharacterPosture::Prone)
    {
        character->sendMsg("You are already prone.\n");
        return;
    }
    character->posture = CharacterPosture::Prone;
    character->sendMsg("You put yourself prone.\n");
}

void DoRest(Character * character, ArgumentHandler & /*args*/)
{
    if (character->posture == CharacterPosture::Rest)
    {
        character->sendMsg("You are already resting.\n");
        return;
    }
    character->posture = CharacterPosture::Rest;
    character->sendMsg("You lie down.\n");
}

void DoStatistics(Character * character, ArgumentHandler & /*args*/)
{
    NoMobile(character);
    Player * player = character->toPlayer();

    std::string msg;
    msg += Formatter::magenta() + "Name: " + Formatter::reset() + player->getName() + " ";
    msg += Formatter::magenta() + "Race: " + Formatter::reset() + player->race->name + "\n";
    msg += Formatter::magenta() + "Gender: " + Formatter::reset()
        + GetGenderTypeName(player->gender) + "\n";
    msg += Formatter::magenta() + "Affiliation: " + Formatter::reset() + player->faction->name
        + "\n";
    msg += Formatter::magenta() + "Experience: " + Formatter::reset() + ToString(player->experience)
        + " px\n";

    msg += Formatter::magenta() + "    Str " + Formatter::reset();
    msg += ToString(player->getAbility(Ability::Strength, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Strength)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Strength)) + "]\n";

    msg += Formatter::magenta() + "    Agi " + Formatter::reset();
    msg += ToString(player->getAbility(Ability::Agility, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Agility)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Agility)) + "]\n";

    msg += Formatter::magenta() + "    Per " + Formatter::reset();
    msg += ToString(player->getAbility(Ability::Perception, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Perception)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Perception)) + "]\n";

    msg += Formatter::magenta() + "    Con " + Formatter::reset();
    msg += ToString(player->getAbility(Ability::Constitution, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Constitution)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Constitution)) + "]\n";

    msg += Formatter::magenta() + "    Int " + Formatter::reset();
    msg += ToString(player->getAbility(Ability::Intelligence, false));
    msg += "(" + ToString(player->effects.getAbilityModifier(Ability::Intelligence)) + ")";
    msg += "[" + ToString(player->getAbilityModifier(Ability::Intelligence)) + "]\n";

    msg += Formatter::magenta() + "    Health " + Formatter::reset();
    msg += ToString(player->getHealth()) + "/" + ToString(player->getMaxHealth());
    msg += "(" + ToString(player->effects.getHealthMod()) + ")\n";

    msg += Formatter::magenta() + "    Stamina " + Formatter::reset();
    msg += ToString(player->getStamina()) + "/" + ToString(player->getMaxStamina());
    msg += "(" + ToString(player->effects.getHealthMod()) + ")\n";

    msg += Formatter::magenta() + "    Armor Class " + Formatter::reset();
    msg += ToString(player->getArmorClass()) + "\n";

    msg += "You " + player->getHealthCondition(true) + ".\n";
    msg += "You " + player->getStaminaCondition() + ".\n";
    msg += "You " + player->getHungerCondition() + ".\n";
    msg += "You " + player->getThirstCondition() + ".\n";

    msg += "You are " + GetPostureName(player->posture) + ".\n";

    player->sendMsg(msg);
}

void DoRent(Character * character, ArgumentHandler & /*args*/)
{
    NoMobile(character);
    Player * player = character->toPlayer();
    // Get the current room.
    Room * room = player->room;
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

void DoSkills(Character * character, ArgumentHandler & /*args*/)
{
    NoMobile(character);
    Player * player = character->toPlayer();
    Table table = Table();
    table.addColumn("LvL", StringAlign::Left);
    table.addColumn("Skill", StringAlign::Left);
    for (auto iterator : player->skills)
    {
        Skill * skill = Mud::instance().findSkill(iterator.first);
        if (skill)
        {
            table.addRow( { skill->name, ToString(iterator.second) });
        }
    }
    character->sendMsg(table.getTable());
}

void DoServer(Character * character, ArgumentHandler & /*args*/)
{
    character->sendMsg("    Mud         : RadMud.\n");
    character->sendMsg("    Version     : " + kVersion + "\n");
    character->sendMsg("    Uptime      : " + ToString(Mud::instance().getUpTime()) + " s\n");
    character->sendMsg("\n");
    character->sendMsg("    Players     : " + ToString(Mud::instance().mudPlayers.size()) + "\n");
    character->sendMsg("    Mobiles     : " + ToString(Mud::instance().mudMobiles.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg(
        "    Models      : " + ToString(Mud::instance().mudItemModels.size()) + "\n");
    character->sendMsg("    Items       : " + ToString(Mud::instance().mudItems.size()) + "\n");
    character->sendMsg("    Corpses     : " + ToString(Mud::instance().mudCorpses.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg(
        "    Continents  : " + ToString(Mud::instance().mudContinents.size()) + "\n");
    character->sendMsg("    Areas       : " + ToString(Mud::instance().mudAreas.size()) + "\n");
    character->sendMsg("    Rooms       : " + ToString(Mud::instance().mudRooms.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Races       : " + ToString(Mud::instance().mudRaces.size()) + "\n");
    character->sendMsg("    Factions    : " + ToString(Mud::instance().mudFactions.size()) + "\n");
    character->sendMsg("    Skills      : " + ToString(Mud::instance().mudSkills.size()) + "\n");
    character->sendMsg("    Writings    : " + ToString(Mud::instance().mudWritings.size()) + "\n");
    character->sendMsg("    Materials   : " + ToString(Mud::instance().mudMaterials.size()) + "\n");
    character->sendMsg(
        "    Professions : " + ToString(Mud::instance().mudProfessions.size()) + "\n");
    character->sendMsg(
        "    Productions : " + ToString(Mud::instance().mudProductions.size()) + "\n");
    character->sendMsg("    Schematics  : " + ToString(Mud::instance().mudBuildings.size()) + "\n");
    character->sendMsg("    Liquids     : " + ToString(Mud::instance().mudLiquids.size()) + "\n");
    character->sendMsg("    News        : " + ToString(Mud::instance().mudNews.size()) + "\n");
    character->sendMsg("\n");
    character->sendMsg("    Commands    : " + ToString(Mud::instance().mudCommands.size()) + "\n");

}
