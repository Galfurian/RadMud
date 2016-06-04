/// @file    character.hpp
/// @brief   Define all the methods need to manipulate a character.
/// @details It's the master class for both Player and Mobile, here are defined
///           all the common methods needed to manipulate every dynamic living
///           beeing that are playing.
/// @author  Enrico Fraccaroli
/// @date    Aug 23 2014
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

#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "actions.hpp"
#include "defines.hpp"
#include "effect.hpp"
#include "exit.hpp"
#include "faction.hpp"
#include "item.hpp"
#include "race.hpp"
#include "utils.hpp"

class Room;
class Player;
class Mobile;

/// @brief Character class, father of Player and Mobile.
/// @details
/// It's the main class that contains all the information that both
/// players and npcs shares. In order to allow dynamic casting(polymorphism),
/// i've created a mthod called isMobile, used to identify the subtype of the subclass.
class Character
{
    public:
        /// Character name.
        std::string name;
        /// Character description.
        std::string description;
        /// Character gender.
        GenderType gender;
        /// Character weight.
        int weight;
        /// Character level.
        int level;
        /// Character flags.
        int flags;
        /// The character race.
        Race * race;
        /// The character faction.
        Faction * faction;
        /// Character health value.
        int health;
        /// Character stamina value.
        int stamina;
        /// Character level of hunger
        int hunger;
        /// Character level of thirst.
        int thirst;
        /// Character strength value.
        int strength;
        /// Character agility value.
        int agility;
        /// Character perception value.
        int perception;
        /// Character constitution value.
        int constitution;
        /// Character intelligence value.
        int intelligence;
        /// The current room the character is in.
        Room * room;
        /// Character's inventory.
        ItemVector inventory;
        /// Character's equipment.
        ItemVector equipment;
        /// Character's posture.
        CharacterPosture posture;
        /// Active effects on player.
        EffectList effects;
        /// Character current action.
        Action action;
        /// The lua_State associated with this character.
        lua_State * L;
        /// List of opponents.
        OpponentsList opponents;

        /// @brief Constructor.
        Character();

        /// @brief Destructor.
        virtual ~Character();

        /// @brief Disable copy constructor.
        Character(const Character& source) = delete;

        /// @brief Disable assign operator.
        Character& operator=(const Character&) = delete;

        /// @brief Allows to pass a string which contains the characteristics.
        /// @param source The string which containts the values
        /// @return <b>True</b> if the string is correct,<br>
        ///         <b>False</b> otherwise.
        bool setCharacteristic(const std::string & source);

        /// @brief Check the correctness of the character information.
        /// @return <b>True</b> if the information are correct,<br><b>False</b> otherwise.
        virtual bool check();

        /// @brief Used to identify if this character is an npc.
        /// @return <b>True</b> if is an NPC,<br>
        ///         <b>False</b> otherwise.
        virtual bool isMobile();

        /// @brief Used to identify if this character is a player.
        /// @return <b>True</b> if is an NPC,<br>
        ///         <b>False</b> otherwise.
        virtual bool isPlayer();

        /// @brief Return the name of the character with all lowercase characters.
        /// @return The name of the character.
        std::string getName();

        /// @brief Return the name of the character.
        /// @return The name of the character.
        std::string getNameCapital();

        /// @brief Return the static description of this character.
        /// @return The static description.
        std::string getStaticDesc();

        /// @brief Return the correct pronoun of the character as a string.
        /// @return The character pronoun.
        std::string getPronoun();

        /// @brief Return the correct pronoun of the character as a string.
        /// @return The character pronoun.
        std::string getPossessivePronoun();

        /// @brief Return the max health value.
        /// @return The maximum health for this character.
        int getMaxHealth();

        /// @brief Return the max stamina value.
        /// @return The maximum stamina for this character.
        int getMaxStamina();

        /// @brief Manage the recovering of both health and stamina.
        void updateResources();

        /// @brief Evaluate the maximum distance at which the character can still see.
        /// @return The maximum radius of view.
        int getViewDistance();

        /// @brief Provides a pointer to the Action object associated to this character.
        /// @return A pointer to Action.
        Action * getAction();

