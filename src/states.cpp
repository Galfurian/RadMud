/// @file   states.cpp
/// @brief  Implements methods that allow to handle player states.
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
#include "states.hpp"

#include <stdexcept>
#include <fstream>

#include "commands.hpp"
#include "mud.hpp"
// Other Include.
#include "utils.hpp"

using namespace std;

/// Player names must consist of characters from this list.
const std::string kValidPlayerName = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
/// Player descriptions must consist of characters from this list.
const std::string kValidDescription = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,.\n";

void LoadStates()
{
    // Step 0 - Insert name.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingName, ProcessPlayerName);
    // Step 1.a  - Insert the password.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingPassword, ProcessPlayerPassword);
    // Step 1.b  - Choose the Name.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewName, ProcessNewName);
    // Step 2  - Choose the Password.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewPwd, ProcessNewPwd);
    // Step 3  - Confirm the Password.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewPwdCon, ProcessNewPwdCon);
    // Step 4  - Short story of the mud world.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewStory, ProcessNewStory);
    // Step 5  - Choose the Race.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewRace, ProcessNewRace);
    // Step 6  - Choose the Attributes.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewAttr, ProcessNewAttr);
    // Step 7  - Choose the Gender.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewGender, ProcessNewGender);
    // Step 8  - Choose the Age.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewAge, ProcessNewAge);
    // Step 9  - Choose the description (optional).
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewDesc, ProcessNewDesc);
    // Step 10 - Choose the Weight.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewWeight, ProcessNewWeight);
    // Step 11 - Confirm the character.
    Mud::getInstance().addStateAction(ConnectionState::AwaitingNewConfirm, ProcessNewConfirm);

    // Playing.
    Mud::getInstance().addStateAction(ConnectionState::Playing, ProcessCommand);
}

