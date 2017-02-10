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

#include "ability.hpp"
#include "exit.hpp"
#include "race.hpp"
#include "item.hpp"
#include "faction.hpp"
#include "bodyPart.hpp"
#include "luaBridge.hpp"
#include "effectManager.hpp"
#include "processInput.hpp"
#include "combatAction.hpp"
#include "argumentHandler.hpp"
#include "meleeWeaponItem.hpp"
#include "rangedWeaponItem.hpp"
#include "characterPosture.hpp"
#include "characterContainer.hpp"

#include <deque>
#include <mutex>

class Room;

class Player;

class Mobile;

/// The list of possible actions.
using GenderType = enum class GenderType_t
{
    None,   ///< The character has no gender (robot).
    Female, ///< The character is a female.
    Male    ///< The character is a male.
};

/// Used to determine the flag of the character.
using CharacterFlag = enum class CharacterFlag_t
{
    None = 0,       ///< No flag.
    IsGod = 1,      ///< The character is a GOD.
    Invisible = 2   ///< The character is invisible.
};

/// @brief Character class, father of Player and Mobile.
/// @details
/// It's the main class that contains all the information that both
/// players and NPCs shares. In order to allow dynamic casting(polymorphism),
/// i've created a method called isMobile, used to identify the subtype of the
/// subclass.
class Character :
    public UpdateInterface
{
public:
    /// Character name.
    std::string name;
    /// Character description.
    std::string description;
    /// Character gender.
    GenderType gender;
    /// Character weight.
    double weight;
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
    int hunger;
    /// Character level of thirst.
    int thirst;
    /// Character abilities.
    std::map<Ability, unsigned int> abilities;
    /// The player's list of skills.
    std::map<int, unsigned int> skills;
    /// The current room the character is in.
    Room * room;
    /// Character's inventory.
    ItemVector inventory;
    /// Character's equipment.
    ItemVector equipment;
    /// Character's posture.
    CharacterPosture posture;
    /// Active effects on player.
    EffectManager effects;
    /// The lua_State associated with this character.
    lua_State * L;
    /// List of opponents.
    CombatHandler combatHandler;
    /// Character current action.
    std::deque<std::shared_ptr<GeneralAction>> actionQueue;
    /// Mutex for the action queue.
    mutable std::mutex actionQueueMutex;
    /// The input handler.
    std::shared_ptr<ProcessInput> inputProcessor;

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

    /// @brief Fills the provided table with the information concerning the
    ///         character.
    /// @param sheet The table that has to be filled.
    virtual void getSheet(Table & sheet) const;

    /// @brief Initializes the variables of the chracter.
    virtual void initialize();

    /// @brief Return the name of the character with all lowercase characters.
    /// @return The name of the character.
    virtual std::string getName() const = 0;

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
    unsigned int getAbility(const Ability & ability,
                            bool withEffects = true) const;

    /// @brief Provides the modifier of the given ability.
    /// @param ability The ability of which the modifier has to be
    ///                 retrieved.
    /// @param withEffects If set to false, this function just return the
    ///                     ability modifier without the contribution due
    ///                     to the active effects.
    /// @return The overall ability modifer.
    unsigned int getAbilityModifier(const Ability & ability,
                                    bool withEffects = true) const;

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
        const double & base = 0.0,
        const double & multiplier = 1.0,
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
    int getViewDistance() const;

    /// @brief Allows to set an action.
    /// @param _action The action that has to be set.
    void pushAction(const std::shared_ptr<GeneralAction> & _action);

    /// @brief Provides a pointer to the action at the front position and
    ///         then remove it from the queue.
    void popAction();

    /// @brief Provides a pointer to the current action.
    std::shared_ptr<GeneralAction> & getAction();

    /// @brief Provides a pointer to the current action.
    std::shared_ptr<GeneralAction> const & getAction() const;

    /// @brief Allows to reset the entire action queue.
    void resetActionQueue();

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

    /// @brief Search an item nearby, (eq, inv, room).
    /// @param itemName The name of the item.
    /// @param number   Position of the item we want to look for.
    /// @return The item, if it's found.
    Item * findNearbyItem(const std::string & itemName, int & number);

    /// @brief Search the item at given position and return it.
    /// @param bodyPart The body part where the method need to search the item.
    /// @return The item, if it's in the character's equipment.
    Item * findItemAtBodyPart(const std::shared_ptr<BodyPart> & bodyPart) const;

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
    /// @param quantity The amount of item to check (by default it is 1).
    /// @return <b>True</b> if the character can lift the item,<br>
    ///         <b>False</b> otherwise.
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
    /// @return Where the item can be wielded.
    std::vector<std::shared_ptr<BodyPart>> canWield(Item * item,
                                                    std::string & error) const;

    /// @brief Check if the character can wear a given item.
    /// @param item  The item to wear.
    /// @param error The error message.
    /// @return Where the item can be worn.
    std::vector<std::shared_ptr<BodyPart>> canWear(Item * item,
                                                   std::string & error) const;

    /// @brief Checks if inside the inventory there is a light source.
    /// @return <b>True</b> if there is a light source,<br>
    ///         <b>False</b> otherwise.
    bool inventoryIsLit() const;

    /// @brief Sums the given value to the current thirst.
    /// @param value The value to sum.
    void addThirst(const int & value);

    /// @brief Get character level of thirst.
    /// @return Thirst of this character.
    std::string getThirstCondition() const;

    /// @brief Sums the given value to the current hunger.
    /// @param value The value to sum.
    void addHunger(const int & value);

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

    /// @brief Function which checks if the character can attack
    ///         with a weapon equipped at the given body part.
    /// @param bodyPart The body part at which the weapon could be.
    /// @return <b>True</b> if the item is there,<br>
    ///         <b>False</b> otherwise.
    bool canAttackWith(const std::shared_ptr<BodyPart> & bodyPart) const;

    /// @brief Checks if the given target is both In Sight and within the Range of Sight.
    /// @param target The target character.
    /// @param range The maximum range.
    /// @return <b>True</b> if the target is in sight,<br>
    ///         <b>False</b> otherwise.
    bool isAtRange(Character * target, const int & range);

    /// @brief Handle what happend when this character die.
    virtual void kill();

    /// @brief Create a corpse on the ground.
    /// @return A pointer to the corpse.
    Item * createCorpse();

    /// @brief Handle character input.
    /// @param command Command that need to be handled.
    /// @return <b>True</b> if the command has been correctly executed,<br>
    ///         <b>False</b> otherwise.
    bool doCommand(const std::string & command);

    /// @brief Returns the character <b>statically</b> casted to player.
    /// @return The player version of the character.
    Player * toPlayer();

    /// @brief Returns the character <b>statically</b> casted to mobile.
    /// @return The mobile version of the character.
    Mobile * toMobile();

    /// @brief Starts a lua environment and loads the given script.
    /// @param scriptFilename The name of the script that has to be loaded.
    void loadScript(const std::string & scriptFilename);

    /// @brief Returns the list of equipped items.
    luabridge::LuaRef luaGetEquipmentItems();

    /// @brief Returns the list of items inside the inventory.
    luabridge::LuaRef luaGetInventoryItems();

    /// @brief Returns the list of rooms in sight.
    luabridge::LuaRef luaGetRoomsInSight();

    /// @brief Returns the list of characters in sight.
    luabridge::LuaRef luaGetCharactersInSight();

    /// @brief Returns the list of items in sight.
    luabridge::LuaRef luaGetItemsInSight();

    /// @brief Returns the list of items in sight.
    luabridge::LuaRef luaGetPathTo(Room * destination);

    /// @brief Allow from lua to load an item.
    /// @param vnumModel    The vnum of the model.
    /// @param vnumMaterial The vnum of the material.
    /// @param qualityValue The initial quality of the item.
    /// @return The newly created item.
    Item * luaLoadItem(int vnumModel,
                       int vnumMaterial,
                       unsigned int qualityValue);

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
    void sendMsg(const std::string & msg,
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

    /// @brief Sends a message to the character.
    /// This one in particular handles unsigned integer.
    template<typename ... Args>
    void sendMsg(const std::string & msg,
                 const unsigned int & first,
                 const Args & ... args)
    {
        this->sendMsg(msg, ToString(first), args ...);
    }

    /// @brief Sends a message to the character. This one in particular handles unsigned integer.
    template<typename ... Args>
    void sendMsg(const std::string & msg,
                 const int & first,
                 const Args & ... args)
    {
        this->sendMsg(msg, ToString(first), args ...);
    }

    /// @brief Sends a message to the character. This one in particular handles unsigned integer.
    template<typename ... Args>
    void sendMsg(const std::string & msg,
                 const size_t & first,
                 const Args & ... args)
    {
        this->sendMsg(msg, ToString(first), args ...);
    }

protected:
    void updateTicImpl() override;

    void updateHourImpl() override;
};

/// @addtogroup FlagsToList
/// @{

/// Return the string describing the type of Gender.
std::string GetGenderTypeName(GenderType type);

/// Return the string describing the given character flag.
std::string GetCharacterFlagName(CharacterFlag flag);

/// Return a list of string containg the Character flags contained inside the value.
std::string GetCharacterFlagString(unsigned int flags);

/// @}
