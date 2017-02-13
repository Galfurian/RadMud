/// @file   general.cpp
/// @brief  Implements <b>general commands</b> used by the player.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "general.hpp"

#include "nameGenerator.hpp"
#include "skillRank.hpp"
#include "logger.hpp"
#include "mud.hpp"

void LoadGeneralCommands()
{
    Mud::instance().addCommand(std::make_shared<Command>(
        DoQuit, "quit", "",
        "Leave the game.",
        false, false, true));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoWho, "who", "",
        "List all the character online.",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSet, "set", "(setting) (value)",
        "Set some character texts(eg. descr).",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoLook, "look", "[(something) or (someone)]",
        "Look at something or someone.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoHelp, "help", "(command)",
        "Show the list of commands or show help for a given command.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoPrompt, "prompt", "(help)|(prompt definition)",
        "Modify your prompt.",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoTime, "time", "",
        "Give the current day phase.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoStatistics, "statistics", "",
        "Show player statistics.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoEffects, "effects", "",
        "Show player active effects.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoRent, "rent", "",
        "Allow player to rent and disconnect.",
        false, false, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoSkills, "skills", "",
        "Shows the playes skills and their level.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoServer, "server", "",
        "Shows the server statistics.",
        false, true, false));
    Mud::instance().addCommand(std::make_shared<Command>(
        DoGenerateName, "generate_name", "",
        "Generates a random name.",
        false, true, false));
}

bool DoQuit(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is a mobile.
    if (character->isMobile())
    {
        character->sendMsg("Npcs are not allowed to execute this command.\n");
        return false;
    }
    // Stop any ongoing action.
    StopAction(character);
    auto player = character->toPlayer();
    // If s/he finished connecting, tell others s/he has left.
    if (player->connectionState == ConnectionState::Playing)
    {
        // Say goodbye to player.
        player->sendMsg("See you next time!\n");
        if (player->room != nullptr)
        {
            // Send the message inside the room.
            player->room->sendToAll(
                "Player %s disappear in a puff of smoke.\n",
                {character},
                player->getName());
        }
        Logger::log(LogLevel::Global, "Player %s has left the game.",
                    player->getName());
        // End of properly connected.
        player->closeConnection();
        return true;
    }
    return false;
}

bool DoWho(Character * character, ArgumentHandler & /*args*/)
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
        std::string location = "Nowhere";
        if (iterator->room != nullptr)
        {
            location = iterator->room->name;
        }
        table.addRow({iterator->getName(), location});
    }
    character->sendMsg(table.getTable());
    character->sendMsg(
        "# Total %sPlayers%s: %s\n",
        Formatter::yellow(),
        Formatter::reset(),
        ToString(table.getNumRows()));
    return true;
}

bool DoSet(Character * character, ArgumentHandler & args)
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
                return true;
            }
        }
    }
    character->sendMsg("Usage: set <opt> <arguments>  \n");
    character->sendMsg("       ---------------------  \n");
    character->sendMsg("            des  [description]\n");
    return false;
}

