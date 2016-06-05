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

/// Player names must consist of characters from this list.
const std::string kValidPlayerName = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-";
/// Player descriptions must consist of characters from this list.
const std::string kValidDescription = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,.\n";

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
            Profession * profession = Mud::instance().findProfession(command);

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

void ProcessPlayerName(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Name can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName, "Name cannot be blank.");
    }
    // Check if the player has typed new.
    else if (ToLower(input) == "new")
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewName);
    }
    // Check if the user want to quit.
    else if (input == "quit")
    {
        player->closeConnection();
    }
    // Check if the give name contains valid characters.
    else if (input.find_first_not_of(kValidPlayerName) != std::string::npos)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName,
            "That player name contains disallowed characters.");
    }
    // Check if the player is already connected.
    else if (Mud::instance().findPlayer(input))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName, input + " is already connected.");
    }
    // Check if the player exists in the Database.
    else if (!SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName, "That player doesen't exist.");
    }
    else
    {
        // Save the name of the player.
        player->name = ToCapitals(input);
        // Load player so we know the player_password etc.
        if (player->loadFromDB())
        {
            // Delete the loaded prompt, otherwise it will be shown.
            player->prompt = "";
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
}

void ProcessPlayerPassword(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Check the correctness of the password.
    if (input != player->password)
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
            AdvanceCharacterCreation(character, ConnectionState::AwaitingPassword, "Incorrect password.");
        }
    }
    else
    {
        AdvanceCharacterCreation(character, ConnectionState::Playing);
    }
}

void ProcessNewName(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Player_password can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewName, "Invalid input.");
    }
    // Check if the player has typed BACK.
    else if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingName);
    }
    // Check if the player has given bad characters.
    else if (input.find_first_not_of(kValidPlayerName) != std::string::npos)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewName,
            "That player name contains disallowed characters.");
    }
    // Check for bad names here.
    else if (Mud::instance().badNames.find(input) != Mud::instance().badNames.end())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewName, "That name is not permitted.");
    }
    // Check if the player name has already been used.
    else if (SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewName,
            "That player already exists, please choose another name.");
    }
    else
    {
        Room * spawnRoom = Mud::instance().findRoom(player->rent_room);
        if (spawnRoom == nullptr)
        {
            spawnRoom = Mud::instance().findRoom(1000);
        }
        player->name = ToCapitals(input);
        player->room = spawnRoom;
        player->faction = Mud::instance().findFaction(1);
        player->prompt_save = "[" + player->name + "]";
        player->password_attempts = 0;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwd);
    }
}

void ProcessNewPwd(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Player_password can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewPwd, "Invalid input.");
    }
    // Check if the player has typed BACK.
    else if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewName);
    }
    // Check if the player has given bad characters.
    else if (input.find_first_not_of(kValidPlayerName) != std::string::npos)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewPwd,
            "Password cannot contain disallowed characters.");
    }
    else
    {
        player->password = input;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwdCon);
    }
}

void ProcessNewPwdCon(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewPwd);
    }
    // Player_password must agree.
    else if (input != player->password)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewPwdCon,
            "Password and confirmation do not agree.");
    }
    else
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewStory);
    }
}

void ProcessNewStory(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Player_password can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewStory, "Invalid input.");
    }
    // Check if the player has typed BACK.
    else if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewPwd);
    }
    // Check if the player has written 'continue' or NOT.
    else if (input != "continue")
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewStory,
            "Write 'continue' after you have readed the story.");
    }
    else
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewRace);
    }
}