        /// @brief Check if the character can move in the given direction, if it's so, returns the room in that direction.
        /// @param direction The direction where to search the room.
        /// @param error     A pointer to a string which will contain the error message if the character cannot go in the given direction.
        /// @return The point to the room in the given direction.
        Room * canMoveTo(Direction direction, std::string & error);

        /// @brief Move the character to another room.
        /// @param destination Destination room.
        /// @param msgDepart   Tell people in original room.
        /// @param msgArrive   Tell people in new room.
        /// @param msgChar     What to tell the character.
        void moveTo(
            Room * destination,
            const std::string & msgDepart,
            const std::string & msgArrive,
            const std::string & msgChar = "");

        /// @brief Search for the item in the inventory.
        /// @param search_parameter The item to search.
        /// @param number           Position of the item we want to look for.
        /// @return The item, if it's in the character's inventory.
        Item * findInventoryItem(std::string search_parameter, int & number);

        /// @brief Search for the item in equipment.
        /// @param search_parameter The item to search.
        /// @param number           Position of the item we want to look for.
        /// @return The item, if it's in the character's equipment.
        Item * findEquipmentItem(std::string search_parameter, int & number);

        /// @brief Search the item at given position and return it.
        /// @param slot The slot where the method need to search the item.
        /// @return The item, if it's in the character's equipment.
        Item * findEquipmentSlotItem(EquipmentSlot slot);

        /// @brief Search the tool in the given equipment slot.
        /// @param slot The slot where the tool can be found.
        /// @param type The type of the tool we are looking for.
        /// @return The tool, if it's in the given slot.
        Item * findEquipmentSlotTool(EquipmentSlot slot, ToolType type);

        /// @brief Search an item nearby, (eq, inv, room).
        /// @param itemName The name of the item.
        /// @param number   Position of the item we want to look for.
        /// @return The item, if it's found.
        Item * findNearbyItem(std::string itemName, int & number);

        /// @brief Search the list of tools nearby the character.
        /// @param tools     The list of tools and their quantities.
        /// @param foundOnes The list of found tools.
        /// @return <b>True</b> if the operation goes well,<br><b>False</b> otherwise.
        bool findNearbyTools(ToolSet tools, ItemVector & foundOnes);

        /// @brief Search the list of ingredients nearby the character.
        /// @param ingredients The list of ingredients and their quantities.
        /// @param foundOnes   The list of found ingredients.
        /// @return <b>True</b> if the operation goes well,<br><b>False</b> otherwise.
        bool findNearbyResouces(IngredientMap ingredients, ItemVector & foundOnes);

        /// @brief Allows to check if an item is inside the inventory.
        /// @param item The item to search.
        /// @return <b>True</b> if the item is inside the inventory,<br><b>False</b> otherwise.
        bool hasInventoryItem(Item * item);

        /// @brief Allows to check if an item is inside the equipment.
        /// @param item The item to search.
        /// @return <b>True</b> if the item is inside the equipment,<br><b>False</b> otherwise.
        bool hasEquipmentItem(Item * item);

        /// @brief Provides the overall armor class.
        /// @return The armor class.
        unsigned int getArmorClass();

        /// @brief Function which checks if the character can attack with a weapon equipped
        ///         in the given slot.
        /// @param slot The slot in which the weapon should be.
        /// @return <b>True</b> if the item is there,<br>
        ///         <b>False</b> otherwise.
        bool canAttackWith(const EquipmentSlot & slot);

        /// @brief Add the passed item to character's inventory.
        /// @param item The item to add to inventory.
        /// @return <b>True</b> if the operation goes well,<br><b>False</b> otherwise.
        bool addInventoryItem(Item * item);

        /// @brief Remove the passed item from the character's inventory.
        /// @param item The item to remove from inventory.
        /// @return <b>True</b> if the operation goes well,<br><b>False</b> otherwise.
        bool remInventoryItem(Item * item);

        /// @brief Check if the player can carry the item.
        /// @param item The item we want to check.
        /// @return <b>True</b> if the character can lift the item,<br><b>False</b> otherwise.
        bool canCarry(Item * item);

        /// @brief The total carrying weight for this character.
        /// @return The total carrying weight.
        int getCarryingWeight();

        /// @brief The maximum carrying weight for this character.
        /// @return The maximum carrying weight.
        int getMaxCarryingWeight();

