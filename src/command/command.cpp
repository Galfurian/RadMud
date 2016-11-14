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
#include "command.hpp"
#include "creationStep.hpp"
#include "processNewAge.hpp"
#include "mud.hpp"

#include "combat.hpp"
#include "communication.hpp"
#include "crafting.hpp"
#include "general.hpp"
#include "god.hpp"
#include "manager.hpp"
#include "object.hpp"

Command::Command() :
    gods(),
    name(),
    help(),
    args(),
    hndl(),
    cuic()
{
    // Nothing to do.
}

Command::Command(bool _gods,
                 std::string _name,
                 std::string _help,
                 std::string _args,
                 ActionHandler _hndl,
                 bool _cuic) :
    gods(_gods),
    name(_name),
    help(_help),
    args(_args),
    hndl(_hndl),
    cuic(_cuic)
{
    // Nothing to do.
}

Command & Command::setGods(const bool & _gods)
{
    gods = _gods;
    return *this;
}

Command & Command::setName(const std::string & _name)
{
    name = _name;
    return *this;
}

Command & Command::setHelp(const std::string & _help)
{
    help = _help;
    return *this;
}

Command & Command::setArgs(const std::string & _args)
{
    args = _args;
    return *this;
}

Command & Command::setHndl(const ActionHandler & _hndl)
{
    hndl = _hndl;
    return *this;
}

Command & Command::setCuic(const bool & _cuic)
{
    cuic = _cuic;
    return *this;
}

bool Command::canUse(Character * character) const
{
    return (gods && HasFlag(character->flags, CharacterFlag::IsGod)) || (!gods);
}

void ProcessCommand(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Huh?\n");
        return;
    }
    auto command = args[0].getContent();
    args.erase(0);

    // Check if it's a direction.
    Direction direction = Mud::instance().findDirection(command, false);

    if (direction != Direction::None)
    {
        DoDirection(character, direction);
    }
    else
    {
        bool found = false;
        // Check if it's a command.
        for (auto iterator : Mud::instance().mudCommands)
        {
            if (!BeginWith(iterator.name, command))
            {
                continue;
            }
            if (!iterator.canUse(character))
            {
                continue;
            }
            if ((!iterator.cuic) && (character->getAction()->getType() == ActionType::Combat) && !iterator.gods)
            {
                character->sendMsg("You cannot do that in combat.\n");
                found = true;
                break;
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
                iterator.hndl(character, args);
                found = true;
                break;
            }
        }
        if (!found)
        {
            Profession * profession = Mud::instance().findProfession(command);
            if (profession != nullptr)
            {
                DoProfession(character, profession, args);
            }
            else
            {
                throw std::runtime_error("Huh?\n");
            }
        }
    }
    character->sendMsg("\n");
}

void NoMobile(Character * character)
{
    if (character->isMobile())
    {
        throw std::runtime_error("Npcs are not allowed to execute this command.\n");
    }
}

void StopAction(Character * character)
{
    if ((character->getAction()->getType() != ActionType::Wait))
    {
        if (character->getAction()->getType() != ActionType::Combat)
        {
            character->doCommand("stop");
        }
    }
}

void LoadStates()
{
    // Step 0 - Insert name.
    Mud::instance().addStateAction(ConnectionState::AwaitingName, ProcessPlayerName);
    // Step 1.a  - Insert the password.
    Mud::instance().addStateAction(ConnectionState::AwaitingPassword, ProcessPlayerPassword);
    // Step 1.b  - Choose the Name.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewName, ProcessNewName);
    // Step 2  - Choose the Password.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewPwd, ProcessNewPassword);
    // Step 3  - Confirm the Password.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewPwdCon, ProcessNewPasswordConfirm);
    // Step 4  - Short story of the mud world.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewStory, ProcessNewStory);
    // Step 5  - Choose the Race.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewRace, ProcessNewRace);
    // Step 6  - Choose the Attributes.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewAttr, ProcessNewAttributes);
    // Step 7  - Choose the Gender.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewGender, ProcessNewGender);
    // Step 8  - Choose the Age.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewAge, ProcessNewAge::process);
    // Step 9  - Choose the description (optional).
    Mud::instance().addStateAction(ConnectionState::AwaitingNewDesc, ProcessNewDescription);
    // Step 10 - Choose the Weight.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewWeight, ProcessNewWeight);
    // Step 11 - Confirm the character.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewConfirm, ProcessNewConfirm);

    // Playing.
    Mud::instance().addStateAction(ConnectionState::Playing, ProcessCommand);
}

