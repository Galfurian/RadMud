/// @file    character.hpp
/// @brief   Define all the methods need to manipulate a character.
/// @details It's the master class for both Player and Mobile, here are defined
///           all the common methods needed to manipulate every dynamic living
///           beeing that are playing.
/// @author  Enrico Fraccaroli
/// @date    Aug 23 2014
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

#pragma once

#include "defines.hpp"
#include "exit.hpp"
#include "race.hpp"
#include "item.hpp"
#include "faction.hpp"
#include "effectList.hpp"
#include "combatAction.hpp"
#include "argumentHandler.hpp"
#include "characterContainer.hpp"

#include <deque>

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
    unsigned int weight;
    /// Character level.
    unsigned int level;
    /// Character flags.
    unsigned int flags;
    /// The character race.
    Race * race;
    /// The character faction.
    Faction * faction;
    /// Character health value.
    unsigned int health;
    /// Character stamina value.
    unsigned int stamina;
    /// Character level of hunger
    unsigned int hunger;
    /// Character level of thirst.
    unsigned int thirst;
    /// Character abilities.
    std::map<Ability, unsigned int> abilities;
    /// The current room the character is in.
    Room * room;
    /// Character's inventory.
    ItemContainer inventory;
    /// Character's equipment.
    ItemContainer equipment;
    /// Character's posture.
    CharacterPosture posture;
    /// Active effects on player.
    EffectList effects;
    /// The lua_State associated with this character.
    lua_State * L;
    /// List of opponents.
    CombatHandler combatHandler;
    /// Character current action.
    std::deque<std::shared_ptr<GeneralAction> > actionQueue;
    /// List of characters in sight.
    CharacterContainer charactersInSight;

    /// @brief Constructor.
    Character();

    /// @brief Destructor.
    virtual ~Character();

    /// @brief Disable copy constructor.
    Character(const Character & source) = delete;

    /// @brief Disable assign operator.
    Character & operator=(const Character &) = delete;

    /// @brief Check the correctness of the character information.
    /// @return <b>True</b> if the information are correct,<br>
    ///         <b>False</b> otherwise.
    virtual bool check() const;

    /// @brief Used to identify if this character is an npc.
    /// @return <b>True</b> if is an NPC,<br>
    ///         <b>False</b> otherwise.
    virtual bool isMobile() const;

    /// @brief Used to identify if this character is a player.
    /// @return <b>True</b> if is an NPC,<br>
    ///         <b>False</b> otherwise.
    virtual bool isPlayer() const;

    /// @brief Fills the provided table with the information concerning the character.
    /// @param sheet The table that has to be filled.
    virtual void getSheet(Table & sheet) const;

    /// @brief Return the name of the character with all lowercase characters.
    /// @return The name of the character.
    std::string getName() const;

    /// @brief Return the name of the character.
    /// @return The name of the character.
    std::string getNameCapital() const;

    /// @brief Return the static description of this character.
    /// @return The static description.
    std::string getStaticDesc() const;

    /// @brief Return the subject pronoun for the character.
    std::string getSubjectPronoun() const;

    /// @brief Return the possessive pronoun for the character.
    std::string getPossessivePronoun() const;

    /// @brief Return the object pronoun for the character.
    std::string getObjectPronoun() const;

    /// @brief Allows to set the value of a given ability.
    /// @param ability The ability to set.
    /// @param value   The value to set.
    /// @return <b>True</b> if the value is correct,<br>
    ///         <b>False</b> otherwise.
    bool setAbility(const Ability & ability, const unsigned int & value);

    /// @brief Provides the value of the given ability.
    /// @param ability The ability to retrieve.
    /// @param withEffects If set to false, this function just return the
    ///                     ability value without the contribution due to
    ///                     the active effects.
    /// @return The overall ability value.
    unsigned int getAbility(const Ability & ability, bool withEffects = true) const;

    /// @brief Provides the modifier of the given ability.
    /// @param ability The ability of which the modifier has to be
    ///                 retrieved.
    /// @param withEffects If set to false, this function just return the
    ///                     ability modifier without the contribution due
    ///                     to the active effects.
    /// @return The overall ability modifer.
    unsigned int getAbilityModifier(const Ability & ability, bool withEffects = true) const;

    /// @brief Provides the base ten logarithm of the desired ability
    ///         modifier, multiplied by an optional multiplier. Also,
    ///         a base value can be provided.
    /// @details Value = Base + (Multiplier * log10(AbilityModifier))
    /// @param ability The ability of which the modifier has to be
    ///                 retrieved.
    /// @param base The base value to which the evaluated modifier is summed.
    /// @param multiplier The log10 modifer is multiplied by this value.
    /// @param withEffects If set to false, this function just return the
    ///                     ability modifier without the contribution due
    ///                     to the active effects.
    /// @return The overall base ten logarithm of the given ability modifer.
    unsigned int getAbilityLog(
        const Ability & ability,
        const double & base,
        const double & multiplier,
        const bool & withEffects = true) const;

    /// @brief Allows to SET the health value.
    /// @param value The value to set.
    /// @param force <b>True</b> if the value is greather than the maximum
    ///               the function set the health to it and
    ///               returns true,<br>
    ///              <b>False</b> return false.
    /// @return <b>True</b> if the function has set the value,<br>
    ///         <b>False</b> otherwise.
    bool setHealth(const unsigned int & value, const bool & force = false);

    /// @brief Allows to ADD a value to the current health value.
    /// @param value The value to add.
    /// @param force <b>True</b> if the resulting value is greather than
    ///               the maximum the function set the health to it and
    ///               returns true,<br>
    ///              <b>False</b> return false.
    /// @return <b>True</b> if the function has added the value,<br>
    ///         <b>False</b> otherwise.
    bool addHealth(const unsigned int & value, const bool & force = false);

    /// @brief Allows to REMOVE a value to the current health value.
    /// @param value The value to remove.
    /// @param force <b>True</b> if the resulting value is lesser than
    ///               zero the function set the health to it and
    ///               returns true,<br>
    ///              <b>False</b> return false.
    /// @return <b>True</b> if the function has removed the value,<br>
    ///         <b>False</b> otherwise.
    bool remHealth(const unsigned int & value, const bool & force = false);

    /// @brief Get character level of health.
    /// @return Health of this character.
    unsigned int getHealth() const;

    /// @brief Return the max health value.
    /// @param withEffects <b>True</b> also add the health due to effects,<br>
    ///                    <b>False</b> otherwise.
    /// @return The maximum health for this character.
    unsigned int getMaxHealth(bool withEffects = true) const;

    /// @brief Get character condition.
    /// @param self If the sentence has to be for another character or not.
    /// @return Condition of this character.
    std::string getHealthCondition(const bool & self = false);

    /// @brief Allows to SET the stamina value.
    /// @param value The value to set.
    /// @param force <b>True</b> if the value is greather than the maximum
    ///               the function set the stamina to it and
    ///               returns true,<br>
    ///              <b>False</b> return false.
    /// @return <b>True</b> if the function has set the value,<br>
    ///         <b>False</b> otherwise.
    bool setStamina(const unsigned int & value, const bool & force = false);

    /// @brief Allows to ADD a value to the current stamina value.
    /// @param value The value to add.
    /// @param force <b>True</b> if the resulting value is greather than
    ///               the maximum the function set the stamina to it and
    ///               returns true,<br>
    ///              <b>False</b> return false.
    /// @return <b>True</b> if the function has added the value,<br>
    ///         <b>False</b> otherwise.
    bool addStamina(const unsigned int & value, const bool & force = false);

    /// @brief Allows to REMOVE a value to the current stamina value.
    /// @param value The value to remove.
    /// @param force <b>True</b> if the resulting value is lesser than
    ///               zero the function set the stamina to it and
    ///               returns true,<br>
    ///              <b>False</b> return false.
    /// @return <b>True</b> if the function has removed the value,<br>
    ///         <b>False</b> otherwise.
    bool remStamina(const unsigned int & value, const bool & force = false);

    /// @brief Get character level of stamina.
    /// @return Stamina of this character.
    unsigned int getStamina() const;

    /// @brief Return the max stamina value.
    /// @param withEffects  <b>True</b> also add the stamina due to effects,<br>
    ///                     <b>False</b> otherwise.
    /// @return The maximum stamina for this character.
    unsigned int getMaxStamina(bool withEffects = true) const;

    /// @brief Get the character stamina condition.
    /// @return Stamina condition of the character.
    std::string getStaminaCondition();

    /// @brief Evaluate the maximum distance at which the character can still see.
    /// @return The maximum radius of view.
    unsigned int getViewDistance() const;

    /// @brief Allows to set an action.
    /// @param _action The action that has to be set.
    void setAction(std::shared_ptr<GeneralAction> _action);

    /// @brief Allows to set a combat action.
    /// @param nextAction The next combat action to execute.
    /// @return <b>True</b> if correct values have been provided,<br>
    ///         <b>False</b> otherwise.
    bool setNextCombatAction(const CombatActionType & nextAction);

    /// @brief Provides a pointer to the action object associated to this character.
    /// @return A pointer to action.
    std::shared_ptr<GeneralAction> getAction() const;

    /// @brief Provides a pointer to the action at the front position and
    ///         then remove it from the queue.
    void popAction();

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
    Item * findEquipmentSlotItem(EquipmentSlot slot) const;

    /// @brief Search the tool in the given equipment slot.
    /// @param slot The slot where the tool can be found.
    /// @param type The type of the tool we are looking for.
    /// @return The tool, if it's in the given slot.
    Item * findEquipmentSlotTool(EquipmentSlot slot, ToolType type);

    /// @brief Search an item nearby, (eq, inv, room).
    /// @param itemName The name of the item.
    /// @param number   Position of the item we want to look for.
    /// @return The item, if it's found.
    Item * findNearbyItem(const std::string & itemName, int & number);

    /// @brief Search the given type of tool in the proximity of the character.
    /// @param toolType        The type of tool that has to be searched.
    /// @param exceptions      Items which cannot be selected.
    /// @param searchRoom      Search inside the room.
    /// @param searchInventory Search inside the character's inventory.
    /// @param searchEquipment Search inside the character's equipment.
    /// @return The searched tool.
    Item * findNearbyTool(
        const ToolType & toolType,
        const std::vector<Item *> & exceptions,
        bool searchRoom,
        bool searchInventory,
        bool searchEquipment);

    /// @brief Search the list of tools nearby the character.
    /// @param tools           The list of tools and their quantities.
    /// @param foundOnes       The list of found tools.
    /// @param searchRoom      Search inside the room.
    /// @param searchInventory Search inside the character's inventory.
    /// @param searchEquipment Search inside the character's equipment.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    bool findNearbyTools(
        std::set<ToolType> tools,
        std::vector<Item *> & foundOnes,
        bool searchRoom,
        bool searchInventory,
        bool searchEquipment);

    /// @brief Search the list of ingredients nearby the character.
    /// @param ingredients The list of ingredients and their quantities.
    /// @param foundOnes   The list of found ingredients.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    bool findNearbyResouces(
        std::map<ResourceType, unsigned int> ingredients,
        std::vector<std::pair<Item *, unsigned int>> & foundOnes);

    /// @brief Search the coins on the character.
    /// @return List of found coins.
    std::vector<Item *> findCoins();

    /// @brief Allows to check if an item is inside the inventory.
    /// @param item The item to search.
    /// @return <b>True</b> if the item is inside the inventory,<br><b>False</b> otherwise.
    bool hasInventoryItem(Item * item);

    /// @brief Allows to check if an item is inside the equipment.
    /// @param item The item to search.
    /// @return <b>True</b> if the item is inside the equipment,<br><b>False</b> otherwise.
    bool hasEquipmentItem(Item * item);

    /// @brief Add the passed item to character's inventory.
    /// @param item The item to add to inventory.
    virtual void addInventoryItem(Item *& item);

    /// @brief Equip the passed item.
    /// @param item The item to equip.
    virtual void addEquipmentItem(Item *& item);

    /// @brief Remove the passed item from the character's inventory.
    /// @param item The item to remove from inventory.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    virtual bool remInventoryItem(Item * item);

    /// @brief Remove from current equipment the item.
    /// @param item The item to remove.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    virtual bool remEquipmentItem(Item * item);

    /// @brief Check if the player can carry the item.
    /// @param item     The item we want to check.
    /// @param quantity The ammount of item to check (by default it is 1).
    /// @return <b>True</b> if the character can lift the item,<br><b>False</b> otherwise.
    bool canCarry(Item * item, unsigned int quantity) const;

    /// @brief The total carrying weight for this character.
    /// @return The total carrying weight.
    double getCarryingWeight() const;

    /// @brief The maximum carrying weight for this character.
    /// @return The maximum carrying weight.
    double getMaxCarryingWeight() const;

    /// @brief Check if the character can wield a given item.
    /// @param item  The item to wield.
    /// @param error The error message.
    /// @param where Where the item has been wielded.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    bool canWield(Item * item, std::string & error, EquipmentSlot & where) const;

    /// @brief Check if the character can wear a given item.
    /// @param item  The item to wear.
    /// @param error The error message.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    bool canWear(Item * item, std::string & error) const;

    /// @brief Sums the given value to the current thirst.
    /// @param value The value to sum.
    /// @return <b>True</b> if the new value is geq 0,<br>
    ///         <b>False</b> otherwise.
    bool setThirst(int value);

    /// @brief Provides the current level of thirst.
    /// @return The current value.
    unsigned int getThirst() const;

    /// @brief Get character level of thirst.
    /// @return Thirst of this character.
    std::string getThirstCondition() const;

    /// @brief Sums the given value to the current hunger.
    /// @param value The value to sum.
    /// @return <b>True</b> if the new value is geq 0,<br>
    ///         <b>False</b> otherwise.
    bool setHunger(int value);

    /// @brief Provides the current level of hunger.
    /// @return The current value.
    unsigned int getHunger() const;

    /// @brief Get character level of hunger.
    /// @return Hunger of this character.
    std::string getHungerCondition() const;

    /// @brief Update the health.
    void updateHealth();

    /// @brief Update the stamina.
    void updateStamina();

    /// @brief Update the hunger.
    void updateHunger();

    /// @brief Update the thirst.
    void updateThirst();

    /// @brief Update the list of expired effects.
    void updateExpiredEffects();

    /// @brief Update the list of activated effects.
    void updateActivatedEffects();

    /// @brief Provide a detailed description of the character.
    /// @return A detailed description of the character.
    std::string getLook();

    /// @brief Check if the current character can see the target character.
    /// @param target The target character.
    /// @return <b>True</b> if the can see the other character,<br>
    ///         <b>False</b> otherwise.
    bool canSee(Character * target) const;

    // Combat functions.
    /// @brief Provides the overall armor class.
    /// @return The armor class.
    unsigned int getArmorClass() const;

    /// @brief Function which checks if the character can attack with a weapon equipped
    ///         in the given slot.
    /// @param slot The slot in which the weapon should be.
    /// @return <b>True</b> if the item is there,<br>
    ///         <b>False</b> otherwise.
    bool canAttackWith(const EquipmentSlot & slot) const;

    /// @brief Checks if the given target is both In Sight and within the Range of Sight.
    /// @param target The target character.
    /// @param range The maximum range.
    /// @return <b>True</b> if the target is in sight,<br>
    ///         <b>False</b> otherwise.
    bool isAtRange(Character * target, const unsigned int & range);

    /// @brief Provides the list of active melee weapons (Left and Right hands).
    /// @return Vector of melee weapons.
    std::vector<MeleeWeaponItem *> getActiveMeleeWeapons();

    /// @brief Provides the list of active ranged weapons (Left and Right hands).
    /// @return Vector of ranged weapons.
    std::vector<RangedWeaponItem *> getActiveRangedWeapons();

    /// @brief Handle what happend when this character die.
    virtual void kill();

    /// @brief Create a corpse on the ground.
    /// @return A pointer to the corpse.
    Item * createCorpse();

    /// @brief Handle character input.
    /// @param command Command that need to be handled.
    void doCommand(const std::string & command);

    /// @brief Returns the character <b>statically</b> casted to player.
    /// @return The player version of the character.
    Player * toPlayer();

    /// @brief Returns the character <b>statically</b> casted to mobile.
    /// @return The mobile version of the character.
    Mobile * toMobile();

    /// @brief Starts a lua environment and loads the given script.
    /// @param scriptFilename The name of the script that has to be loaded.
    void loadScript(const std::string & scriptFilename);

    /// @brief Returns the list of available targets using the vector
    ///         structure made for lua environment.
    /// @return The vector of targets.
    VectorHelper<Character *> luaGetTargets();

    /// @brief Specific function used by lua to add an equipment item.
    void luaAddEquipment(Item * item);

    /// @brief Specific function used by lua to remove an equipment item.
    bool luaRemEquipment(Item * item);

    /// @brief Specific function used by lua to add an inventory item.
    void luaAddInventory(Item * item);

    /// @brief Specific function used by lua to remove an inventory item.
    bool luaRemInventory(Item * item);

    /// @brief Function used to register inside the lua environment the class.
    /// @param L The lua environment.
    static void luaRegister(lua_State * L);

    /// @brief Operator used to order the character based on their name.
    bool operator<(const class Character & source) const;

    /// @brief Operator used to order the character based on their name.
    bool operator==(const class Character & source) const;

    /// @brief Sends a message to the character.
    /// @param msg Message to send.
    virtual void sendMsg(const std::string & msg);

    /// @brief Sends a message to the character.
    /// @param msg   The message to send
    /// @param first The first unpacked argument.
    /// @param args  Packed arguments.
    template<typename ... Args>
    void sendMsg(const std::string & msg, const std::string & first, const Args & ... args)
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

    /// @brief Sends a message to the character. This one in particular handles unsigned integer.
    template<typename ... Args>
    void sendMsg(const std::string & msg, const unsigned int & first, const Args & ... args)
    {
        this->sendMsg(msg, ToString(first), args ...);
    }
};

/// @addtogroup FlagsToList
/// @{

/// Return a list of string containg the Character flags contained inside the value.
std::string GetCharacterFlagString(unsigned int flags);

/// @}
