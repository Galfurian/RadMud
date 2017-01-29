/// @file   commandGodCharacter.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 02 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "commandGodCharacter.hpp"
#include "characterUtilities.hpp"
#include "mud.hpp"

bool DoGodInfo(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide player name.\n");
        return false;
    }
    Character * target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        target = Mud::instance().findMobile(args[0].getContent());
        if (target == nullptr)
        {
            std::string msgFound;
            for (auto it : Mud::instance().mudMobiles)
            {
                if (BeginWith(it.first, args[0].getContent()))
                {
                    if (msgFound.empty())
                    {
                        msgFound += "Maybe you mean:\n";
                    }
                    msgFound += "    " + it.first + "\n";
                }
            }
            character->sendMsg("Mobile not found.\n" + msgFound);
            return false;
        }
    }
    // Create a table.
    Table sheet;
    // Get the sheet.
    target->getSheet(sheet);
    // Show the seet to character.
    character->sendMsg(sheet.getTable());
    return true;
}

bool DoTransfer(Character * character, ArgumentHandler & args)
{
    if ((args.size() != 1) && (args.size() != 2))
    {
        character->sendMsg("You have to select at least a target and"
                               " optionally a destination.\n");
        return false;
    }
    Character * target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        target = Mud::instance().findMobile(args[0].getContent());
        if (target == nullptr)
        {
            character->sendMsg("Can't find the target character.\n");
            return false;
        }
    }
    if (target->isMobile())
    {
        if (!target->toMobile()->isAlive())
        {
            character->sendMsg("You cannot transfer a dead mobile.\n");
            return false;
        }
    }
    auto destination = character->room;
    if (args.size() == 2)
    {
        auto roomVnum = ToNumber<int>(args[1].getContent());
        destination = character->room->area->getRoom(roomVnum);
        if (destination == nullptr)
        {
            character->sendMsg("Can't find the room %s setting this room.\n",
                               ToString(roomVnum));
            return false;
        }
    }
    // Stop any action the character is executing.
    StopAction(target);
    // Prepare messages.
    // Move player.
    MoveCharacterTo(
        target,
        destination,
        target->getNameCapital() + " is yanked away by unseen forces!",
        target->getNameCapital() + " appears breathlessly!",
        "\n" + character->getNameCapital() +
        " transfers you to another room!\n");
    character->sendMsg("You transfer %s to room %s.\n", target->getName(),
                       destination->name);
    return true;
}

bool DoHurt(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("Who do you want to hurt?\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(),
                                                 args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    // Set health to 1.
    target->setHealth(1);
    // Notify.
    character->sendMsg("You point your finger, %s cry in pain.\n",
                       target->getName());
    target->sendMsg("%s points the finger towards you, you cry in pain.\n",
                    character->getNameCapital());
    // Send the message inside the room.
    target->room->sendToAll(
        "%s points the finger towards %s, %s cries in pain.\n",
        {character, target},
        character->getNameCapital(),
        target->getName(),
        target->getSubjectPronoun());
    return true;
}

bool DoAggroList(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide character name.\n");
        return false;
    }
    Character * targer = Mud::instance().findPlayer(args[0].getContent());
    if (targer == nullptr)
    {
        targer = Mud::instance().findMobile(args[0].getContent());
        if (targer == nullptr)
        {
            character->sendMsg("Character not found.\n");
            return false;
        }
    }
    for (auto aggressor : targer->combatHandler)
    {
        if (aggressor->aggressor != nullptr)
        {
            character->sendMsg("%s\t%s",
                               ToString(aggressor->aggression),
                               aggressor->aggressor->getNameCapital());
        }
    }
    return true;
}

bool DoFeast(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        character->sendMsg("You must insert a valide target.\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(),
                                                 args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("You must provide a valide target.\n");
        return false;
    }
    target->setHealth(target->getMaxHealth(), true);
    target->setStamina(target->getMaxStamina(), true);
    target->thirst = 100;
    target->hunger = 100;
    target->sendMsg(
        "%sA banquet with any kind of delicacy appears from nowhere!%s%s\n",
        Formatter::magenta(), Formatter::reset(),
        ((target != character) ? "\n" : ""));
    return true;
}

