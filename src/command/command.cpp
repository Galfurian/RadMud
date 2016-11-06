/// @file   command.cpp
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
#include "command.hpp"
#include "mud.hpp"

#include "combat.hpp"
#include "communication.hpp"
#include "crafting.hpp"
#include "general.hpp"
#include "god.hpp"
#include "manager.hpp"
#include "object.hpp"

/// Player names must consist of characters from this list.
#define  VALID_CHARACTERS_NAME "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-"
/// Player descriptions must consist of characters from this list.
#define  VALID_CHARACTERS_DESC "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,.\n"

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

void ProcessPlayerName(Character * character, ArgumentHandler & args)
{
    auto player = character->toPlayer();
    auto input = args.getOriginal();
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
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingName,
            "That player name contains disallowed characters.");
    }
        // Check if the player is already connected.
    else if (Mud::instance().findPlayer(input))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingName,
            input + " is already connected.");
    }
        // Check if the player exists in the Database.
    else if (!SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingName,
            "That player doesen't exist.");
    }
    else
    {
        // Save the name of the player.
        player->name = ToCapitals(input);
        // Load player so we know the player_password etc.
        if (SQLiteDbms::instance().loadPlayer(player))
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

void ProcessPlayerPassword(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
    // Check the correctness of the password.
    if (input != player->password)
    {
        // Detect too many password attempts.
        if (++player->password_attempts >= 3)
        {
            player->closeConnection();
            player->sendMsg("Goodbye!\n");
            player->sendMsg("Too many attempts to guess the password!\n");
        }
        else
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingPassword,
                "Incorrect password.");
        }
    }
    else
    {
        AdvanceCharacterCreation(character, ConnectionState::Playing);
    }
}

void ProcessNewName(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewName,
            "That player name contains disallowed characters.");
    }
        // Check for bad names here.
    else if (Mud::instance().badNames.find(input) != Mud::instance().badNames.end())
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewName,
            "That name is not permitted.");
    }
        // Check if the player name has already been used.
    else if (SQLiteDbms::instance().searchPlayer(ToCapitals(input)))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewName,
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

void ProcessNewPwd(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
    else if (input.find_first_not_of(VALID_CHARACTERS_NAME) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewPwd,
            "Password cannot contain disallowed characters.");
    }
    else
    {
        player->password = input;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewPwdCon);
    }
}

void ProcessNewPwdCon(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewPwd);
    }
        // Player_password must agree.
    else if (input != player->password)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewPwdCon,
            "Password and confirmation do not agree.");
    }
    else
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewStory);
    }
}

void ProcessNewStory(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewStory,
            "Write 'continue' after you have readed the story.");
    }
    else
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewRace);
    }
}

