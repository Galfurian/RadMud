/// @file   communication.cpp
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

#include "command.hpp"
#include "../mud.hpp"

using namespace std;

void DoSay(Character * character, std::istream & sArgs)
{
    // //////////////////////////////////////////
    // Retrieve the message.
    std::string message;
    getline(sArgs, message);
    if (message.empty())
    {
        character->sendMsg("My dear friend, say what?\n");
        return;
    }

    // //////////////////////////////////////////
    // Retrieve the target, if the first word is one.
    std::string target = "";
    {
        std::vector<std::string> arguments = GetWords(message);
        if (arguments.size() <= 0)
        {
            character->sendMsg("My dear friend, say what?\n");
            return;
        }
        target = arguments[0];
    }

    // //////////////////////////////////////////
    // Extract the number, if the character has provided one.
    int number = 1;
    ExtractNumber(target, number);

    // //////////////////////////////////////////
    // Check if the speaker is invisible.
    std::string chName =
        (HasFlag(character->flags, CharacterFlag::Invisible)) ?
            "Someone" : character->getNameCapital();

    // //////////////////////////////////////////
    // Check if the character are talking to another character.
    Character * receiver = character->room->findCharacter(target, number, { character });
    if (receiver != nullptr)
    {
        // Get the rest of the message, minus the first word.
        message = message.substr(target.size(), message.size());
        if (message.empty())
        {
            character->sendMsg("My dear friend, say what to %s?\n", receiver->getName());
            return;
        }
        // Eat the space between the name and the message.
        message = Trim(message, " ");
        // Player send.
        character->sendMsg(
            "You say to %s, \"%s\"\n",
            receiver->getName(),
            Formatter::cyan() + Formatter::italic() + message + Formatter::reset());

        // Target receive.
        receiver->sendMsg(
            "%s say to you, \"%s\"\n\n",
            chName,
            Formatter::cyan() + Formatter::italic() + message + Formatter::reset());

        // Send the message inside the room.
        character->room->sendToAll(
            "%s says to %s, \"%s\".\n",
            { character, receiver },
            chName,
            receiver->getName(),
            Formatter::cyan() + Formatter::italic() + message + Formatter::reset());
        // If it's a mobile, activate the trigger.
        if (receiver->isMobile())
        {
            receiver->toMobile()->triggerEventMessage(character, message);
        }
    }
    else
    {
        character->sendMsg(
            "You say \"%s\".\n",
            Formatter::cyan() + Formatter::italic() + message + Formatter::reset());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s says \"%s\".\n",
            { character },
            chName,
            Formatter::cyan() + Formatter::italic() + message + Formatter::reset());
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
    std::string chName =
        (HasFlag(character->flags, CharacterFlag::Invisible)) ?
            "Someone" : character->getNameCapital();

    // //////////////////////////////////////////
    // Check the existance of the target character.
    Character * receiver = character->room->findCharacter(target, number, { character });
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
        "%sYou whisper to %s, %s\"%s\".\n",
        Formatter::magenta(),
        receiver->getName(),
        Formatter::reset(),
        message);
    receiver->sendMsg(
        "%s whisper to you, %s\"%s\"\n\n",
        Formatter::magenta() + chName,
        Formatter::reset(),
        message);
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
    std::string chName =
        (HasFlag(character->flags, CharacterFlag::Invisible)) ?
            "Someone" : character->getNameCapital();

    // //////////////////////////////////////////
    // Send the emote.
    character->sendMsg("%sYou %s\n", Formatter::yellow(), emote + Formatter::reset());
    // Send the message inside the room.
    character->room->sendToAll(
        "%s %s\n",
        { character },
        Formatter::yellow() + chName,
        emote + Formatter::reset());
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

    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Board", arguments))
    {
        character->sendMsg("Something gone wrong during the storing of your bug.\n");
        SQLiteDbms::instance().rollbackTransection();
        return;
    }
    SQLiteDbms::instance().endTransaction();
    character->sendMsg("Bug posted on Board correctly.\n");
    character->sendMsg("# Author   :%s\n", character->getName());
    character->sendMsg("# Date     :%s\n", GetDate());
    character->sendMsg("# Location :%s\n", character->room->name);
    character->sendMsg("# Message  :%s\n", message);
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

    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Board", arguments))
    {
        character->sendMsg("Something gone wrong during the storing of your idea.\n");
        SQLiteDbms::instance().rollbackTransection();
        return;
    }
    SQLiteDbms::instance().endTransaction();

    character->sendMsg("Idea posted on Board correctly.\n");
    character->sendMsg("# Author   :%s\n", character->getName());
    character->sendMsg("# Message  :%s\n", message);
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

    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Board", arguments))
    {
        character->sendMsg("Something gone wrong during the storing of the Typo.\n");
        SQLiteDbms::instance().rollbackTransection();
        return;
    }
    SQLiteDbms::instance().endTransaction();

    character->sendMsg("Typo posted on Board correctly.\n");
    character->sendMsg("# Author   :%s\n", character->getName());
    character->sendMsg("# Date     :%s\n", GetDate());
    character->sendMsg("# Location :%s\n", character->room->name);
    character->sendMsg("# Message  :%s\n", message);
}