bool DoLook(Character * character, ArgumentHandler & args)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You can't see anything, you're sleeping.\n");
        return false;
    }
    // If there are no arguments, show the room.
    if (args.empty())
    {
        character->sendMsg(character->room->getLook(character));
        return true;
    }
    if (args.size() == 1)
    {
        // Check if the room is lit by a light.
        if (!character->room->isLit())
        {
            character->sendMsg("You can't see.\n");
            return false;
        }
        // Check if the character is looking at another character.
        auto target = character->room->findCharacter(args[0].getContent(),
                                                     args[0].getIndex(),
                                                     {character});
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
                    target->sendMsg("%s looks at you.\n\n",
                                    character->getNameCapital());
                }
                return true;
            }
        }
        // Check if the character is looking at an item.
        auto item = character->findNearbyItem(args[0].getContent(),
                                              args[0].getIndex());
        if (item)
        {
            character->sendMsg(item->getLook());
            return true;
        }
        character->sendMsg("You don't see '%s' anywhere.\n",
                           args[0].getContent());
    }
    if (args.size() == 2)
    {
        // Check if the room is lit by a light.
        if (!character->room->isLit())
        {
            character->sendMsg("You can't see.\n");
            return false;
        }
        auto target = character->room->findCharacter(args[1].getContent(),
                                                     args[1].getIndex(),
                                                     {character});
        if (target != nullptr)
        {
            if (character->canSee(target))
            {
                auto item = target->findEquipmentItem(args[0].getContent(),
                                                      args[0].getIndex());
                if (item != nullptr)
                {
                    character->sendMsg(item->getLook());
                    return true;
                }
                character->sendMsg("You don't see %s on %s.\n",
                                   args[0].getContent(),
                                   target->getName());
                return false;
            }
            character->sendMsg(
                "You don't see the container '%s' anywhere.\n",
                args[1].getContent());
            return false;
        }
        auto container = character->findNearbyItem(args[1].getContent(),
                                                   args[1].getIndex());
        if (container)
        {
            if (container->isAContainer())
            {
                auto item = container->findContent(args[0].getContent(),
                                                   args[0].getIndex());
                if (item)
                {
                    character->sendMsg(item->getLook());
                    return true;
                }
                character->sendMsg("It's not inside %s.\n",
                                   container->getName(true));
                return false;
            }
            character->sendMsg("%s is not a container.\n",
                               container->getNameCapital(true));
            return false;
        }
        character->sendMsg("You don't see the container '%s' anywhere.\n",
                           args[1].getContent());
    }
    if (args.size() > 2)
    {
        character->sendMsg("You don't remember how to look?\n");
    }
    return false;
}

bool DoHelp(Character * character, ArgumentHandler & args)
{
    if (args.empty())
    {
        size_t numColumns = 4;
        TableRow baseRow, godsRow;
        Table baseCommands("Commands"), godsCommands("Management");
        for (size_t it = 0; it < numColumns; ++it)
        {
            baseCommands.addColumn("", StringAlign::Left, 15);
            godsCommands.addColumn("", StringAlign::Left, 15);
        }
        for (auto it : Mud::instance().mudCommands)
        {
            if (it->canUse(character))
            {
                if (it->gods)
                {
                    godsRow.push_back(it->name);
                    if (godsRow.size() == numColumns)
                    {
                        godsCommands.addRow(godsRow);
                        godsRow.clear();
                    }
                }
                else
                {
                    baseRow.push_back(it->name);
                    if (baseRow.size() == numColumns)
                    {
                        baseCommands.addRow(baseRow);
                        baseRow.clear();
                    }
                }
            }
        }
        if (!godsRow.empty())
        {
            for (auto it = godsRow.size(); it < 4; ++it)
            {
                godsRow.emplace_back("");
            }
            godsCommands.addRow(godsRow);
        }
        if (!baseRow.empty())
        {
            for (auto it = baseRow.size(); it < 4; ++it)
            {
                baseRow.emplace_back("");
            }
            baseCommands.addRow(baseRow);
        }
        character->sendMsg(baseCommands.getTable(true));
        character->sendMsg(godsCommands.getTable(true));
    }
    else if (args.size() == 1)
    {
        auto command = args[0].getContent();
        for (auto it : Mud::instance().mudCommands)
        {
            if (it->canUse(character))
            {
                if (BeginWith(ToLower(it->name), command))
                {
                    std::string msg;
                    msg += "Showing help for command :" + it->name + "\n";
                    msg += Formatter::yellow() + " Command   : " +
                           Formatter::reset()
                           + it->name + "\n";
                    msg += Formatter::yellow() + " Level     : " +
                           Formatter::reset()
                           + ToString(it->gods) + "\n";
                    msg += Formatter::yellow() + " Arguments : " +
                           Formatter::reset()
                           + it->arguments + "\n";
                    msg += Formatter::yellow() + " Help      : " +
                           Formatter::reset()
                           + it->help + "\n";
                    character->sendMsg(msg);
                    return true;
                }
            }
        }
        character->sendMsg("There is no help for '%s'.\n", args.getOriginal());
        return false;
    }
    return true;
}