void ProcessNewRace(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    // Player_password can't be blank.
    if ((args.size() != 1) && (args.size() != 2))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, "Invalid input.");
    }
        // Check if the player has typed BACK.
    else if (ToLower(args[0].getContent()) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewStory);
    }
        // If the player has insert help (race_number), show its help.
    else if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 2)
        {
            // Get the race.
            Race * race = Mud::instance().findRace(ToNumber<int>(args[1].getContent()));
            if (race == nullptr)
            {
                AdvanceCharacterCreation(
                    character,
                    ConnectionState::AwaitingNewRace,
                    "No help for that race.");
            }
            else if (!race->player_allow)
            {
                AdvanceCharacterCreation(
                    character,
                    ConnectionState::AwaitingNewRace,
                    "No help for that race.");
            }
            else
            {
                std::string helpMessage;
                helpMessage += "Help about " + race->name + ".\n";
                helpMessage += "Strength     " + ToString(race->getAbility(Ability::Strength))
                               + ".\n";
                helpMessage += "Agility      " + ToString(race->getAbility(Ability::Agility))
                               + ".\n";
                helpMessage += "Perception   " + ToString(race->getAbility(Ability::Perception))
                               + ".\n";
                helpMessage += "Constitution " + ToString(race->getAbility(Ability::Constitution))
                               + ".\n";
                helpMessage += "Intelligence " + ToString(race->getAbility(Ability::Intelligence))
                               + ".\n";
                helpMessage += Formatter::italic() + race->description + Formatter::reset() + "\n";
                AdvanceCharacterCreation(character, ConnectionState::AwaitingNewRace, helpMessage);
            }

        }
        else
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewRace,
                "You have to specify the race number.");
        }
    }
    else if (IsNumber(args[0].getContent()))
    {
        // Get the race.
        Race * race = Mud::instance().findRace(ToNumber<int>(args[0].getContent()));
        if (race == nullptr)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewRace,
                "Not a valid race.");
        }
        else if (!race->player_allow)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewRace,
                "Not a valid race.");
        }
        else
        {
            player->race = race;
            player->setAbility(Ability::Strength, race->getAbility(Ability::Strength));
            player->setAbility(Ability::Agility, race->getAbility(Ability::Agility));
            player->setAbility(Ability::Perception, race->getAbility(Ability::Perception));
            player->setAbility(Ability::Constitution, race->getAbility(Ability::Constitution));
            player->setAbility(Ability::Intelligence, race->getAbility(Ability::Intelligence));
            player->setHealth(player->getMaxHealth(), true);
            player->setStamina(player->getMaxStamina(), true);
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewAttr);
        }
    }
}

void ProcessNewAttr(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    // Player_password can't be blank.
    if ((args.size() != 1) && (args.size() != 2))
    {
        AdvanceCharacterCreation(character, ConnectionState::AwaitingNewAttr, "Invalid input.");
    }
        // Check if the player has typed BACK.
    else if (ToLower(args[0].getContent()) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewRace);
    }
        // Check if the player has not yet typed RESET.
    else if (ToLower(args[0].getContent()) == "reset")
    {
        player->remaining_points = 0;
        player->setAbility(Ability::Strength, player->race->getAbility(Ability::Strength));
        player->setAbility(Ability::Agility, player->race->getAbility(Ability::Agility));
        player->setAbility(Ability::Perception, player->race->getAbility(Ability::Perception));
        player->setAbility(Ability::Constitution, player->race->getAbility(Ability::Constitution));
        player->setAbility(Ability::Intelligence, player->race->getAbility(Ability::Intelligence));
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewAttr,
            Formatter::cyan() + "Attribute has been set by default." + Formatter::reset() + "\n");
    }
        // If the player has insert help (attribute number), show its help.
    else if (BeginWith(ToLower(args[0].getContent()), "continue"))
    {
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewGender);
    }
        // If the player has insert help (attribute number), show its help.
    else if (BeginWith(ToLower(args[0].getContent()), "help"))
    {
        if (args.size() == 2)
        {
            std::string helpMessage;
            if (args[1].getContent() == "1")
            {
                helpMessage = "Help about Strength.\n" + Formatter::italic();
                helpMessage += "Strength is important for increasing the Carrying Weight and ";
                helpMessage +=
                    "satisfying the minimum Strength requirements for some weapons and armors.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "2")
            {
                helpMessage = "Help about Agility.\n" + Formatter::italic();
                helpMessage += "Besides increasing mobility in combat, it increases the recharge ";
                helpMessage +=
                    "speed of all the weapons, as well as the ability to use light armor.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "3")
            {
                helpMessage = "Help about Perception.\n" + Formatter::italic();
                helpMessage += "The ability to see, hear, taste and notice unusual things. ";
                helpMessage += "A high Perception is important for a sharpshooter.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "4")
            {
                helpMessage = "Help about Constitution.\n" + Formatter::italic();
                helpMessage += "Stamina and physical toughness. A character with a high Endurance ";
                helpMessage += "will survive where others may not.";
                helpMessage += Formatter::reset() + "\n";
            }
            else if (args[1].getContent() == "5")
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
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewAttr,
                "You have to specify the attribute number.");
        }
    }
    else
    {
        if (args.size() == 2)
        {
            std::string helpMessage;
            int modifier = ToNumber<int>(args[1].getContent());
            // Check for errors.
            if (player->remaining_points < modifier)
            {
                helpMessage = "You don't have enough points left.";
            }
            else if (args[0].getContent() == "1")
            {
                int result = static_cast<int>(player->getAbility(Ability::Strength, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Strength)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Strength)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Strength can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Strength can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Strength, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "2")
            {
                int result = static_cast<int>(player->getAbility(Ability::Agility, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Agility)) + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Agility)) - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Agility can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Agility can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Agility, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "3")
            {
                int result = static_cast<int>(player->getAbility(Ability::Perception, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Perception))
                                 + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Perception))
                                 - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Perception can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Perception can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Perception, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "4")
            {
                int result = static_cast<int>(player->getAbility(Ability::Constitution, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Constitution))
                                 + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Constitution))
                                 - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Constitution can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Constitution can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Constitution, static_cast<unsigned int>(result));
                }
            }
            else if (args[0].getContent() == "5")
            {
                int result = static_cast<int>(player->getAbility(Ability::Intelligence, false))
                             + modifier;
                int upperBound = static_cast<int>(player->race->getAbility(Ability::Intelligence))
                                 + 5;
                int lowerBound = static_cast<int>(player->race->getAbility(Ability::Intelligence))
                                 - 5;
                if (lowerBound < 0)
                {
                    lowerBound = 0;
                }
                if (result < lowerBound)
                {
                    helpMessage = "Constitution can't go below " + ToString(lowerBound) + ".";
                }
                else if (result > upperBound)
                {
                    helpMessage = "Intelligence can't go above " + ToString(upperBound) + ".";
                }
                else
                {
                    player->remaining_points -= modifier;
                    player->setAbility(Ability::Intelligence, static_cast<unsigned int>(result));
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
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewAttr,
                "Type [#attribute +/-(value)].");
        }
    }
}