void PrintChoices(Character * character)
{
    Player * player = character->toPlayer();
    std::string preview = std::string();
    preview += Formatter::clearScreen();
    preview += "# ------------ Character Creation ------------ #\n";

    // NAME
    preview += "# Name         :";
    if (!player->name.empty())
    {
        preview += player->name + Formatter::reset();
    }
    preview += "\n";

    // PASSWORD
    preview += "# Passowrd     :";
    if (!player->password.empty())
    {
        for (unsigned int i = 0; i < player->password.size(); i++)
            preview += "*";
    }
    preview += "\n";

    // RACE
    preview += "# Race         :";
    if (player->race != nullptr)
    {
        preview += player->race->name + Formatter::reset();
    }
    preview += "\n";

    // STRENGTH
    preview += "# Strength     :";
    if (player->getAbility(Ability::Strength, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Strength, false)) + Formatter::reset();
    }
    preview += "\n";

    // AGILITY
    preview += "# Agility      :";
    if (player->getAbility(Ability::Agility, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Agility, false)) + Formatter::reset();
    }
    preview += "\n";

    // PERCEPTION
    preview += "# Perception   :";
    if (player->getAbility(Ability::Perception, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Perception, false)) + Formatter::reset();
    }
    preview += "\n";

    // CONSTITUTION
    preview += "# Constitution :";
    if (player->getAbility(Ability::Constitution, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Constitution, false)) + Formatter::reset();
    }
    preview += "\n";

    // INTELLIGENCE
    preview += "# Intelligence :";
    if (player->getAbility(Ability::Intelligence, false) > 0)
    {
        preview += ToString(player->getAbility(Ability::Intelligence, false)) + Formatter::reset();
    }
    preview += "\n";

    // GENDER
    preview += "# Gender       :" + GetGenderTypeName(player->gender) + "\n";

    // AGE
    preview += "# Age          :";
    if (player->age > 0)
    {
        preview += ToString(player->age) + Formatter::reset();
    }
    preview += "\n";

    // DESCRIPTION
    preview += "# Description  :";
    if (!player->description.empty())
    {
        preview += player->description + Formatter::reset();
    }
    preview += "\n";

    // WEIGHT
    preview += "# Weight       :";
    if (player->weight > 0)
    {
        preview += ToString(player->weight) + Formatter::reset();
    }
    preview += "\n";
    preview += "# -------------------------------------------- #\n";
    player->sendMsg(preview);
}

void RollbackCharacterCreation(Character * character, ConnectionState new_state)
{
    Player * player = character->toPlayer();
    // Change the player connection state with the received argument.
    player->connection_state = new_state;
    if (new_state == ConnectionState::AwaitingNewName)
    {
        player->name = "";
    }
    else if (new_state == ConnectionState::AwaitingNewPwd)
    {
        player->password = "";
    }
    else if (new_state == ConnectionState::AwaitingNewPwdCon)
    {
        player->password = "";
    }
    else if (new_state == ConnectionState::AwaitingNewRace)
    {
        player->race = nullptr;
        player->setHealth(0);
        player->setStamina(0);
        player->setAbility(Ability::Strength, 0);
        player->setAbility(Ability::Agility, 0);
        player->setAbility(Ability::Perception, 0);
        player->setAbility(Ability::Constitution, 0);
        player->setAbility(Ability::Intelligence, 0);
        player->remaining_points = 0;
    }
    else if (new_state == ConnectionState::AwaitingNewAttr)
    {
        player->setAbility(Ability::Strength, player->race->getAbility(Ability::Strength));
        player->setAbility(Ability::Agility, player->race->getAbility(Ability::Agility));
        player->setAbility(Ability::Perception, player->race->getAbility(Ability::Perception));
        player->setAbility(Ability::Constitution, player->race->getAbility(Ability::Constitution));
        player->setAbility(Ability::Intelligence, player->race->getAbility(Ability::Intelligence));
    }
    else if (new_state == ConnectionState::AwaitingNewGender)
    {
        player->gender = GenderType::None;
    }
    else if (new_state == ConnectionState::AwaitingNewAge)
    {
        player->age = 0;
    }
    else if (new_state == ConnectionState::AwaitingNewDesc)
    {
        player->description = "";
    }
    else if (new_state == ConnectionState::AwaitingNewWeight)
    {
        player->weight = 0;
    }
    AdvanceCharacterCreation(player, new_state);
}

