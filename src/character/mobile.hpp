/// @file   mobile.hpp
/// @brief  Define mobile variables and methods.
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

#pragma once

#include <list>
#include <set>
#include <string>
#include <thread>
#include <mutex>
#include <future>

#include "character.hpp"

/// @brief Holds details about mobile: mob, vendor, quest npcs.
class Mobile :
    public Character
{
public:
    /// An identifier of the mobile.
    std::string id;
    /// The room where the mobile must respawn.
    Room * respawnRoom;
    /// List of keys used to target this mobile.
    std::vector<std::string> keys;
    /// Short description.
    std::string shortdesc;
    /// Static description.
    std::string staticdesc;
    /// Default actions that the mobile execute.
    std::vector<std::string> actions;
    /// Mobile buffer of received message.
    std::string message_buffer;
    /// How many seconds before respawn.
    std::chrono::time_point<std::chrono::system_clock> nextRespawn;
    /// The character that is controlling this one.
    Character * controller;
    /// The file that contains the behaviour of this mobile.
    std::string lua_script;
    /// Seconds until next action.
    std::chrono::time_point<std::chrono::system_clock> nextActionCooldown;
    /// The item of which this mobile is the manager.
    Item * managedItem;

    /// @brief Constructor.
    Mobile();

    /// @brief Destructor.
    ~Mobile();

    bool check() const override;

    bool isMobile() const override;

    void getSheet(Table & sheet) const override;

    std::string getName() const override;

    /// @brief Allows to pass a string which contains the abilities.
    /// @param source The string which containts the values
    /// @return <b>True</b> if the string is correct,<br>
    ///         <b>False</b> otherwise.
    bool setAbilities(const std::string & source);

    /// @brief Initialize mobile.
    void respawn(bool actNow = false);

    /// @brief Checks if the mobile is alive.
    /// @return <b>True</b> if is alive,<br>
    ///         <b>False</b> otherwise.
    bool isAlive() const;

    /// @brief Check if the mobile has the desired key.
    /// @param key The key to find.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool hasKey(const std::string & key) const;

    /// @brief Check if the mobile has the desired action set.
    /// @param _action The action to check.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool hasAction(const std::string & _action) const;

    /// @brief Handle what happend when this mob die.
    void kill() override;

    /// @brief Return the number of second until next respawn.
    /// @return Seconds until respawn.
    int64_t getRespawnTime() const;

    /// @brief Check if the mobile can be respawned.
    /// @return <b>True</b> if it can be respawned,<br>
    ///         <b>False</b> Otherwise.
    bool canRespawn();

    /// @brief Reload the LUA enviroment of the mobile.
    void reloadLua();

    /// @brief Output to player (any type).
    /// @param msg The string to sent.
    void sendMsg(const std::string & msg) override;

    /// @defgroup MobileLuaEvent Mobile Lua Events Function
    /// @brief All the functions necessary to call the correspondent Function on Lua file,
    /// in order to react to a particular event.
    /// @{

    /// @brief This event is triggered when the mobile it's created,
    ///         usually it's used to load the mobile equipment.
    void triggerEventInit();

    /// @brief This event is triggered at the beginning of every fight turn.
    void triggerEventFight(Character * character);

    /// @brief This event is triggered when the passed character enters the room.
    void triggerEventEnter(Character * character);

    /// @brief This event is triggered when the passed character exits the room.
    void triggerEventExit(Character * character);

    /// @brief This event is triggered when the passed character
    ///         send a message to this mobile.
    void triggerEventMessage(Character * character, std::string message);

    /// @brief This event is triggered at random every 10 seconds.
    void triggerEventRandom();

    /// @brief This event is triggered at the beginning of Morning.
    void triggerEventMorning();

    /// @brief This event is triggered at the beginning of the Day.
    void triggerEventDay();

    /// @brief This event is triggered at the beginning of Dusk.
    void triggerEventDusk();

    /// @brief This event is triggered at the beginning of Night.
    void triggerEventNight();

    /// @brief This event is triggered when the mobile dies.
    void triggerEventDeath();

    /// @brief This event is triggered at every time tick.
    void triggerEventMain();
    ///@}

protected:

    /// @brief A thread used to handle mobile actions.
    /// @param event     The event name.
    /// @param character The target character.
    /// @param message   The received message.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool mobileThread(std::string event, Character * character,
                      std::string message);

    void updateTicImpl() override;

    void updateHourImpl() override;

private:
    /// @brief Disable copy constructor.
    Mobile(const Mobile & source);

    /// @brief Disable assign operator.
    Mobile & operator=(const Mobile &);
};