        /// @brief Equip the passed item.
        /// @param item The item to equip.
        /// @return <b>True</b> if the operation goes well,<br><b>False</b> otherwise.
        bool addEquipmentItem(Item * item);

        /// @brief Remove from current equipment the item.
        /// @param item The item to remove.
        /// @return <b>True</b> if the operation goes well,<br><b>False</b> otherwise.
        virtual bool remEquipmentItem(Item * item);

        /// @brief Check if the character can wield a given item.
        /// @param item    The item to wield.
        /// @param message The error message.
        /// @param where   Where the item has been wielded.
        /// @return <b>True</b> if the operation goes well,<br>
        ///         <b>False</b> otherwise.
        bool canWield(Item * item, std::string & message, EquipmentSlot & where);

        /// @brief Check if the character can wear a given item.
        /// @param item    The item to wear.
        /// @param message The error message.
        /// @return <b>True</b> if the operation goes well,<br>
        ///         <b>False</b> otherwise.
        bool canWear(Item * item, std::string & message);

        /// @brief Get character condition.
        /// @param character The target character.
        /// @return Condition of this character.
        std::string getHealthCondition(Character * character = nullptr);

        /// @brief Get character level of thirst.
        /// @param character The target character.
        /// @return Thirst of this character.
        std::string getThirst(Character * character = nullptr);

        /// @brief Get character level of hunger.
        /// @param character The target character.
        /// @return Hunger of this character.
        std::string getHunger(Character * character = nullptr);

        /// @brief Provide a detailed description of the character.
        /// @param character The target character.
        /// @return A detailed description of the character.
        std::string getLook(Character * character = nullptr);

        /// @brief Check if the current character can see the target character.
        /// @param target The target character.
        /// @return <b>True</b> if the can see the other character,<br>
        ///         <b>False</b> otherwise.
        bool canSee(Character * target);

        Character * getNextOpponent();

        /// @brief Returns the cooldown before next attack.
        /// @return The non-decreasing value of the cooldown.
        unsigned int getNextAttack();

        /// @brief Handle character input.
        /// @param command Command that need to be handled.
        void doCommand(const std::string & command);

        /// @brief Handle what happend when this character die.
        virtual void triggerDeath();

        /// @brief Create a corpse on the ground.
        Item * createCorpse();

        /// @brief Returns the character <b>statically</b> casted to player.
        /// @return The player version of the character.
        Player * toPlayer();

        /// @brief Returns the character <b>statically</b> casted to mobile.
        /// @return The mobile version of the character.
        Mobile * toMobile();

        /// @brief Returns the list of available targets using the vector
        ///         structure made for lua environment.
        /// @return The vector of targets.
        VectorHelper<Character *> luaGetTargets();

        /// @brief Returns the list of available exits from the current room
        ///         using the vector structure made for lua environment.
        /// @return The vector of exits.
        VectorHelper<Exit *> luaGetExits();

        /// @brief Starts a lua environment and loads the given script.
        /// @param scriptFilename The name of the script that has to be loaded.
        void loadScript(const std::string & scriptFilename);

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);

        /// @brief Operator used to order the character based on their name.
        bool operator<(const class Character & source) const
            {
            return name < source.name;
        }

        /// @brief Send a message to the character.
        /// @param msg Message to send.
        virtual void sendMsg(const std::string & msg);

        /// @brief Print to consol and to logging file the gievn string.
        /// @param  level  The category of the message.
        /// @param  log    The message to log.
        /// @tparam T
        template<typename ... Args>
        void sendMsg(
            const std::string & msg,
            const std::string & first,
            const Args & ... args)
        {
            std::string::size_type pos = msg.find("%s");
            if (pos == std::string::npos)
            {
                sendMsg(msg);
            }
            else
            {
                std::string working(msg);
                working.replace(pos, 2, first);
                sendMsg(working, args ...);
            }
        }
};

/// @brief Character list handler.
typedef std::vector<Character *> CharacterList;

/// @brief An action handler for the character.
typedef std::function<void(Character * character, std::istream & args)> ActionHandler;

/// @brief Map of things to do for various connection states.
typedef std::map<ConnectionState, ActionHandler> StateActionMap;

#endif
