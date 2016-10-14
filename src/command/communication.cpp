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

#include "communication.hpp"

#include "command.hpp"
#include "../mud.hpp"

void LoadCommunicationCommands()
{
    Command command;
    command.gods = false;
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
}

void DoSay(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("My dear friend, say what?\n");
        return;
    }
    // Check if the character are talking to another character.
    auto receiver = character->room->findCharacter(
        args.get(0).getContent(),
        args.get(0).getIndex(),
        {character});
    if (receiver != nullptr)
    {
        // Get the rest of the message, minus the first word.
        auto message = args.substr(1);
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
            character->getName(),
            Formatter::cyan() + Formatter::italic() + message + Formatter::reset());

        // Send the message inside the room.
        character->room->sendToAll(
            "%s says to %s, \"%s\".\n",
            {character, receiver},
            character->getName(),
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
            Formatter::cyan() + Formatter::italic() + args.getOriginal() + Formatter::reset());
        // Send the message inside the room.
        character->room->sendToAll(
            "%s says \"%s\".\n",
            {character},
            character->getName(),
            Formatter::cyan() + Formatter::italic() + args.getOriginal() + Formatter::reset());
    }
}

void DoWhisper(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Whisper to whom?\n");
        return;
    }
    // Check the existance of the target character.
    auto receiver = character->room->findCharacter(args[0].getContent(), args[0].getIndex(), {
        character});
    if (receiver == nullptr)
    {
        character->sendMsg("You don't see %s here.\n", args[0].getContent());
        return;
    }
    // Retrieve the message.
    auto message = args.substr(1);
    if (message.empty())
    {
        character->sendMsg("What do you want to whisper to %s?\n", receiver->getName());
        return;
    }
    // Check if the sender is invisible.
    auto sender = (receiver->canSee(character)) ? "Someone" : character->getNameCapital();
    // Send the message.
    character->sendMsg(
        "%sYou whisper to %s, %s\"%s\".\n",
        Formatter::magenta(),
        receiver->getName(),
        Formatter::reset(),
        message);
    receiver->sendMsg(
        "%s whisper to you, %s\"%s\"\n\n",
        Formatter::magenta() + sender,
        Formatter::reset(),
        message);
}

void DoEmote(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("My dear friend, emote what?\n");
        return;
    }
    // Send the messages.
    character->sendMsg("%sYou %s\n", Formatter::yellow(), args.getOriginal() + Formatter::reset());
    character->room->sendToAll(
        "%s %s\n",
        {character},
        Formatter::yellow() + character->getName(),
        args.getOriginal() + Formatter::reset());
}

void DoBug(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Writing nothing won't help us!\n");
        return;
    }
    std::vector<std::string> arguments;
    arguments.push_back(character->getName()); // Author
    arguments.push_back(GetDate()); // Date.
    arguments.push_back("0"); // Type
    arguments.push_back(ToString(character->room->vnum)); // Location
    arguments.push_back(args.getOriginal());
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
    character->sendMsg("# Message  :%s\n", args.getOriginal());
}

void DoIdea(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Writing nothing won't help us!\n");
        return;
    }
    std::vector<std::string> arguments;
    arguments.push_back(character->getName()); // Author
    arguments.push_back(GetDate()); // Date.
    arguments.push_back("1"); // Type
    arguments.push_back(ToString(character->room->vnum)); // Location
    arguments.push_back(args.getOriginal());
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
    character->sendMsg("# Message  :%s\n", args.getOriginal());
}

void DoTypo(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        character->sendMsg("Writing nothing won't help us!\n");
        return;
    }
    std::vector<std::string> arguments;
    arguments.push_back(character->getName()); // Author
    arguments.push_back(GetDate()); // Date.
    arguments.push_back("2"); // Type
    arguments.push_back(ToString(character->room->vnum)); // Location
    arguments.push_back(args.getOriginal());

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
    character->sendMsg("# Message  :%s\n", args.getOriginal());
}