void ProcessNewGender(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewGender,
            "Not a valid gender.");
    }
}

void ProcessNewAge(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
        int age = ToNumber<int>(input);
        if (age < 18)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewAge,
                "A creature so young is not suitable for a world so wicked.");
        }
        else if (50 < age)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewAge,
                "Life expectancy in this world is 70 years, in order to still be competitive you can choose 50 years at most.");
        }
        else
        {
            player->age = age;
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewDesc);
        }
    }
}

void ProcessNewDesc(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
    else if (input.find_first_not_of(VALID_CHARACTERS_DESC) != std::string::npos)
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewDesc,
            "Description cannot contain disallowed characters.");
    }
    else
    {
        player->description = input;
        AdvanceCharacterCreation(player, ConnectionState::AwaitingNewWeight);
    }
}

void ProcessNewWeight(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
    // Check if the player has typed BACK.
    if (ToLower(input) == "back")
    {
        RollbackCharacterCreation(player, ConnectionState::AwaitingNewDesc);
    }
    else if (!IsNumber(input))
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewWeight,
            "Not a valid weight.");
    }
    else
    {
        int weight = ToNumber<int>(input);
        if (weight < 40)
        {
            AdvanceCharacterCreation(
                character,
                ConnectionState::AwaitingNewWeight,
                "You can't be a feather.");
        }
        else if (weight > 120)
        {
            AdvanceCharacterCreation(character, ConnectionState::AwaitingNewWeight, "Too much.");
        }
        else
        {
            player->weight = static_cast<unsigned int>(weight);
            AdvanceCharacterCreation(player, ConnectionState::AwaitingNewConfirm);
        }
    }
}

void ProcessNewConfirm(Character * character, ArgumentHandler & args)
{
    Player * player = character->toPlayer();
    auto input = args.getOriginal();
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
        SQLiteDbms::instance().beginTransaction();
        if (player->updateOnDB())
        {
            AdvanceCharacterCreation(player, ConnectionState::Playing);
        }
        else
        {
            player->closeConnection();
        }
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        AdvanceCharacterCreation(
            character,
            ConnectionState::AwaitingNewConfirm,
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
