/// @file   command.hpp
/// @brief  Define the methods used by the character in order to execute commands.
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

#pragma once

#include "../character.hpp"
#include "../constants.hpp"
#include "../profession.hpp"
#include <iosfwd>

#include "argumentHandler.hpp"

/// @brief An action handler for the character.
typedef std::function<void(Character * character, ArgumentHandler & args)> ActionHandler;

/// @brief Contains all the informations concerning a command, including its handler.
class Command
{
    public:
        Command() :
                level(),
                name(),
                help(),
                args(),
                hndl()
        {
        }

        /// @brief Create a complete structure for a command.
        /// @param _level The level necessary to execute the command.
        /// @param _name  The name of the command.
        /// @param _help  The help message of the command.
        /// @param _args  The arguments of the command.
        /// @param _hndl  The handler of the command.
        Command(
            int _level,
            std::string _name,
            std::string _help,
            std::string _args,
            ActionHandler _hndl) :
                level(_level),
                name(_name),
                help(_help),
                args(_args),
                hndl(_hndl)
        {
        }

        ~Command()
        {
        }

        /// @brief Checks if the provided character can use the command.
        /// @return <b>True</b> if the character can use the command,<br>
        ///         <b>False</b> otherwise.
        bool canUse(Character * character) const
        {
            if (this->level == 1)
            {
                if (!HasFlag(character->flags, CharacterFlag::IsGod))
                {
                    return false;
                }
            }
            return true;
        }

        /// The level of the command.
        int level;
        /// The name of the command.
        std::string name;
        /// The help message of the command.
        std::string help;
        /// The arguemtns of the command.
        std::string args;
        /// The handler of the command.
        ActionHandler hndl;
};

/// @defgroup ProcessStates Player state processing.
/// @brief All the functions necessary to process players commands, from creation to gameplay.
/// @{

/// @brief Process commands when character is connected.
/// @param character The character that execute the command.
/// @param sArgs  Command arguments.
void ProcessCommand(Character * character, ArgumentHandler & args);
/// Check player name.
void ProcessPlayerName(Character * character, ArgumentHandler & args);
/// Check if the player password is correct.
void ProcessPlayerPassword(Character * character, ArgumentHandler & args);
/// Step 1  - Choose the Name.
void ProcessNewName(Character * character, ArgumentHandler & args);
/// Step 2  - Choose the Password.
void ProcessNewPwd(Character * character, ArgumentHandler & args);
/// Step 3  - Confirm the Password.
void ProcessNewPwdCon(Character * character, ArgumentHandler & args);
/// Step 4  - Short story of the mud world.
void ProcessNewStory(Character * character, ArgumentHandler & args);
/// Step 5  - Choose the Race.
void ProcessNewRace(Character * character, ArgumentHandler & args);
/// Step 6  - Choose the Attributes.
void ProcessNewAttr(Character * character, ArgumentHandler & args);
/// Step 7  - Choose the Gender.
void ProcessNewGender(Character * character, ArgumentHandler & args);
/// Step 8  - Choose the Age.
void ProcessNewAge(Character * character, ArgumentHandler & args);
/// Step 9  - Choose the description (optional).
void ProcessNewDesc(Character * character, ArgumentHandler & args);
/// Step 10 - Choose the Weight.
void ProcessNewWeight(Character * character, ArgumentHandler & args);
/// Step 11 - Confirm the character.
void ProcessNewConfirm(Character * character, ArgumentHandler & args);
///@}

/// @brief Check if the executer of this command is a player.
void NoMobile(Character * character);

/// @brief Stop any action the character is executing.
void StopAction(Character * character);

/// @brief Load all the possible player states.
void LoadStates();

/// @brief Print the values inserted until now.
/// @param character The player whose creating a new character.
void PrintChoices(Character * character);

/// @brief Reset the informations inserted in the previous state.
/// @param character The player whose creating a new character.
/// @param new_state The step reached by this player.
void RollbackCharacterCreation(Character * character, ConnectionState new_state);

/// @brief Print the advancement in the character creation.
/// @details If you want to introduce a new step, you have to insert it's text here.
/// @param character The player whose creating a new character.
/// @param con_state The step reached by this player.
/// @param message   An optional message used only during error handling.
void AdvanceCharacterCreation(
    Character * character,
    ConnectionState con_state,
    std::string message = "");