void PrintChoices(Character * character)
{
    Player * player = character->toPlayer();
    std::string preview = std::string();
    preview += Telnet::clearScreen();
    preview += "# ------------ Character Creation ------------ #\n";

    // NAME
    preview += "# Name         :";
    if (!player->name.empty())
    {
        preview += player->name + Telnet::reset();
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
        preview += player->race->name + Telnet::reset();
    }
    preview += "\n";

    // STRENGTH
    preview += "# Strength     :";
    if (player->strength > 0)
    {
        preview += ToString(player->strength) + Telnet::reset();
    }
    preview += "\n";

    // AGILITY
    preview += "# Agility      :";
    if (player->agility > 0)
    {
        preview += ToString(player->agility) + Telnet::reset();
    }
    preview += "\n";

    // PERCEPTION
    preview += "# Perception   :";
    if (player->perception > 0)
    {
        preview += ToString(player->perception) + Telnet::reset();
    }
    preview += "\n";

    // CONSTITUTION
    preview += "# Constitution :";
    if (player->constitution > 0)
    {
        preview += ToString(player->constitution) + Telnet::reset();
    }
    preview += "\n";

    // INTELLIGENCE
    preview += "# Intelligence :";
    if (player->intelligence > 0)
    {
        preview += ToString(player->intelligence) + Telnet::reset();
    }
    preview += "\n";

    // SEX
    preview += "# Sex          :" + player->getSexAsString() + "\n";

    // AGE
    preview += "# Age          :";
    if (player->age > 0)
    {
        preview += ToString(player->age) + Telnet::reset();
    }
    preview += "\n";

    // DESCRIPTION
    preview += "# Description  :";
    if (!player->description.empty())
    {
        preview += player->description + Telnet::reset();
    }
    preview += "\n";

    // WEIGHT
    preview += "# Weight       :";
    if (player->weight > 0)
    {
        preview += ToString(player->weight) + Telnet::reset();
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
    switch (new_state)
    {
        case ConnectionState::NoState:
            break;
        case ConnectionState::AwaitingName:
            break;
        case ConnectionState::AwaitingPassword:
            break;
        case ConnectionState::AwaitingNewName:
            player->name = "";
            break;
        case ConnectionState::AwaitingNewPwd:
            player->password = "";
            break;
        case ConnectionState::AwaitingNewPwdCon:
            player->password = "";
            break;
        case ConnectionState::AwaitingNewStory:
            break;
        case ConnectionState::AwaitingNewRace:
            player->race = nullptr;
            player->health = 0;
            player->stamina = 0;
            player->strength = 0;
            player->agility = 0;
            player->perception = 0;
            player->constitution = 0;
            player->intelligence = 0;
            player->remaining_points = 0;
            break;
        case ConnectionState::AwaitingNewAttr:
            player->strength = player->race->strength;
            player->agility = player->race->agility;
            player->perception = player->race->perception;
            player->constitution = player->race->constitution;
            player->intelligence = player->race->intelligence;
            break;
        case ConnectionState::AwaitingNewGender:
            player->sex = 0;
            break;
        case ConnectionState::AwaitingNewAge:
            player->age = 0;
            break;
        case ConnectionState::AwaitingNewDesc:
            player->description = "";
            break;
        case ConnectionState::AwaitingNewWeight:
            player->weight = 0;
            break;
        case ConnectionState::Playing:
        case ConnectionState::AwaitingNewConfirm:
            player->sendMsg("It seems that this choiche has not been handled...");
            player->closeConnection();
            break;
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
        msg += Telnet::clearScreen();
        msg += "\nWelcome to RadMud. Version " + Telnet::green() + kVersion + Telnet::reset() + "!\n";
        msg += Telnet::red();
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
        msg += Telnet::reset();
        msg += "# Enter your name, or type '" + Telnet::magenta() + "new" + Telnet::reset();
        msg += "' in order to create a new character!\n";
    }
    else if (new_state == ConnectionState::AwaitingPassword)
    {
        // Nothing to do.
    }
    else if (new_state == ConnectionState::AwaitingNewName)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Name." + Telnet::reset() + "\n";
        msg += "# Choose carefully, because this it's the only chance you have";
        msg += " to pick a legendary name, maybe one day it will be whispered all over the lands.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the login.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewPwd)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Password." + Telnet::reset() + "\n";
        msg += "# Choose a proper password, in order to protect the acces to your character.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewPwdCon)
    {
        msg += Telnet::green() + "Re-enter the password.." + Telnet::reset();
    }
    else if (new_state == ConnectionState::AwaitingNewStory)
    {
        PrintChoices(player);
        msg += "# The story of the Wasteland.\n";
        msg += "#\n";
        msg += "Year 374\n";
        msg += "#\n";
        msg += "# Type [" + Telnet::magenta() + "continue" + Telnet::reset() + "] to continue character creation.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewRace)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Race." + Telnet::reset() + "\n";
        for (auto iterator : Mud::getInstance().mudRaces)
        {
            Race * race = &iterator.second;
            if (race->player_allow) msg += "#    [" + ToString(race->vnum) + "] " + race->name + ".\n";
        }
        msg += "#\n";
        msg += "# Choose one of the above race by typing the correspondent number.\n";
        msg += "#\n";
        msg += "# Type [" + Telnet::magenta() + "help [Number]" + Telnet::reset() + "]";
        msg += " to read a brief description of the race.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "]";
        msg += " to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewAttr)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Attributes." + Telnet::reset() + "\n";
        msg += "#    [1] Strength     :" + ToString(player->strength) + "\n";
        msg += "#    [2] Agility      :" + ToString(player->agility) + "\n";
        msg += "#    [3] Perception   :" + ToString(player->perception) + "\n";
        msg += "#    [4] Constitution :" + ToString(player->constitution) + "\n";
        msg += "#    [5] Intelligence :" + ToString(player->intelligence) + "\n";
        msg += "#\n";
        msg += "# Remaining Points: " + Telnet::green() + ToString(player->remaining_points) + Telnet::reset() + "\n";
        msg += "#\n";
        msg += "# Type [" + Telnet::magenta() + "(number) +/-modifier" + Telnet::reset() + "]";
        msg += " to decrease or increase the value of an attribute.\n";
        msg += "# Type [" + Telnet::magenta() + "help (number)" + Telnet::reset() + "]";
        msg += " to read a brief description of the attribute.\n";
        msg += "# Type [" + Telnet::magenta() + "reset" + Telnet::reset() + "]";
        msg += " to reset the values as default.\n";
        msg += "# Type [" + Telnet::magenta() + "continue" + Telnet::reset() + "]";
        msg += " to continue character creation.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "]";
        msg += " to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewGender)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Gender." + Telnet::reset() + "\n";
        msg += "#    [1] Male.\n";
        msg += "#    [2] Female.\n";
        msg += "#\n";
        msg += "# Choose one of the above sex by typing the correspondent number.\n";
        msg += "#\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewAge)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Age." + Telnet::reset() + "\n";
        msg += "# Choose the starting age of your character, be aware that during the game the time will pass.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewDesc)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Description." + Telnet::reset() + "\n";
        msg += "# Insert a brief description of your character, its optional.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the previus step.\n";
        msg += "# Type [" + Telnet::magenta() + "skip" + Telnet::reset() + "] to just pass to the next step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewWeight)
    {
        PrintChoices(player);
        msg += "# " + Telnet::bold() + "Character's Weight." + Telnet::reset() + "\n";
        msg += "# Choose the wheight of your character.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewConfirm)
    {
        PrintChoices(player);
        msg += "# Give a look to the information you have provided, now it's the right time";
        msg += " to decide if you want to change something.\n";
        msg += "# Type [" + Telnet::magenta() + "confirm" + Telnet::reset() + "] to conclude the character creation.\n";
        msg += "# Type [" + Telnet::magenta() + "back" + Telnet::reset() + "]    to return to the previus step.\n";
        msg += Telnet::green() + "Do you confirm? " + Telnet::reset();
    }

    if (!message.empty())
    {
        msg += "# " + message + "\n";
    }

    player->sendMsg(msg);
}

void ProcessPlayerName(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);

    try
    {
        // Name can't be blank.
        if (input.empty())
        {
            throw runtime_error("Name cannot be blank.");
        }
        // Check if the user want to quit.
        if (input == "quit")
        {
            player->closeConnection();
        }
        // Check if the give name contains valid characters.
        if (input.find_first_not_of(kValidPlayerName) != string::npos)
        {
            throw runtime_error("That player name contains disallowed characters.");
        }
        // Check if the player is already connected.
        if (Mud::getInstance().findPlayer(input))
        {
            throw runtime_error(input + " is already connected.");
        }
        // Check if the player has typed new.
        if (ToLower(input) == "new")
        {
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewName);
            return;
        }
        // Check if the player exists in the Database.
        if (!Mud::getInstance().getDbms().searchPlayer(ToCapitals(input)))
        {
            throw runtime_error("That player doesen't exist.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName, e.what());
        return;
    }

    // Save the name of the player.
    player->name = ToCapitals(input);
    // Load player so we know the player_password etc.
    if (player->loadFromDB())
    {
        // Delete the loaded prompt, otherwise it will be shown.
        player->prompt = "";
        // Disable echo of command client side.
        //msg +=(const char*) echo_off_str);
        // Set to 0 the current password attempts.
        player->password_attempts = 0;
        player->sendMsg("Username is correct, now insert the password.\n");
        AdvanceCharacterCreation(character, ConnectionState::AwaitingPassword);
    }
    else
    {
        player->closeConnection();
    }
}
void ProcessPlayerPassword(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);

    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Password cannot be blank.");
        }
        // Check the correctness of the password.
        if (input != player->password)
        {
            throw runtime_error("That password is incorrect.");
        }
    }
    catch (runtime_error & e)
    {
        // Detect too many password attempts.
        if (++player->password_attempts >= kMaxPasswordAttempts)
        {
            player->closeConnection();
            player->sendMsg("Goodbye!\n");
            player->sendMsg("Too many attempts to guess the password!\n");
        }
        else
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingPassword, e.what());
        }
        return;
    }

    AdvanceCharacterCreation(character, ConnectionState::Playing);
}
void ProcessNewName(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);

    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingName);
            return;
        }

        // Check if the player has given bad characters.
        if (input.find_first_not_of(kValidPlayerName) != string::npos)
        {
            throw runtime_error("That player name contains disallowed characters.");
        }
        // Check for bad names here.
        if (Mud::getInstance().badNames.find(input) != Mud::getInstance().badNames.end())
        {
            throw runtime_error("That name is not permitted.");
        }
        // Check if the player name has already been used.
        if (Mud::getInstance().getDbms().searchPlayer(ToCapitals(input)))
        {
            throw runtime_error("That player already exists, please choose another name.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewName, e.what());
        return;
    }

    player->name = ToCapitals(input);
    player->room = Mud::getInstance().findRoom(1000);
    player->faction = Mud::getInstance().findFaction(1);
    player->prompt_save = "[" + player->name + "]";
    player->password_attempts = 0;

    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwd);
}
void ProcessNewPwd(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);

    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewName);
            return;
        }

        // Check if the player has given bad characters.
        if (input.find_first_not_of(kValidPlayerName) != string::npos)
        {
            throw runtime_error("Password cannot contain disallowed characters.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewPwd, e.what());
        return;
    }

    player->password = input;

    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwdCon);
}
void ProcessNewPwdCon(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Player_password must agree.
        if (input != player->password)
        {
            throw runtime_error("Password and confirmation do not agree.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewPwdCon, e.what());
        return;
    }
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewStory);
}
void ProcessNewStory(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewPwd);
            return;
        }

        // Check if the player has written 'continue' or NOT.
        if (input != "continue")
        {
            throw runtime_error("Write 'continue' after you have readed the story.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewStory, e.what());
        return;
    }
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewRace);
}
void ProcessNewRace(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    Race * race;
    string input;
    string help_msg = "";
    getline(sArgs, input);
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewStory);
            return;
        }
        // If the player has insert help (race_number), show its help.
        if (BeginWith(ToLower(input), "help"))
        {
            // Get the race number.
            vector<string> args = GetWords(input);
            int choice = atoi(args[1].c_str());

            // Check for errors.
            if ((race = Mud::getInstance().findRace(choice)) == nullptr)
            {
                throw runtime_error("No help for that race.");
            }
            if (!race->player_allow)
            {
                throw runtime_error("No help for that race.");
            }
            help_msg += "Help about " + race->name + ".\n";
            help_msg += "Strength     " + ToString(race->strength) + ".\n";
            help_msg += "Agility      " + ToString(race->agility) + ".\n";
            help_msg += "Perception   " + ToString(race->perception) + ".\n";
            help_msg += "Constitution " + ToString(race->constitution) + ".\n";
            help_msg += "Intelligence " + ToString(race->intelligence) + ".\n";
            help_msg += Telnet::italic() + race->description + Telnet::reset() + "\n";
            throw runtime_error(help_msg);
        }
        else if (IsNumber(input))
        {
            int choice = atoi(input.c_str());
            if ((race = Mud::getInstance().findRace(choice)) == nullptr)
            {
                throw runtime_error("Not a valid race.");
            }
            if (!race->player_allow)
            {
                throw runtime_error("Not a valid race.");
            }
        }
        else
        {
            throw runtime_error("Not a valid race.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, e.what());
        return;
    }
    player->race = race;
    player->health = race->constitution * 5;
    player->stamina = race->constitution * 10;
    player->strength = race->strength;
    player->agility = race->agility;
    player->perception = race->perception;
    player->constitution = race->constitution;
    player->intelligence = race->intelligence;
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewAttr);
}
void ProcessNewAttr(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    string help_msg;
    getline(sArgs, input);
    try
    {
        // Attribute can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewRace);
            return;
        }
        // If the player has insert help (race_number), show its help.
        else if (BeginWith(ToLower(input), "help"))
        {
            // Get the race number.
            vector<string> args = GetWords(input);
            int choice = atoi(args[1].c_str());

            help_msg += Telnet::cyan();
            switch (choice)
            {
                case 1:
                    help_msg += "Help about Strength.\n" + Telnet::italic();
                    help_msg += "Strength is important for increasing the Carrying Weight and ";
                    help_msg += "satisfying the minimum Strength requirements for some weapons and armors.";
                    help_msg += Telnet::reset() + "\n";
                    break;
                case 2:
                    help_msg += "Help about Agility.\n" + Telnet::italic();
                    help_msg += "Besides increasing mobility in combat, it increases the recharge ";
                    help_msg += "speed of all the weapons, as well as the ability to use light armor.";
                    help_msg += Telnet::reset() + "\n";
                    break;
                case 3:
                    help_msg += "Help about Perception.\n" + Telnet::italic();
                    help_msg += "The ability to see, hear, taste and notice unusual things. ";
                    help_msg += "A high Perception is important for a sharpshooter.";
                    help_msg += Telnet::reset() + "\n";
                    break;
                case 4:
                    help_msg += "Help about Constitution.\n" + Telnet::italic();
                    help_msg += "Stamina and physical toughness. A character with a high Endurance ";
                    help_msg += "will survive where others may not.";
                    help_msg += Telnet::reset() + "\n";
                    break;
                case 5:
                    help_msg += "Help about Intelligence.\n" + Telnet::italic();
                    help_msg += "Knowledge, wisdom and the ability to think quickly, ";
                    help_msg += "this attribute is important for any character.";
                    help_msg += Telnet::reset() + "\n";
                    break;
                default:
                    throw runtime_error("No help for that attribute.");
            }
            throw runtime_error(help_msg);
        }
        // Check if the player has not yet typed RESET.
        else if (ToLower(input) == "reset")
        {
            help_msg += Telnet::cyan();
            help_msg += "Attribute has been set by default.\n";

            player->remaining_points = 0;
            player->strength = player->race->strength;
            player->agility = player->race->agility;
            player->perception = player->race->perception;
            player->constitution = player->race->constitution;
            player->intelligence = player->race->intelligence;

            throw runtime_error(help_msg);
        }
        // Check if the player has not yet typed CONTINUE.
        else if (ToLower(input) != "continue")
        {
            int new_value;
            int base_value;
            // Get the race number.
            vector<string> args = GetWords(input);
            int attribute = atoi(args[0].c_str());
            int modifier = atoi(args[1].c_str());
            // Check for errors.
            if (args.size() != 2)
            {
                throw runtime_error("Type [#attribute +/-(value)].");
            }
            if (attribute < 1 || 5 < attribute)
            {
                throw runtime_error("Must select a valid attribute.");
            }
            if (modifier < -5 || +5 < modifier)
            {
                throw runtime_error("Modifier must be between -5 and +5.");
            }
            if (player->remaining_points < modifier)
            {
                throw runtime_error("You don't have enough points left.");
            }
            switch (attribute)
            {
                case 1:
                    new_value = player->strength + modifier;
                    base_value = player->race->strength;
                    if (new_value < (base_value - 5))
                    {
                        throw runtime_error("Strength can't go below " + ToString((base_value - 5)) + ".");
                    }
                    if ((base_value + 5) < new_value)
                    {
                        throw runtime_error("Strength can't go above " + ToString((base_value + 5)) + ".");
                    }
                    player->remaining_points -= modifier;
                    player->strength = new_value;
                    break;
                case 2:
                    new_value = player->agility + modifier;
                    base_value = player->race->agility;
                    if (new_value < (base_value - 5))
                    {
                        throw runtime_error("Agility can't go below " + ToString((base_value - 5)) + ".");
                    }
                    if ((base_value + 5) < new_value)
                    {
                        throw runtime_error("Agility can't go above " + ToString((base_value + 5)) + ".");
                    }
                    player->remaining_points -= modifier;
                    player->agility = new_value;
                    break;
                case 3:
                    new_value = player->perception + modifier;
                    base_value = player->race->perception;
                    if (new_value < (base_value - 5))
                    {
                        throw runtime_error("Perception can't go below " + ToString((base_value - 5)) + ".");
                    }
                    if ((base_value + 5) < new_value)
                    {
                        throw runtime_error("Perception can't go above " + ToString((base_value + 5)) + ".");
                    }
                    player->remaining_points -= modifier;
                    player->perception = new_value;
                    break;
                case 4:
                    new_value = player->constitution + modifier;
                    base_value = player->race->constitution;
                    if (new_value < (base_value - 5))
                    {
                        throw runtime_error("Constitution can't go below " + ToString((base_value - 5)) + ".");
                    }
                    if ((base_value + 5) < new_value)
                    {
                        throw runtime_error("Constitution can't go above " + ToString((base_value + 5)) + ".");
                    }
                    player->remaining_points -= modifier;
                    player->constitution = new_value;
                    break;
                case 5:
                    new_value = player->intelligence + modifier;
                    base_value = player->race->intelligence;
                    if (new_value < (base_value - 5))
                    {
                        throw runtime_error("Intelligence can't go below " + ToString((base_value - 5)) + ".");
                    }
                    if ((base_value + 5) < new_value)
                    {
                        throw runtime_error("Intelligence can't go above " + ToString((base_value + 5)) + ".");
                    }
                    player->remaining_points -= modifier;
                    player->intelligence = new_value;
                    break;
                default:
                    throw runtime_error("Must select a valid attribute.");
            }
            throw runtime_error("");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, e.what());
        return;
    }
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewGender);
}
void ProcessNewGender(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);
    int choice;
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewAttr);
            return;
        }
        else if (!IsNumber(input))
        {
            throw runtime_error("Not a valid gender.");
        }
        choice = atoi(input.c_str());
        if (choice < 1 || choice > 2)
        {
            throw runtime_error("Not a valid gender.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewGender, e.what());
        return;
    }
    player->sex = choice;
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewAge);
}
void ProcessNewAge(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);
    int choice;
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewGender);
            return;
        }
        if (!IsNumber(input))
        {
            throw runtime_error("Not a valid age.");
        }
        choice = atoi(input.c_str());
        if (choice < 18)
        {
            throw runtime_error("A creature so young is not suitable for a world so wicked.");
        }
        if (50 < choice)
        {
            throw runtime_error(
                "Life expectancy in this world is 70 years, in order to still be competitive you can choose 50 years at most.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAge, e.what());
        return;
    }
    player->age = choice;
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewDesc);
}
void ProcessNewDesc(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewAge);
            return;
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "skip")
        {
            player->description = "You see " + ToLower(player->name) + " here.";
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewWeight);
            return;
        }

        // Check if the description contains bad characters.
        if (input.find_first_not_of(kValidDescription) != string::npos)
        {
            throw runtime_error("Description cannot contain disallowed characters.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewDesc, e.what());
        return;
    }
    player->description = input;
    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewWeight);
}
void ProcessNewWeight(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);

    // Set the minimum weight.
    int choice;
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewDesc);
            return;
        }
        if (!IsNumber(input))
        {
            throw runtime_error("Not a valid weight.");
        }
        choice = atoi(input.c_str());
        if (choice < 40)
        {
            throw runtime_error("You can't be a feather.");
        }
        if (120 < choice)
        {
            throw runtime_error("Too much.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewWeight, e.what());
        return;
    }
    player->weight = choice;

    AdvanceCharacterCreation(player, ConnectionState::AwaitingNewConfirm);
}
void ProcessNewConfirm(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    string input;
    getline(sArgs, input);
    try
    {
        // Player_password can't be blank.
        if (input.empty())
        {
            throw runtime_error("Invalid input.");
        }
        // Check if the player has typed BACK.
        if (ToLower(input) == "back")
        {
            RollbackCharacterCreation(player, ConnectionState::AwaitingNewWeight);
            return;
        }
        // Check if the player has typed CONFIRM.
        if (ToLower(input) != "confirm")
        {
            throw runtime_error("You must write 'confirm' if you agree.");
        }
    }
    catch (runtime_error & e)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewConfirm, e.what());
        return;
    }

    // //////////////////////////////////////////////////////////////
    // Set the last variables.
    player->level = 0;
    player->experience = 0;
    player->flags = 0;
    player->rent_room = 1000;
    if (player->createOnDB())
    {
        AdvanceCharacterCreation(player, ConnectionState::Playing);
    }
    else
    {
        player->closeConnection();
    }
}