void ProcessNewRace(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();

    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);

    // Player_password can't be blank.
    if ((arguments.size() != 1) && (arguments.size() != 2))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "Invalid input.");
    }
    // Check if the player has typed BACK.
    else if (ToLower(arguments[0].first) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewStory);
    }
    // If the player has insert help (race_number), show its help.
    else if (BeginWith(ToLower(arguments[0].first), "help"))
    {
        if (arguments.size() == 2)
        {
            // Get the race.
            Race * race = Mud::instance().findRace(ToInt(arguments[1].first));
            if (race == nullptr)
            {
                AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "No help for that race.");
            }
            else if (!race->player_allow)
            {
                AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "No help for that race.");
            }
            else
            {
                std::string helpMessage;
                helpMessage += "Help about " + race->name + ".\n";
                helpMessage += "Strength     " + ToString(race->strength) + ".\n";
                helpMessage += "Agility      " + ToString(race->agility) + ".\n";
                helpMessage += "Perception   " + ToString(race->perception) + ".\n";
                helpMessage += "Constitution " + ToString(race->constitution) + ".\n";
                helpMessage += "Intelligence " + ToString(race->intelligence) + ".\n";
                helpMessage += Formatter::italic() + race->description + Formatter::reset() + "\n";
                AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, helpMessage);
            }

        }
        else
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace,
                "You have to specify the race number.");
        }
    }
    else if (IsNumber(arguments[0].first))
    {
        // Get the race.
        Race * race = Mud::instance().findRace(ToInt(arguments[0].first));
        if (race == nullptr)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "Not a valid race.");
        }
        else if (!race->player_allow)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "Not a valid race.");
        }
        else
        {
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
    }
}

void ProcessNewAttr(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();

    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Player_password can't be blank.
    if ((arguments.size() != 1) && (arguments.size() != 2))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, "Invalid input.");
    }
    // Check if the player has typed BACK.
    else if (ToLower(arguments[0].first) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewRace);
    }
    // Check if the player has not yet typed RESET.
    else if (ToLower(arguments[0].first) == "reset")
    {
        player->remaining_points = 0;
        player->strength = player->race->strength;
        player->agility = player->race->agility;
        player->perception = player->race->perception;
        player->constitution = player->race->constitution;
        player->intelligence = player->race->intelligence;
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr,
            Formatter::cyan() + "Attribute has been set by default." + Formatter::reset() + "\n");
    }
    // If the player has insert help (attribute number), show its help.
    else if (BeginWith(ToLower(arguments[0].first), "continue"))
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewGender);
    }
    // If the player has insert help (attribute number), show its help.
    else if (BeginWith(ToLower(arguments[0].first), "help"))
    {
        if (arguments.size() == 2)
        {
            std::string helpMessage;
            if (arguments[1].first == "1")
            {
                helpMessage = "Help about Strength.\n" + Formatter::italic();
                helpMessage += "Strength is important for increasing the Carrying Weight and ";
                helpMessage += "satisfying the minimum Strength requirements for some weapons and armors.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (arguments[1].first == "2")
            {
                helpMessage = "Help about Agility.\n" + Formatter::italic();
                helpMessage += "Besides increasing mobility in combat, it increases the recharge ";
                helpMessage += "speed of all the weapons, as well as the ability to use light armor.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (arguments[1].first == "3")
            {
                helpMessage = "Help about Perception.\n" + Formatter::italic();
                helpMessage += "The ability to see, hear, taste and notice unusual things. ";
                helpMessage += "A high Perception is important for a sharpshooter.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (arguments[1].first == "4")
            {
                helpMessage = "Help about Constitution.\n" + Formatter::italic();
                helpMessage += "Stamina and physical toughness. A character with a high Endurance ";
                helpMessage += "will survive where others may not.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (arguments[1].first == "5")
            {
                helpMessage = "Help about Intelligence.\n" + Formatter::italic();
                helpMessage += "Knowledge, wisdom and the ability to think quickly, ";
                helpMessage += "this attribute is important for any character.";
                helpMessage += Formatter::reset() + "\n";
            }
            else
            {
                helpMessage = "No help for that attribute.";
            }
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, helpMessage);
        }
        else
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr,
                "You have to specify the attribute number.");
        }
    }
    else
    {
        if (arguments.size() == 2)
        {
            std::string helpMessage;
            int modifier = ToInt(arguments[1].first);
            // Check for errors.
            if (player->remaining_points < modifier)
            {
                helpMessage = "You don't have enough points left.";
            }
            else if (arguments[0].first == "1")
            {
                if ((player->strength + modifier) < (player->race->strength - 5))
                {
                    helpMessage = "Strength can't go below " + ToString((player->race->strength - 5)) + ".";
                }
                else if ((player->strength + modifier) > (player->race->strength + 5))
                {
                    helpMessage = "Strength can't go above " + ToString((player->race->strength + 5)) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->strength += modifier;
                }
            }
            else if (arguments[0].first == "2")
            {
                if ((player->agility + modifier) < (player->race->agility - 5))
                {
                    helpMessage = "Agility can't go below " + ToString((player->race->agility - 5)) + ".";
                }
                else if ((player->strength + modifier) > (player->race->strength + 5))
                {
                    helpMessage = "Agility can't go above " + ToString((player->race->agility + 5)) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->agility += modifier;
                }
            }
            else if (arguments[0].first == "3")
            {
                if ((player->perception + modifier) < (player->race->perception - 5))
                {
                    helpMessage = "Perception can't go below " + ToString((player->race->perception - 5)) + ".";
                }
                else if ((player->strength + modifier) > (player->race->strength + 5))
                {
                    helpMessage = "Perception can't go above " + ToString((player->race->perception + 5)) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->perception += modifier;
                }
            }
            else if (arguments[0].first == "4")
            {
                if ((player->constitution + modifier) < (player->race->constitution - 5))
                {
                    helpMessage = "Constitution can't go below " + ToString((player->race->constitution - 5)) + ".";
                }
                else if ((player->strength + modifier) > (player->race->strength + 5))
                {
                    helpMessage = "Constitution can't go above " + ToString((player->race->constitution + 5)) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->constitution += modifier;
                }
            }
            else if (arguments[0].first == "5")
            {
                if ((player->intelligence + modifier) < (player->race->intelligence - 5))
                {
                    helpMessage = "Intelligence can't go below " + ToString((player->race->intelligence - 5)) + ".";
                }
                else if ((player->strength + modifier) > (player->race->strength + 5))
                {
                    helpMessage = "Intelligence can't go above " + ToString((player->race->intelligence + 5)) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->intelligence += modifier;
                }
            }
            else
            {
                helpMessage = "Must select a valid attribute.";
            }
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, helpMessage);
        }
        else
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, "Type [#attribute +/-(value)].");
        }
    }
}