bool DoPrompt(Character * character, ArgumentHandler & args)
{
    // Check if the character is a mobile.
    if (character->isMobile())
    {
        character->sendMsg("Npcs are not allowed to execute this command.\n");
        return false;
    }
    auto player = character->toPlayer();
    if (args.empty())
    {
        character->sendMsg("Current prompt:\n");
        character->sendMsg("    %s\n", player->prompt);
        character->sendMsg("Type %sprompt help %sto read the guide.\n",
                           Formatter::yellow(), Formatter::reset());
    }
    else
    {
        if (args[0].getContent() == "help")
        {
            auto yellow = [](std::string s)
            {
                return Formatter::yellow() + s + Formatter::reset();
            };
            auto italic = [](std::string s)
            {
                return Formatter::italic() + s + Formatter::reset();
            };
            std::string msg;
            msg += yellow("Prompt Help") + "\n";
            msg += "You can set the prompt you prefer, respectfully to this constraints:\n";
            msg += " - Not more than 15 characters.\n";
            msg += "\n";
            msg += "You can use the following shortcuts in you prompt:\n";
            msg += "    " + italic("&n") + " - Player name.\n";
            msg += "    " + italic("&N") + " - Player name capitalized.\n";
            msg += "    " + italic("&h") + " - Player current health.\n";
            msg += "    " + italic("&H") + " - Player maximum health.\n";
            msg += "    " + italic("&s") + " - Player current stamina.\n";
            msg += "    " + italic("&S") + " - Player maximum stamina.\n";
            msg += "    " + italic("&T") + " - Currently aimed character.\n";
            player->sendMsg(msg);
        }
        else
        {
            player->prompt = args.substr(0);
        }
    }
    return true;
}

bool DoTime(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You can't see anything, you're sleeping.\n");
        return false;
    }
    if (MudUpdater::instance().getDayPhase() == DayPhase::Morning)
    {
        character->sendMsg("%sThe sun has just risen.%s\n",
                           Formatter::yellow(),
                           Formatter::reset());
    }
    else if (MudUpdater::instance().getDayPhase() == DayPhase::Day)
    {
        character->sendMsg("%sThe sun is high in the sky.%s\n",
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
        character->sendMsg("%sThe darkness surrounds you.%s\n",
                           Formatter::blue(),
                           Formatter::reset());
    }
    return true;
}

bool DoStatistics(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is a mobile.
    if (character->isMobile())
    {
        character->sendMsg("Npcs are not allowed to execute this command.\n");
        return false;
    }
    auto player = character->toPlayer();

    auto BLD = [](std::string s)
    {
        return Formatter::bold() + s + Formatter::reset();
    };
    auto MAG = [](std::string s)
    {
        return Formatter::magenta() + s + Formatter::reset();
    };
    std::string msg;
    msg += MAG("Name        : ") + player->getName() + " ";
    msg += MAG("Gender : ") + GetGenderTypeName(player->gender) + " ";
    msg += MAG("Race : ") + player->race->name + "\n";
    msg += MAG("Weight      : ") + ToString(player->weight) + " ";
    msg += Mud::instance().getWeightMeasure() + "\n";
    msg += MAG("Affiliation : ") + player->faction->name + "\n";
    msg += MAG("Experience  : ") + ToString(player->experience) + " px\n";
    // Add the abilities.
    // STRENGTH
    for (auto const & ability:player->abilities)
    {
        msg += "    " + MAG(ability.first.getAbbreviation(true)) + "    ";
        msg += AlignString(player->getAbility(ability.first),
                           StringAlign::Right, 5) + "(";
        msg += AlignString(player->getAbilityModifier(ability.first),
                           StringAlign::Right, 3) + ")\n";
    }
    // HEALTH
    msg += "    " + MAG("Health  ");
    msg += ToString(player->health) + "/" + ToString(player->getMaxHealth());
    msg += "\n";
    msg += "    " + MAG("Stamina ");
    msg += ToString(player->stamina) + "/" + ToString(player->getMaxStamina());
    msg += "\n";
    // Add the Armor Class.
    msg += "    " + MAG("Armor Class ") + ToString(player->getArmorClass());
    msg += "\n";
    // Add the health and stamina conditions.
    msg += "You " + BLD(player->getHealthCondition(true)) + ".\n";
    msg += "You " + BLD(player->getStaminaCondition()) + ".\n";
    // Add the hunger and thirst conditions.
    msg += "You " + BLD(player->getHungerCondition()) + ".\n";
    msg += "You " + BLD(player->getThirstCondition()) + ".\n";
    // Add the posture.
    msg += "You are " + BLD(player->posture.getAction()) + ".\n\n";
    // [IF EXIST] Add the current action.
    if (player->getAction()->getType() != ActionType::Wait)
    {
        msg += "You are ";
        msg += BLD(player->getAction()->getDescription());
        msg += ".\n";
    }
    // [IF EXIST] Add the current opponent.
    if (player->combatHandler.getPredefinedTarget() != nullptr)
    {
        msg += "You are fighting with ";
        msg += BLD(player->combatHandler.getPredefinedTarget()->getName());
        msg += ".\n";
    }
    // [IF EXIST] Add the current aimed opponent.
    if (player->combatHandler.getAimedTarget() != nullptr)
    {
        msg += "You are aiming at ";
        msg += BLD(player->combatHandler.getAimedTarget()->getName());
        msg += ".\n";
    }
    player->sendMsg(msg);
    return true;
}