void AdvanceCharacterCreation(Character * character, ConnectionState new_state, std::string message)
{
    Player * player = character->toPlayer();
    // Change the player connection state with the received argument.
    player->connection_state = new_state;
    // The message that has to be sent.
    std::string msg;
    if (new_state == ConnectionState::Playing)
    {
        // Retrieve the saved prompt.
        player->prompt = player->prompt_save;
        // Entered the MUD.
        player->enterGame();
    }
    else if (new_state == ConnectionState::AwaitingName)
    {
        msg += Formatter::clearScreen();
        msg += "\nWelcome to RadMud!\n";
        msg += Formatter::red();
        msg += "#--------------------------------------------#\n";
        msg += "                 XXXXXXXXXXXXX                \n";
        msg += "      /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    \n";
        msg += "      \\##/#/#XXXXXXXX /O\\ XXXXXXXX#\\'\\##/ \n";
        msg += "       \\/#/#XXXXXXXXX \\O/ XXXXXXXXX#\\#\\/  \n";
        msg += "        \\/##XXXXXXXXXX   XXXXXXXXXX##\\/     \n";
        msg += "         ###XXXX  ''-.XXX.-''  XXXX###        \n";
        msg += "           \\XXXX               XXXX/         \n";
        msg += "             XXXXXXXXXXXXXXXXXXXXX            \n";
        msg += "             XXXX XXXX X XXXX XXXX            \n";
        msg += "             XXX # XXX X XXX # XXX            \n";
        msg += "            /XXXX XXX XXX XXX XXXX\\          \n";
        msg += "           ##XXXXXXX X   X XXXXXXX##          \n";
        msg += "          ##   XOXXX X   X XXXOX   ##         \n";
        msg += "          ##    #XXXX XXX XXX #    ##         \n";
        msg += "           ##..##  XXXXXXXXX  ##..##          \n";
        msg += "            ###      XXXXX     ####           \n";
        msg += "#--------------------------------------------#\n";
        msg += "| Created by : Enrico Fraccaroli.            |\n";
        msg += "| Date       : 21 Agosto 2014                |\n";
        msg += "#--------------------------------------------#\n";
        msg += Formatter::reset();
        msg += "# Enter your name, or type '" + Formatter::magenta() + "new" + Formatter::reset();
        msg += "' in order to create a new character!\n";
    }
    else if (new_state == ConnectionState::AwaitingPassword)
    {
        // Nothing to do.
    }
    else if (new_state == ConnectionState::AwaitingNewName)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Name." + Formatter::reset() + "\n";
        msg += "# Choose carefully, because this it's the only chance you have";
        msg +=
            " to pick a legendary name, maybe one day it will be whispered all over the lands.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the login.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewPwd)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Password." + Formatter::reset() + "\n";
        msg += "# Choose a proper password, in order to protect the acces to your character.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewPwdCon)
    {
        msg += Formatter::green() + "Re-enter the password.." + Formatter::reset() + "\n";
    }
    else if (new_state == ConnectionState::AwaitingNewStory)
    {
        PrintChoices(player);
        msg += "# The story of the Wasteland.\n";
        msg += "#\n";
        msg += "Year 374\n";
        msg += "#\n";
        msg += "# Type [" + Formatter::magenta() + "continue" + Formatter::reset()
               + "] to continue character creation.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewRace)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Race." + Formatter::reset() + "\n";
        for (auto iterator : Mud::instance().mudRaces)
        {
            Race * race = iterator.second;
            if (race->player_allow)
            {
                msg += "#    [" + ToString(race->vnum) + "] " + race->name + ".\n";
            }
        }
        msg += "#\n";
        msg += "# Choose one of the above race by typing the correspondent number.\n";
        msg += "#\n";
        msg += "# Type [" + Formatter::magenta() + "help [Number]" + Formatter::reset() + "]";
        msg += " to read a brief description of the race.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "]";
        msg += " to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewAttr)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Attributes." + Formatter::reset() + "\n";
        msg += "#    [1] Strength     :" + ToString(player->getAbility(Ability::Strength, false))
               + "\n";
        msg += "#    [2] Agility      :" + ToString(player->getAbility(Ability::Agility, false))
               + "\n";
        msg += "#    [3] Perception   :" + ToString(player->getAbility(Ability::Perception, false))
               + "\n";
        msg += "#    [4] Constitution :"
               + ToString(player->getAbility(Ability::Constitution, false)) + "\n";
        msg += "#    [5] Intelligence :"
               + ToString(player->getAbility(Ability::Intelligence, false)) + "\n";
        msg += "#\n";
        msg += "# Remaining Points: " + Formatter::green() + ToString(player->remaining_points)
               + Formatter::reset() + "\n";
        msg += "#\n";
        msg += "# Type [" + Formatter::magenta() + "(number) +/-modifier" + Formatter::reset()
               + "]";
        msg += " to decrease or increase the value of an attribute.\n";
        msg += "# Type [" + Formatter::magenta() + "help (number)" + Formatter::reset() + "]";
        msg += " to read a brief description of the attribute.\n";
        msg += "# Type [" + Formatter::magenta() + "reset" + Formatter::reset() + "]";
        msg += " to reset the values as default.\n";
        msg += "# Type [" + Formatter::magenta() + "continue" + Formatter::reset() + "]";
        msg += " to continue character creation.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "]";
        msg += " to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewGender)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Gender." + Formatter::reset() + "\n";
        msg += "#    [1] Male.\n";
        msg += "#    [2] Female.\n";
        msg += "#\n";
        msg += "# Choose one of the above gender by typing the correspondent number.\n";
        msg += "#\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewAge)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Age." + Formatter::reset() + "\n";
        msg +=
            "# Choose the starting age of your character, be aware that during the game the time will pass.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewDesc)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Description." + Formatter::reset() + "\n";
        msg += "# Insert a brief description of your character, its optional.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the previus step.\n";
        msg += "# Type [" + Formatter::magenta() + "skip" + Formatter::reset()
               + "] to just pass to the next step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewWeight)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Weight." + Formatter::reset() + "\n";
        msg += "# Choose the wheight of your character.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewConfirm)
    {
        PrintChoices(player);
        msg += "# Give a look to the information you have provided, now it's the right time";
        msg += " to decide if you want to change something.\n";
        msg += "# Type [" + Formatter::magenta() + "confirm" + Formatter::reset()
               + "] to conclude the character creation.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset()
               + "]    to return to the previus step.\n";
        msg += Formatter::green() + "Do you confirm? " + Formatter::reset() + "\n";
    }
    if (!message.empty())
    {
        msg += "# " + message + "\n";
    }
    player->sendMsg(msg);
}

void LoadCommands()
{
    // Beware the order is important.
    // Changing the order will not compromise the correct execution of the Mud, but...
    // If a player just types 'l' or 'lo' in order to 'look' the first commant which get hit is the 'load' command.
    LoadGeneralCommands();
    LoadObjectCommands();
    LoadCommunicationCommands();
    LoadCraftingCommands();
    LoadCombatCommands();
    LoadManagerCommands();
    LoadGodCommands();
}