void ProcessNewGender(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewAttr);
    }
    else if (input == "1")
    {
        player->gender = GenderType::Male;
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAge);
    }
    else if (input == "2")
    {
        player->gender = GenderType::Female;
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAge);
    }
    else
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewGender, "Not a valid gender.");
    }
}

void ProcessNewAge(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewGender);
    }
    else if (!IsNumber(input))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAge, "Not a valid age.");
    }
    else
    {
        int age = ToInt(input);
        if (age < 18)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAge,
                "A creature so young is not suitable for a world so wicked.");
        }
        else if (50 < age)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAge,
                "Life expectancy in this world is 70 years, in order to still be competitive you can choose 50 years at most.");
        }
        else
        {
            player->age = age;
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewDesc);
        }
    }
}

void ProcessNewDesc(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);
    // Player_password can't be blank.
    if (input.empty())
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewDesc, "Invalid input.");
    }
    // Check if the player has typed BACK.
    else if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewAge);
    }
    // Check if the player has typed BACK.
    else if (ToLower(input) == "skip")
    {
        player->description = "You see " + ToLower(player->name) + " here.";
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewWeight);
    }
    // Check if the description contains bad characters.
    else if (input.find_first_not_of(kValidDescription) != std::string::npos)
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewDesc,
            "Description cannot contain disallowed characters.");
    }
    else
    {
        player->description = input;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewWeight);
    }
}