bool DoInvisible(Character * character, ArgumentHandler & /*args*/)
{
    // Check if already invisible.
    if (HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("Don't worry, you are already invisible.\n");
        return false;
    }
    // Set the character invisible.
    SetFlag(&character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are invisible now.\n");
    return true;
}

bool DoVisible(Character * character, ArgumentHandler & /*args*/)
{
    // Check if already visible.
    if (!HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("You are not invisible.\n");
        return false;
    }
    // Set the character visible.
    ClearFlag(&character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are no more invisible.\n");
    return true;
}

bool DoPlayerList(Character * character, ArgumentHandler & /*args*/)
{
    Table table;
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ROOM", StringAlign::Left);
    for (auto iterator : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(iterator->name);
        if (iterator->room != nullptr)
        {
            row.push_back(iterator->room->name);
        }
        else
        {
            row.push_back("Nowhere!");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoPlayerSetFlag(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return false;
    }
    auto target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '%s'.\n",
                           args[0].getContent());
        return false;
    }
    auto flag = static_cast<CharacterFlag>(ToNumber<int>(args[1].getContent()));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return false;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag already set.\n");
        return false;
    }
    // Set the flag.
    SetFlag(&target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You set the flag '%s' for %s\n",
                       GetCharacterFlagName(flag), target->getName());
    return true;
}

bool DoPlayerClearFlag(Character * character, ArgumentHandler & args)
{
    if (args.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return false;
    }
    auto target = Mud::instance().findPlayer(args[0].getContent());
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '%s'.\n",
                           args[0].getContent());
        return false;
    }
    auto flag = static_cast<CharacterFlag>(ToNumber<int>(args[1].getContent()));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return false;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag not set.\n");
        return false;
    }
    // Set the flag.
    ClearFlag(&target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You clear the flag '%s' for %s\n",
                       GetCharacterFlagName(flag), target->getName());
    return true;
}

bool DoPlayerModSkill(Character * character, ArgumentHandler & args)
{
    if (args.size() != 3)
    {
        character->sendMsg("Usage: [target] [#skill] [+/-VALUE]\n");
        return false;
    }
    auto target = character->room->findPlayer(args[0].getContent(),
                                              args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    auto skill = Mud::instance().findSkill(ToNumber<int>(args[1].getContent()));
    if (skill == nullptr)
    {
        character->sendMsg("Cannot find the desired skill.\n");
        return false;
    }
    int modifier = ToNumber<int>(args[2].getContent());
    if (modifier == 0)
    {
        character->sendMsg("You must insert a valid modifier.\n");
        return false;
    }
    auto skillEntry = target->skills.find(skill->vnum);
    if (skillEntry == target->skills.end())
    {
        character->sendMsg("%s does not possess that skill.\n",
                           target->getNameCapital());
        return false;
    }
    auto modified = static_cast<int>(skillEntry->second) + modifier;
    if (modified <= 0)
    {
        character->sendMsg("You cannot reduce the skill <= 0 (%s).\n",
                           modified);
        return false;
    }
    auto skillCap = SkillRank::getSkillCap();
    if (modified >= static_cast<int>(skillCap))
    {
        modified = static_cast<int>(skillCap);
    }
    // Change the skill value.
    skillEntry->second = static_cast<unsigned int>(modified);
    // Notify.
    character->sendMsg("You have successfully %s by %s the \"%s\" skill,"
                           "the new level is %s.\n",
                       ((modifier > 0) ? "increased " : "decreased"),
                       modifier,
                       skill->name,
                       skillEntry->second);
    Skill::updateSkillEffects(character);
    Skill::checkIfUnlockedSkills(character);
    return true;
}

bool DoPlayerModAttr(Character * character, ArgumentHandler & args)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Check the number of arguments.
    if (args.size() != 3)
    {
        character->sendMsg("Usage: [target] [attribute] [+/-VALUE]\n");
        return false;
    }
    auto target = character->room->findCharacter(args[0].getContent(),
                                                 args[0].getIndex());
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return false;
    }
    auto modifier = ToNumber<int>(args[2].getContent());
    if (modifier == 0)
    {
        character->sendMsg("You must insert a valid value.\n");
        return false;
    }
    Ability ability;
    if (args[1].getContent() == "str")
    {
        ability = Ability::Strength;
    }
    else if (args[1].getContent() == "agi")
    {
        ability = Ability::Agility;
    }
    else if (args[1].getContent() == "per")
    {
        ability = Ability::Perception;
    }
    else if (args[1].getContent() == "con")
    {
        ability = Ability::Constitution;
    }
    else if (args[1].getContent() == "int")
    {
        ability = Ability::Intelligence;
    }
    else
    {
        character->sendMsg("Bad attribute name, accepted :"
                               "(str, agi, per, con, int).\n");
        return false;
    }
    // Get the resulting ability value.
    auto result = static_cast<int>(target->getAbility(ability)) + modifier;
    if (result < 0)
    {
        character->sendMsg("Attribute cannot go below 0.");
        return false;
    }
    else if (!target->setAbility(ability, static_cast<unsigned int>(result)))
    {
        character->sendMsg("Attribute cannot go above 60.");
        return false;
    }
    character->sendMsg("You have successfully %s by %s the %s of the target.",
                       std::string((modifier > 0) ? "increased" : "decreased"),
                       ToString(modifier),
                       ability.toString());
    return true;
}