bool DoEffects(Character * character, ArgumentHandler &)
{
    std::string msg;
    for (auto const & effect : character->effects.getActiveEffects())
    {
        msg += AlignString(effect.name, StringAlign::Left, 30) + "\n";
    }
    for (auto const & effect : character->effects.getPassiveEffects())
    {
        msg += AlignString(effect.name, StringAlign::Left, 30) + "\n";
    }
    for (auto const & effect : character->effects.getActiveAbilityModifier())
    {
        msg += AlignString(effect.first.getAbbreviation(),
                           StringAlign::Left, 30);
        msg += AlignString(effect.second, StringAlign::Right, 5) + "\n";
    }
    for (auto const & effect : character->effects.getActiveCombatModifier())
    {
        msg += AlignString(effect.first.toString(),
                           StringAlign::Left, 30);
        msg += AlignString(effect.second, StringAlign::Right, 5) + "\n";
    }
    for (auto const & effect : character->effects.getActiveStatusModifier())
    {
        msg += AlignString(effect.first.toString(),
                           StringAlign::Left, 30);
        msg += AlignString(effect.second, StringAlign::Right, 5) + "\n";
    }
    for (auto const & effect : character->effects.getActiveKnowledge())
    {
        msg += AlignString(effect.first.toString(),
                           StringAlign::Left, 30);
        msg += AlignString(effect.second, StringAlign::Right, 5) + "\n";
    }
    character->sendMsg(msg);
    return true;
}

bool DoRent(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is a mobile.
    if (character->isMobile())
    {
        character->sendMsg("Npcs are not allowed to execute this command.\n");
        return false;
    }
    // Check if the character is sleeping.
    if (character->posture == CharacterPosture::Sleep)
    {
        character->sendMsg("You can't see anything, you're sleeping.\n");
        return false;
    }
    auto player = character->toPlayer();
    // Check if the room allow to rent.
    if (!HasFlag(player->room->flags, RoomFlag::Rent))
    {
        character->sendMsg("You can't rent here.\n");
        return false;
    }
    else
    {
        player->rent_room = player->room->vnum;
        player->doCommand("quit");
    }
    return true;
}

bool DoSkills(Character * character, ArgumentHandler & /*args*/)
{
    // Check if the character is a mobile.
    if (character->isMobile())
    {
        character->sendMsg("Npcs are not allowed to execute this command.\n");
        return false;
    }
    auto player = character->toPlayer();
    Table table = Table();
    table.addColumn("LvL", StringAlign::Left);
    table.addColumn("Skill", StringAlign::Left);
    for (auto it : player->skills)
    {
        auto skill = Mud::instance().findSkill(it.first);
        if (skill)
        {
            table.addRow({skill->name,
                          SkillRank::getSkillRank(it.second).toString()});
        }
    }
    character->sendMsg(table.getTable());
    return true;
}

