/// @file   comm_communication.cpp
/// @brief  Implements the methods used by the player in order to <b>communicate</b>.
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

#include "commands.hpp"
// Other Include.
#include "mud.hpp"

using namespace std;

void DoSay(Character * character, std::istream & sArgs)
{
    // //////////////////////////////////////////
    // Retrieve the message.
    std::string message;
    getline(sArgs, message);
    if (message.empty())
    {
        throw std::runtime_error("My dear friend, say what?\n");
    }

    // //////////////////////////////////////////
    // Retrieve the target, if the first word is one.
    std::string target = "";
    {
        std::vector<std::string> arguments = GetWords(message);
        if (arguments.size() <= 0)
        {
            throw std::runtime_error("My dear friend, say what?\n");
        }
        target = arguments[0];
    }

    // //////////////////////////////////////////
    // Extract the number, if the character has provided one.
    int number = 1;
    ExtractNumber(target, number);

    // //////////////////////////////////////////
    // Check if the speaker is invisible.
    std::string chName = (HasFlag(character->flags, CharacterFlag::Invisible)) ? "Someone" : character->getNameCapital();

    // //////////////////////////////////////////
    // Check if the character are talking to another character.
    Character * receiver = character->room->findCharacter(target, number, character);
    if (receiver != nullptr)
    {
        // Get the rest of the message, minus the first word.
        message = message.substr(target.size(), message.size());
        if (message.empty())
        {
            throw std::runtime_error("My dear friend, say what to " + receiver->getName() + "?\n");
        }
        // Eat the space between the name and the message.
        message = Trim(message, " ");
        // Player send.
        character->sendMsg(
            "You say to " + receiver->getName() + ", \"" + Telnet::cyan() + Telnet::italic() + message + Telnet::reset()
                + "\"\n");

        // Target receive.
        receiver->sendMsg(
            chName + " say to you, \"" + Telnet::cyan() + Telnet::italic() + message + Telnet::reset() + "\"\n\n");

        // Other in room.
        character->room->sendToAll(
            chName + " says to " + receiver->getName() + ", \"" + Telnet::cyan() + Telnet::italic() + message
                + Telnet::reset() + "\".\n", character, receiver);

        // If it's a mobile, activate the trigger.
        if (receiver->isMobile())
        {
            receiver->toMobile()->triggerEventMessage(character, message);
        }
    }
    else
    {
        character->sendMsg("You say \"" + Telnet::cyan() + Telnet::italic() + message + Telnet::reset() + "\".\n");
        character->room->sendToAll(
            chName + " says \"" + Telnet::cyan() + Telnet::italic() + message + Telnet::reset() + "\".\n", character);
    }
}

void DoWhisper(Character * character, std::istream & sArgs)
{
    // //////////////////////////////////////////
    // Retrieve the target.
    std::string target = "";
    sArgs >> target >> std::ws;
    if (target.empty())
    {
        throw std::runtime_error("Whisper to whom?\n");
    }

    // //////////////////////////////////////////
    // Extract the number, if the character has provided one.
    int number = 1;
    ExtractNumber(target, number);

    // //////////////////////////////////////////
    // Check if the speaker is invisible.
    std::string chName = (HasFlag(character->flags, CharacterFlag::Invisible)) ? "Someone" : character->getNameCapital();

    // //////////////////////////////////////////
    // Check the existance of the target character.
    Character * receiver = character->room->findCharacter(target, number, character);
    if (receiver == nullptr)
    {
        throw std::runtime_error("You don't see " + target + " here.\n");
    }

    // //////////////////////////////////////////
    // Retrieve the message.
    std::string message;
    getline(sArgs, message);
    if (message.empty())
    {
        throw std::runtime_error("Whisper to " + target + " what?\n");
    }

    // //////////////////////////////////////////
    // Send the message.
    character->sendMsg(
        Telnet::magenta() + "You whisper to " + receiver->getName() + ", " + Telnet::reset() + "\"" + message + "\"\n");
    receiver->sendMsg(Telnet::magenta() + chName + " whisper to you, " + Telnet::reset() + "\"" + message + "\"\n\n");
}

void DoEmote(Character * character, std::istream & sArgs)
{
    // //////////////////////////////////////////
    // Retrieve the emote.
    std::string emote;
    getline(sArgs, emote);
    if (emote.empty())
    {
        character->sendMsg("Emote what?\n");
        return;
    }

    // //////////////////////////////////////////
    // Check if the character is invisible.
    std::string chName = (HasFlag(character->flags, CharacterFlag::Invisible)) ? "Someone" : character->getNameCapital();

    // //////////////////////////////////////////
    // Send the emote.
    character->sendMsg(Telnet::yellow() + "You " + emote + Telnet::reset() + "\n");
    character->room->sendToAll(Telnet::yellow() + chName + " " + emote + Telnet::reset() + "\n", character);
}

void DoBug(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    vector<string> arguments;
    string message;

    getline(sArgs, message);

    if (message.empty())
    {
        character->sendMsg("Writing nothing won't help us!\n");
        return;
    }

    arguments.push_back(character->getName()); // Author
    arguments.push_back(GetDate()); // Date.
    arguments.push_back("0"); // Type
    arguments.push_back(ToString(character->room->vnum)); // Location
    arguments.push_back(message);

    if (!Mud::getInstance().getDbms().insertInto("Board", arguments))
    {
        character->sendMsg("Something gone wrong during the storing of your bug.\n");
        return;
    }
    character->sendMsg("Bug posted on Board correctly.\n");
    character->sendMsg("# Author   :" + character->getName() + "\n");
    character->sendMsg("# Date     :" + GetDate() + "\n");
    character->sendMsg("# Location :" + character->room->name + "\n");
    character->sendMsg("# Message  :" + message + "\n");
}

void DoIdea(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    vector<string> arguments;
    string message;

    getline(sArgs, message);

    if (message.empty())
    {
        character->sendMsg("Give us some ideas!\n");
        return;
    }

    arguments.push_back(character->getName()); // Author
    arguments.push_back(GetDate()); // Date.
    arguments.push_back("1"); // Type
    arguments.push_back(ToString(character->room->vnum)); // Location
    arguments.push_back(message);

    if (!Mud::getInstance().getDbms().insertInto("Board", arguments))
    {
        character->sendMsg("Something gone wrong during the storing of your idea.\n");
        return;
    }
    character->sendMsg("Idea posted on Board correctly.\n");
    character->sendMsg("# Author   :" + character->getName() + "\n");
    character->sendMsg("# Message  :" + message + "\n");
}

void DoTypo(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    vector<string> arguments;
    string message;

    getline(sArgs, message);

    if (message.empty())
    {
        character->sendMsg("Writing nothing won't help us!\n");
        return;
    }

    arguments.push_back(character->getName()); // Author
    arguments.push_back(GetDate()); // Date.
    arguments.push_back("2"); // Type
    arguments.push_back(ToString(character->room->vnum)); // Location
    arguments.push_back(message);

    if (!Mud::getInstance().getDbms().insertInto("Board", arguments))
    {
        character->sendMsg("Something gone wrong during the storing of the Typo.\n");
        return;
    }
    character->sendMsg("Typo posted on Board correctly.\n");
    character->sendMsg("# Author   :" + character->getName() + "\n");
    character->sendMsg("# Date     :" + GetDate() + "\n");
    character->sendMsg("# Location :" + character->room->name + "\n");
    character->sendMsg("# Message  :" + message + "\n");
}