/// @brief Map all the command to the respective std::string that the character can type.
void LoadCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// Execute character movement.
void DoDirection(Character * character, Direction direction);
/// Allow the character to mote the continental map.
void DoTravel(Character * character, ArgumentHandler & args);
/// Disconnect the player from the Mud.
void DoQuit(Character * character, ArgumentHandler & args);
/// Shows a list of all the online players.
void DoWho(Character * character, ArgumentHandler & args);
/// Allow to modify the character informations.
void DoSet(Character * character, ArgumentHandler & args);
/// Stop the current action of the character.
void DoStop(Character * character, ArgumentHandler & args);
/// The character look at: the current room, the near rooms, an object, a player, a character, and so on.
void DoLook(Character * character, ArgumentHandler & args);
/// Provide a complete help on the mud.
void DoHelp(Character * character, ArgumentHandler & args);
/// Allow character to modify his prompt.
void DoPrompt(Character * character, ArgumentHandler & args);
/// Report the current day phase.
void DoTime(Character * character, ArgumentHandler & args);
/// Make the player stand up.
void DoStand(Character * character, ArgumentHandler & args);
/// Make the player crouch.
void DoCrouch(Character * character, ArgumentHandler & args);
/// Make the player sit.
void DoSit(Character * character, ArgumentHandler & args);
/// Make the player prone.
void DoProne(Character * character, ArgumentHandler & args);
/// Make the player lie down and rest.
void DoRest(Character * character, ArgumentHandler & args);
/// Show player statistics.
void DoStatistics(Character * character, ArgumentHandler & args);
/// Allow player to rent and disconnect.
void DoRent(Character * character, ArgumentHandler & args);
/// Show player skills.
void DoSkills(Character * character, ArgumentHandler & args);
/// Show server informations and statistics.
void DoServer(Character * character, ArgumentHandler & args);
/// Allow the character to pick up an object, or take an object from another one.
void DoTake(Character * character, ArgumentHandler & args);
/// Allow the character to drop an object.
void DoDrop(Character * character, ArgumentHandler & args);
/// Allow the character to give an object to another character.
void DoGive(Character * character, ArgumentHandler & args);
/// List all the item equipped by the character.
void DoEquipments(Character * character, ArgumentHandler & args);
/// Wield a object, if it can be wielded.
void DoWield(Character * character, ArgumentHandler & args);
/// Wear a object, if it can be weared.
void DoWear(Character * character, ArgumentHandler & args);
/// Remove an equipped or wielded item.
void DoRemove(Character * character, ArgumentHandler & args);
/// List the character's inventory.
void DoInventory(Character * character, ArgumentHandler & args);
/// Organize the items in the room.
void DoOrganize(Character * character, ArgumentHandler & args);
/// Open a door if not unlocked.
void DoOpen(Character * character, ArgumentHandler & args);
/// Close a door.
void DoClose(Character * character, ArgumentHandler & args);
/// Put an object inside a container.
void DoPut(Character * character, ArgumentHandler & args);
/// Drink from a container of liquids.
void DoDrink(Character * character, ArgumentHandler & args);
/// Fill a container of liquids from a source of liquid.
void DoFill(Character * character, ArgumentHandler & args);
/// Pour the container of liquids into another one or on the ground.
void DoPour(Character * character, ArgumentHandler & args);
/// Deposit a currency inside a shop.
void DoDeposit(Character * character, ArgumentHandler & args);
/// Sell an item.
void DoSell(Character * character, ArgumentHandler & args);
/// Buy an item.
void DoBuy(Character * character, ArgumentHandler & args);
/// Shows the total ammount of monney.
void DoBalance(Character * character, ArgumentHandler & args);
/// The character say something.
void DoSay(Character * character, ArgumentHandler & args);
/// The character whisper something to someone, even in distance.
void DoWhisper(Character * character, ArgumentHandler & args);
/// The character emote something.
void DoEmote(Character * character, ArgumentHandler & args);
/// The character report a bug.
void DoBug(Character * character, ArgumentHandler & args);
/// The character send an idea.
void DoIdea(Character * character, ArgumentHandler & args);
/// The character report a typo.
void DoTypo(Character * character, ArgumentHandler & args);
/// The character read a book.
void DoRead(Character * character, ArgumentHandler & args);
/// Engage in combat the desired target.
void DoKill(Character * character, ArgumentHandler & args);
/// Try to flee from combat.
void DoFlee(Character * character, ArgumentHandler & args);
/// Provides the list of targets nearby.
void DoAim(Character * character, ArgumentHandler & args);
/// Shutdown the Mud.
void DoShutdown(Character * character, ArgumentHandler & args);
/// Save the Mud.
void DoMudSave(Character * character, ArgumentHandler & args);
/// Go to the desired room.
void DoGoTo(Character * character, ArgumentHandler & args);
/// Transfer a character from room to room.
void DoTransfer(Character * character, ArgumentHandler & args);
/// Regain completely health and stamina.
void DoFeast(Character * character, ArgumentHandler & args);
/// Get information about a character.
void DoGodInfo(Character * character, ArgumentHandler & args);
/// Set a flag to a character.
void DoSetFlag(Character * character, ArgumentHandler & args);
/// Remove a flag from the character.
void DoClearFlag(Character * character, ArgumentHandler & args);
/// Show all the information fo the desired model.
void DoModelInfo(Character * character, ArgumentHandler & args);
/// Generate a new item from the vnum of a model.
void DoItemCreate(Character * character, ArgumentHandler & args);
/// Materialize an item from everywere.
void DoItemGet(Character * character, ArgumentHandler & args);
/// Destroy an item.
void DoItemDestroy(Character * character, ArgumentHandler & args);
/// Get information about an item.
void DoItemInfo(Character * character, ArgumentHandler & args);
/// Show the informations about an area.
void DoAreaInfo(Character * character, ArgumentHandler & args);
/// Show the informations about a room.
void DoRoomInfo(Character * character, ArgumentHandler & args);
/// Create a room in the given direction.
void DoRoomCreate(Character * character, ArgumentHandler & args);
/// Delete a room in the given direction.
void DoRoomDelete(Character * character, ArgumentHandler & args);
/// Edit a room name or description.
void DoRoomEdit(Character * character, ArgumentHandler & args);
/// Kill the desired mobile, in the same room.
void DoMobileKill(Character * character, ArgumentHandler & args);
/// Reload the lua script for the target mobile, in the same room.
void DoMobileReload(Character * character, ArgumentHandler & args);
/// Show the mobile's log.
void DoMobileLog(Character * character, ArgumentHandler & args);
/// Hurt the desired target.
void DoHurt(Character * character, ArgumentHandler & args);
/// Became invisible.
void DoInvisibility(Character * character, ArgumentHandler & args);
/// Return visible.
void DoVisible(Character * character, ArgumentHandler & args);
/// Modify the value of the player skill.
void DoModSkill(Character * character, ArgumentHandler & args);
/// Modify the value of the player attribute.
void DoModAttr(Character * character, ArgumentHandler & args);
/// Provides the list of opponents of the given target.
void DoAggroList(Character * character, ArgumentHandler & args);
/// Provide all the information regarding the given material.
void DoMaterialInfo(Character * character, ArgumentHandler & args);
/// Create the item with the given liquid inside a container.
void DoLiquidCreate(Character * character, ArgumentHandler & args);
/// Provide all the information regarding the given liquid.
void DoLiquidInfo(Character * character, ArgumentHandler & args);
/// Provide all the information regarding the given production.
void DoProductionInfo(Character * character, ArgumentHandler & args);
/// Provide all the information regarding the given profession.
void DoProfessionInfo(Character * character, ArgumentHandler & args);
/// Shows the infos about a faction.
void DoFactionInfo(Character * character, ArgumentHandler & args);
/// List all the model used to define items.
void DoModelList(Character * character, ArgumentHandler & args);
/// List all the items in the Mud.
void DoItemList(Character * character, ArgumentHandler & args);
/// List all the mobiles.
void DoMobileList(Character * character, ArgumentHandler & args);
/// List all the players.
void DoPlayerList(Character * character, ArgumentHandler & args);
/// List all the areas.
void DoAreaList(Character * character, ArgumentHandler & args);
/// List all the rooms.
void DoRoomList(Character * character, ArgumentHandler & args);
/// List all the races.
void DoRaceList(Character * character, ArgumentHandler & args);
/// List all the factions.
void DoFactionList(Character * character, ArgumentHandler & args);
/// List all the skills.
void DoSkillList(Character * character, ArgumentHandler & args);
/// List all the writings.
void DoWritingList(Character * character, ArgumentHandler & args);
/// List all the corpses
void DoCorpseList(Character * character, ArgumentHandler & args);
/// List all the continents.
void DoContinentList(Character * character, ArgumentHandler & args);
/// Get the list of materials.
void DoMaterialList(Character * character, ArgumentHandler & args);
/// Get the list of all the professions.
void DoProfessionList(Character * character, ArgumentHandler & args);
/// Get the list of all the productions.
void DoProductionList(Character * character, ArgumentHandler & args);
/// Get the list of liquids.
void DoLiquidList(Character * character, ArgumentHandler & args);
/// Get the list of buildings.
void DoBuildingList(Character * character, ArgumentHandler & args);
/// Execute a profession.
void DoProfession(Character * character, Profession * profession, ArgumentHandler & args);
/// Build something.
void DoBuild(Character * character, ArgumentHandler & args);
/// Deconstruct something.
void DoDeconstruct(Character * character, ArgumentHandler & args);
/// Allows to assign a mobile to a task/building.
void DoAssign(Character * character, ArgumentHandler & args);
///@}