bool DoServer(Character * character, ArgumentHandler & /*args*/)
{
    std::string msg;
    msg += "Mud         : RadMud.\n";
    msg += "Version     : ";
    msg += ToString(RADMUD_MAJOR_VERSION) + ".";
    msg += ToString(RADMUD_MINOR_VERSION) + ".";
    msg += ToString(RADMUD_VERSION) + "\n";
    msg += "    Uptime      : ";
    msg += ToString(Mud::instance().getUpTime()) + " s\n";
    msg += "    Players     : ";
    msg += ToString(Mud::instance().mudPlayers.size()) + "\n";
    msg += "    Mobiles     : ";
    msg += ToString(Mud::instance().mudMobiles.size()) + "\n";
    msg += "    Models      : ";
    msg += ToString(Mud::instance().mudItemModels.size()) + "\n";
    msg += "    Items       : ";
    msg += ToString(Mud::instance().mudItems.size()) + "\n";
    msg += "    Corpses     : ";
    msg += ToString(Mud::instance().mudCorpses.size()) + "\n";
    msg += "    Areas       : ";
    msg += ToString(Mud::instance().mudAreas.size()) + "\n";
    msg += "    Rooms       : ";
    msg += ToString(Mud::instance().mudRooms.size()) + "\n";
    msg += "    Races       : ";
    msg += ToString(Mud::instance().mudRaces.size()) + "\n";
    msg += "    Factions    : ";
    msg += ToString(Mud::instance().mudFactions.size()) + "\n";
    msg += "    Skills      : ";
    msg += ToString(Mud::instance().mudSkills.size()) + "\n";
    msg += "    Writings    : ";
    msg += ToString(Mud::instance().mudWritings.size()) + "\n";
    msg += "    Materials   : ";
    msg += ToString(Mud::instance().mudMaterials.size()) + "\n";
    msg += "    Professions : ";
    msg += ToString(Mud::instance().mudProfessions.size()) + "\n";
    msg += "    Productions : ";
    msg += ToString(Mud::instance().mudProductions.size()) + "\n";
    msg += "    Schematics  : ";
    msg += ToString(Mud::instance().mudBuildings.size()) + "\n";
    msg += "    Liquids     : ";
    msg += ToString(Mud::instance().mudLiquids.size()) + "\n";
    msg += "    News        : ";
    msg += ToString(Mud::instance().mudNews.size()) + "\n";
    msg += "    Commands    : ";
    msg += ToString(Mud::instance().mudCommands.size()) + "\n";
    character->sendMsg(msg);
    return true;
}

bool DoGenerateName(Character * character, ArgumentHandler & args)
{
    if (args.size() != 1)
    {
        std::string help;
        help += "You must provide a pattern.\n";
        help += "Guide\n";
        help += "\ts - generic syllable\n";
        help += "\tv - vowel\n";
        help += "\tV - vowel or vowel combination\n";
        help += "\tc - consonant\n";
        help += "\tB - consonant or consonant combination suitable for beginning a word\n";
        help += "\tC - consonant or consonant combination suitable anywhere in a word\n";
        help += "\ti - insult\n";
        help += "\tm - mushy name\n";
        help += "\tM - mushy name ending\n";
        help += "\tD - consonant suited for a stupid person's name\n";
        help += "\td - syllable suited for a stupid person's name (begins with a vowel)\n";
        help += "\n";
        help += "All characters between parenthesis () are emitted literally.\n";
        help += "  For example, the pattern 's(dim)', emits a random generic\n";
        help += "  syllable followed by 'dim'.\n";
        help += "Characters between angle brackets <> emit patterns from\n";
        help += "  the table above. Imagine the entire pattern is wrapped in\n";
        help += "  one of these.\n";
        help += "In both types of groupings, a vertical bar | denotes a\n";
        help += "  random choice. Empty groups are allowed.\n";
        help += "  For example, '(foo|bar)' emits either 'foo' or 'bar'.\n";
        help += "  The pattern '<c|v|>' emits a constant, vowel, or\n";
        help += "  nothing at all.\n";
        help += "An exclamation point ! means to capitalize the component \n";
        help += "  that follows it. For example, '!(foo)' will emit 'Foo'\n";
        help += "  and 'v!s' will emit a lowercase vowel followed by a\n";
        help += "  capitalized syllable, like 'eRod'.\n";
        help += "A tilde ~ means to reverse the letters of the component\n";
        help += "  that follows it. For example, '~(foo)' will emit 'oof'.\n";
        help += "  To reverse an entire template, wrap it in brackets.\n";
        help += "  For example, to reverse 'sV'i' as a whole use '~<sV'i>'.\n";
        help += "  The template '~sV'i' will only reverse the initial\n";
        help += "  syllable.\n";
        character->sendMsg(help + "\n");
        return false;
    }
    namegen::NameGenerator nameGenerator(args[0].getContent());
    character->sendMsg("Combinations : %s\n", nameGenerator.combinations());
    character->sendMsg("Names : %s\n\n", nameGenerator.toString());
    return true;
}