void ProcessNewWeight(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewDesc);
    }
    else if (!IsNumber(input))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewWeight, "Not a valid weight.");
    }
    else
    {
        int weight = ToInt(input);
        if (weight < 40)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewWeight, "You can't be a feather.");
        }
        else if (120 < weight)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewWeight, "Too much.");
        }
        else
        {
            player->weight = weight;
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewConfirm);
        }
    }
}

void ProcessNewConfirm(Character * character, std::istream & sArgs)
{
    Player * player = character->toPlayer();
    std::string input;
    getline(sArgs, input);

    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewWeight);
    }
    // Check if the player has typed CONFIRM.
    else if (ToLower(input) == "confirm")
    {
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
    else
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewConfirm,
            "You must write 'confirm' if you agree.");
    }
}

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
    if ((character->getAction()->getType() != ActionType::Wait)
        && (character->getAction()->getType() != ActionType::NoAction))
    {
        character->doCommand("stop");
    }
}

ArgumentList ParseArgs(std::istream & sArgs)
{
    ArgumentList arguments;
    std::vector<std::string> words;

    // Get the words from the input line.
    std::string argumentsLine;
    std::getline(sArgs, argumentsLine);
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

void LoadStates()
{
    // Step 0 - Insert name.
    Mud::instance().addStateAction(ConnectionState::AwaitingName, ProcessPlayerName);
    // Step 1.a  - Insert the password.
    Mud::instance().addStateAction(ConnectionState::AwaitingPassword, ProcessPlayerPassword);
    // Step 1.b  - Choose the Name.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewName, ProcessNewName);
    // Step 2  - Choose the Password.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewPwd, ProcessNewPwd);
    // Step 3  - Confirm the Password.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewPwdCon, ProcessNewPwdCon);
    // Step 4  - Short story of the mud world.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewStory, ProcessNewStory);
    // Step 5  - Choose the Race.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewRace, ProcessNewRace);
    // Step 6  - Choose the Attributes.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewAttr, ProcessNewAttr);
    // Step 7  - Choose the Gender.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewGender, ProcessNewGender);
    // Step 8  - Choose the Age.
    Mud::instance().addStateAction(ConnectionState::AwaitingNewAge, ProcessNewAge);
    // Step 9  - Choose the description (optional).
    Mud::instance().addStateAction(ConnectionState::AwaitingNewDesc, ProcessNewDesc);
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
    if (player->strength > 0)
    {
        preview += ToString(player->strength) + Formatter::reset();
    }
    preview += "\n";

    // AGILITY
    preview += "# Agility      :";
    if (player->agility > 0)
    {
        preview += ToString(player->agility) + Formatter::reset();
    }
    preview += "\n";

    // PERCEPTION
    preview += "# Perception   :";
    if (player->perception > 0)
    {
        preview += ToString(player->perception) + Formatter::reset();
    }
    preview += "\n";

    // CONSTITUTION
    preview += "# Constitution :";
    if (player->constitution > 0)
    {
        preview += ToString(player->constitution) + Formatter::reset();
    }
    preview += "\n";

    // INTELLIGENCE
    preview += "# Intelligence :";
    if (player->intelligence > 0)
    {
        preview += ToString(player->intelligence) + Formatter::reset();
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
            player->gender = GenderType::None;
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
        msg += Formatter::clearScreen();
        msg += "\nWelcome to RadMud. Version " + Formatter::green() + kVersion + Formatter::reset() + "!\n";
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
        msg += " to pick a legendary name, maybe one day it will be whispered all over the lands.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the login.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewPwd)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Password." + Formatter::reset() + "\n";
        msg += "# Choose a proper password, in order to protect the acces to your character.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the previus step.\n";
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
        msg += "# Type [" + Formatter::magenta() + "continue" + Formatter::reset() + "] to continue character creation.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewRace)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Race." + Formatter::reset() + "\n";
        for (auto iterator : Mud::instance().mudRaces)
        {
            Race * race = &iterator.second;
            if (race->player_allow) msg += "#    [" + ToString(race->vnum) + "] " + race->name + ".\n";
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
        msg += "#    [1] Strength     :" + ToString(player->strength) + "\n";
        msg += "#    [2] Agility      :" + ToString(player->agility) + "\n";
        msg += "#    [3] Perception   :" + ToString(player->perception) + "\n";
        msg += "#    [4] Constitution :" + ToString(player->constitution) + "\n";
        msg += "#    [5] Intelligence :" + ToString(player->intelligence) + "\n";
        msg += "#\n";
        msg += "# Remaining Points: " + Formatter::green() + ToString(player->remaining_points) + Formatter::reset() + "\n";
        msg += "#\n";
        msg += "# Type [" + Formatter::magenta() + "(number) +/-modifier" + Formatter::reset() + "]";
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
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewAge)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Age." + Formatter::reset() + "\n";
        msg += "# Choose the starting age of your character, be aware that during the game the time will pass.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewDesc)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Description." + Formatter::reset() + "\n";
        msg += "# Insert a brief description of your character, its optional.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the previus step.\n";
        msg += "# Type [" + Formatter::magenta() + "skip" + Formatter::reset() + "] to just pass to the next step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewWeight)
    {
        PrintChoices(player);
        msg += "# " + Formatter::bold() + "Character's Weight." + Formatter::reset() + "\n";
        msg += "# Choose the wheight of your character.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "] to return to the previus step.\n";
    }
    else if (new_state == ConnectionState::AwaitingNewConfirm)
    {
        PrintChoices(player);
        msg += "# Give a look to the information you have provided, now it's the right time";
        msg += " to decide if you want to change something.\n";
        msg += "# Type [" + Formatter::magenta() + "confirm" + Formatter::reset() + "] to conclude the character creation.\n";
        msg += "# Type [" + Formatter::magenta() + "back" + Formatter::reset() + "]    to return to the previus step.\n";
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
    Command command;
    command.level = 0;
    {
        command.name = "save";
        command.help = "Save character into the database.";
        command.args = "";
        command.hndl = DoSave;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "quit";
        command.help = "Leave the game.";
        command.args = "";
        command.hndl = DoQuit;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "who";
        command.help = "List all the character online.";
        command.args = "";
        command.hndl = DoWho;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "set";
        command.help = "Set some character texts(eg. descr).";
        command.args = "(setting) (value)";
        command.hndl = DoSet;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "stop";
        command.help = "Stop the current character action.";
        command.args = "";
        command.hndl = DoStop;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "look";
        command.help = "Look at something or someone.";
        command.args = "[(something) or (someone)]";
        command.hndl = DoLook;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "help";
        command.help = "Show the list of commands or show help for a given command.";
        command.args = "(command)";
        command.hndl = DoHelp;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "prompt";
        command.help = "Modify your prompt.";
        command.args = "(help)|(prompt definition)";
        command.hndl = DoPrompt;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "time";
        command.help = "Give the current day phase.";
        command.args = "";
        command.hndl = DoTime;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "stand";
        command.help = "Make the player stand.";
        command.args = "";
        command.hndl = DoStand;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "crouch";
        command.help = "The player crouches down himself, it's a good stance for hiding.";
        command.args = "";
        command.hndl = DoCrouch;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "sit";
        command.help = "The player sits down, ideal for a quick break.";
        command.args = "";
        command.hndl = DoSit;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "prone";
        command.help = "The player starts prone, a perfect position to shoot long distance.";
        command.args = "";
        command.hndl = DoProne;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "rest";
        command.help = "The player lies down and begin to rest.";
        command.args = "";
        command.hndl = DoRest;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "statistics";
        command.help = "Show player statistics.";
        command.args = "";
        command.hndl = DoStatistics;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "rent";
        command.help = "Allow player to rent and disconnect.";
        command.args = "";
        command.hndl = DoRent;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "skills";
        command.help = "Shows the playes abilities and their level.";
        command.args = "";
        command.hndl = DoSkills;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "server";
        command.help = "Shows the server statistics.";
        command.args = "";
        command.hndl = DoServer;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "travel";
        command.help = "Allow the character to travel.";
        command.args = "";
        command.hndl = DoTravel;
        Mud::instance().addCommand(command);
    }

    // Inventory and Equipment.
    {
        command.name = "take";
        command.help = "Take something from the ground or from a container.";
        command.args = "(item) [(container)]";
        command.hndl = DoTake;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "drop";
        command.help = "Drop an object.";
        command.args = "(item)";
        command.hndl = DoDrop;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "give";
        command.help = "Give an object to someone.";
        command.args = "(item) (someone)";
        command.hndl = DoGive;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "equipments";
        command.help = "List all the items you are wearing.";
        command.args = "";
        command.hndl = DoEquipments;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "wield";
        command.help = "Wield a weapon, a shield or maybe a tool.";
        command.args = "(item)";
        command.hndl = DoWield;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "wear";
        command.help = "Puts on a piece of equipment.";
        command.args = "(item)";
        command.hndl = DoWear;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "remove";
        command.help = "Remove a weared or wielded item.";
        command.args = "(item)";
        command.hndl = DoRemove;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "inventory";
        command.help = "Show character's inventory.";
        command.args = "";
        command.hndl = DoInventory;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "organize";
        command.help = "Order the desired container or if no target is passed, the room.";
        command.args = "(name|weight) [(target)]";
        command.hndl = DoOrganize;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "open";
        command.help = "Open a door in a given direction.";
        command.args = "(item)|(direction)";
        command.hndl = DoOpen;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "close";
        command.help = "Close a door in a given direction.";
        command.args = "(item)|(direction)";
        command.hndl = DoClose;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "put";
        command.help = "Put something inside a container.";
        command.args = "(something) (container)";
        command.hndl = DoPut;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "drink";
        command.help = "Drink from a container of liquids.";
        command.args = "(container)";
        command.hndl = DoDrink;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "fill";
        command.help = "Fill a container of liquids from a source of liquid.";
        command.args = "(container) (source)";
        command.hndl = DoFill;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "pour";
        command.help = "Pour the content of the container into another one or on the ground.";
        command.args = "(container) [container]";
        command.hndl = DoPour;
        Mud::instance().addCommand(command);
    }

    // Comunication.
    {
        command.name = "say";
        command.help = "Talk to people in the current room.";
        command.args = "[someone] (something)";
        command.hndl = DoSay;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "whisper";
        command.help = "Whisper secretly to a single character.";
        command.args = "(someone) (something)";
        command.hndl = DoWhisper;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "emote";
        command.help = "Execute and emote.";
        command.args = "(emotion)";
        command.hndl = DoEmote;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "bug";
        command.help = "Report a bug, your character's name, location and date will be saved.";
        command.args = "(message)";
        command.hndl = DoBug;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "idea";
        command.help = "Send an idea, try to be as clear as possible.";
        command.args = "(message)";
        command.hndl = DoIdea;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "typo";
        command.help = "Report a typo.";
        command.args = "(message)";
        command.hndl = DoTypo;
        Mud::instance().addCommand(command);
    }

    // Gathering.
    //{
    //command.name = "mine";
    //command.help = "Gather some metal from a node.";
    //command.args = "(node)";
    //command.hndl = DoMine;
    //Mud::instance().addCommand(command);
    //}
    //{
    //command.name = "chop";
    //command.help = "Gather some wood branches.";
    //command.args = "(node)";
    //command.hndl = DoChop;
    //Mud::instance().addCommand(command);
    //}

    // Crafting.
    {
        command.name = "build";
        command.help = "Build something.";
        command.args = "(item)";
        command.hndl = DoBuild;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "deconstruct";
        command.help = "Deconstruct a building.";
        command.args = "(building)";
        command.hndl = DoDeconstruct;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "read";
        command.help = "Read an inscription from an item.";
        command.args = "(item)";
        command.hndl = DoRead;
        Mud::instance().addCommand(command);
    }

    // Combat.
    {
        command.name = "kill";
        command.help = "Engage in combat the desired target.";
        command.args = "(target)";
        command.hndl = DoKill;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "flee";
        command.help = "Try to flee from combat.";
        command.args = "";
        command.hndl = DoFlee;
        Mud::instance().addCommand(command);
    }

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // GODS COMMANDS
    // ////////////////////////////////////////////////////////////////////////////////////////////

    {
        command.name = "shutdown";
        command.help = "Shut the MUD down.";
        command.args = "";
        command.hndl = DoShutdown;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "goto";
        command.help = "Go to another room.";
        command.args = "(Room.vnum)";
        command.hndl = DoGoTo;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "hurt";
        command.help = "Hurt the desired target.";
        command.args = "(Target)";
        command.hndl = DoHurt;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "invisibility";
        command.help = "Became invisible.";
        command.args = "";
        command.hndl = DoInvisibility;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "visible";
        command.help = "Return visible.";
        command.args = "";
        command.hndl = DoVisible;
        Mud::instance().addCommand(command);
    }

    // Interaction with Players.
    {
        command.name = "pinfo";
        command.help = "Return all the information concerning a player.";
        command.args = "(Target)";
        command.hndl = DoPlayerInfo;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "transfer";
        command.help = "Transfer another character here, or to another room.";
        command.args = "(Target) [Where]";
        command.hndl = DoTransfer;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "sflag";
        command.help = "Sets a flag for a character.";
        command.args = "(Target) (Flag)";
        command.hndl = DoSetFlag;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "cflag";
        command.help = "Remove a flag from a character.";
        command.args = "(Target) (Flag)";
        command.hndl = DoClearFlag;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "modskill";
        command.help = "Modify the value of the player skill.";
        command.args = "(Target) (Skill) (Value)";
        command.hndl = DoModSkill;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "modattribute";
        command.help = "Modify the value of the player attribute.";
        command.args = "(Target) (Attribute) (Value)";
        command.hndl = DoModAttr;
        Mud::instance().addCommand(command);
    }

    // Item's Model.
    {
        command.name = "imodel";
        command.help = "List all the information about a model.";
        command.args = "(Model.vnum)";
        command.hndl = DoModelInfo;
        Mud::instance().addCommand(command);
    }

    // Items.
    {
        command.name = "iitem";
        command.help = "Show information about an item.";
        command.args = "(Item.vnum)";
        command.hndl = DoItemInfo;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "citem";
        command.help = "Create a new item, if not set the quality will be Normal.";
        command.args = "(Model.vnum)(Material.vnum)[Quality]";
        command.hndl = DoItemCreate;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "ditem";
        command.help = "Destroy the desired object.";
        command.args = "(Item.vnum)";
        command.hndl = DoItemDestroy;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "gitem";
        command.help = "Materialize the desired object.";
        command.args = "(Item.vnum)";
        command.hndl = DoItemGet;
        Mud::instance().addCommand(command);
    }

    // Areas.
    {
        command.name = "iarea";
        command.help = "Show the informations about a specific area.";
        command.args = "(Area.vnum)";
        command.hndl = DoAreaInfo;
        Mud::instance().addCommand(command);
    }

    // Rooms.
    {
        command.name = "iroom";
        command.help = "Show the informations about a specific room.";
        command.args = "(Room.vnum)";
        command.hndl = DoRoomInfo;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "croom";
        command.help = "Create a room in the given direction.";
        command.args = "(Direction)";
        command.hndl = DoRoomCreate;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "droom";
        command.help = "Delete the room in the given direction.";
        command.args = "(Direction)";
        command.hndl = DoRoomDelete;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "eroom";
        command.help = "Change room values.";
        command.args = "(Option) (Value)";
        command.hndl = DoRoomEdit;
        Mud::instance().addCommand(command);
    }

    // Mobiles.
    {
        command.name = "imobile";
        command.help = "Get information about a mobile.";
        command.args = "(Mobile.id)";
        command.hndl = DoMobileInfo;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "kmobile";
        command.help = "Kill the desired mobile, in the same room.";
        command.args = "(Mobile.name)";
        command.hndl = DoMobileKill;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "rmobile";
        command.help = "Reload the lua script for the target mobile, in the same room.";
        command.args = "(Mobile.name)";
        command.hndl = DoMobileReload;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "mobilelog";
        command.help = "Given a mobile id, it returns the curresponding mobile log.";
        command.args = "(Mobile.id)";
        command.hndl = DoMobileLog;
        Mud::instance().addCommand(command);
    }

    // Materials.
    {
        command.name = "imaterial";
        command.help = "Show the informations about a specific material.";
        command.args = "(Material.vnum)";
        command.hndl = DoMaterialInfo;
        Mud::instance().addCommand(command);
    }

    // Liquids.
    {
        command.name = "iliquid";
        command.help = "Show the informations about a specific liquid.";
        command.args = "(Liquid.vnum)";
        command.hndl = DoLiquidInfo;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "cliquid";
        command.help = "Materialize some liquid sinde a container.";
        command.args = "(Container.name)(Liquid.vnum)(Quantity)";
        command.hndl = DoLiquidCreate;
        Mud::instance().addCommand(command);
    }

    // Productions.
    {
        command.name = "iproduction";
        command.help = "Provide all the information regarding the given production.";
        command.args = "(Production.vnum)";
        command.hndl = DoProductionInfo;
        Mud::instance().addCommand(command);
    }

    // Profession.
    {
        command.name = "iprofession";
        command.help = "Provide all the information regarding the given profession.";
        command.args = "(Profession.command)";
        command.hndl = DoProfessionInfo;
        Mud::instance().addCommand(command);
    }

    //
    // LISTS
    //
    {
        command.name = "lmodel";
        command.help = "List all the models for the items.";
        command.args = "";
        command.hndl = DoModelList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "litem";
        command.help = "List all the items.";
        command.args = "";
        command.hndl = DoItemList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lmobile";
        command.help = "List all the mobiles.";
        command.args = "";
        command.hndl = DoMobileList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lplayer";
        command.help = "List all the players.";
        command.args = "";
        command.hndl = DoPlayerList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "larea";
        command.help = "List all the areas.";
        command.args = "";
        command.hndl = DoAreaList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lroom";
        command.help = "List all the rooms.";
        command.args = "";
        command.hndl = DoRoomList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lrace";
        command.help = "List all the races.";
        command.args = "";
        command.hndl = DoRaceList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lfaction";
        command.help = "List all the factions.";
        command.args = "";
        command.hndl = DoFactionList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lskill";
        command.help = "List all the skills.";
        command.args = "";
        command.hndl = DoSkillList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lwriting";
        command.help = "List all the writings.";
        command.args = "";
        command.hndl = DoWritingList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lcorpse";
        command.help = "List all the corpses.";
        command.args = "";
        command.hndl = DoCorpseList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lcontinent";
        command.help = "List all the continents.";
        command.args = "";
        command.hndl = DoContinentList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lmaterial";
        command.help = "List all the materials.";
        command.args = "";
        command.hndl = DoMaterialList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lprofession";
        command.help = "Get the list of all the professions.";
        command.args = "";
        command.hndl = DoProfessionList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lproduction";
        command.help = "Get the list of all the productions.";
        command.args = "";
        command.hndl = DoProductionList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lliquid";
        command.help = "List all the liquids.";
        command.args = "";
        command.hndl = DoLiquidList;
        Mud::instance().addCommand(command);
    }
    {
        command.name = "lbuild";
        command.help = "List all the buildings.";
        command.args = "";
        command.hndl = DoBuildingList;
        Mud::instance().addCommand(command);
    }
}
