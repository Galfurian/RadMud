/// @file   mobile.hpp
/// @brief  Define mobile variables and methods.
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

#ifndef MOBILE_HPP
#define MOBILE_HPP

#include <list>
#include <set>
#include <string>
#include <thread>
#include <mutex>
#include <future>

#include "character.hpp"

/// @brief Holds details about mobile: mob, vendor, quest npcs.
class Mobile: public Character
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
        /// Mobile most important variable, deterine if the mobile it's alive or not.
        bool alive;
        /// How many seconds before respawn.
        TimeClock nextRespawn;
        /// The character that is controlling this one.
        Character * controller;
        /// The file that contains the behaviour of this mobile.
        std::string lua_script;
        /// The mutex for this mobile.
        std::mutex lua_mutex;
        /// Seconds until next action.
        TimeClock nextActionCooldown;

        /// @brief Constructor.
        Mobile();

        /// @brief Destructor.
        ~Mobile();

        virtual bool check();

        virtual bool isMobile();

        /// @brief Allows to pass a string which contains the abilities.
        /// @param source The string which containts the values
        /// @return <b>True</b> if the string is correct,<br>
        ///         <b>False</b> otherwise.
        bool setAbilities(const std::string & source);

        /// @brief Initialize mobile.
        void respawn();

        /// @brief Check if the mobile has the desired key.
        /// @param key The key to find.
        /// @return <b>True</b> if the operations succeeded,<br><b>False</b> Otherwise.
        bool hasKey(const std::string & key);

        /// @brief Check if the mobile has the desired action set.
        /// @param _action The action to check.
        /// @return <b>True</b> if the operations succeeded,<br>
        ///         <b>False</b> Otherwise.
        bool hasAction(const std::string & _action) const;

        /// @brief Handle what happend when this mob die.
        void triggerDeath();

        /// @brief Return the number of second until next respawn.
        /// @return Seconds until respawn.
        int64_t getRespawnTime();

        bool canRespawn();

        /// @brief Reload the LUA enviroment of the mobile.
        void reloadLua();

        /// @brief Output to player (any type).
        /// @param msg The string to sent.
        void sendMsg(const std::string & msg);

        /// @brief A thread used to handle mobile actions.
        /// @param event     The event name.
        /// @param character The target character.
        /// @param message   The received message.
        /// @return <b>True</b> if the operations succeeded,<br><b>False</b> Otherwise.
        bool mobileThread(std::string event, Character * character, std::string message);

        /// @defgroup MobileLuaEvent Mobile Lua Events Function
        /// @brief All the functions necessary to call the correspondent Function on Lua file,
        /// in order to react to a particular event.
        /// @{

        /// This event is triggered when the mobile it's created, usually it's used to load the mobile equipment.
        void triggerEventInit();
        /// This event is triggered at the beginning of every fight turn.
        void triggerEventFight(Character * character);
        /// This event is triggered when the passed character enters the room.
        void triggerEventEnter(Character * character);
        /// This event is triggered when the passed character exits the room.
        void triggerEventExit(Character * character);
        /// This event is triggered when the passed character send a message to this mobile.
        void triggerEventMessage(Character * character, std::string message);
        /// This event is triggered at random every 10 seconds.
        void triggerEventRandom();
        /// This event is triggered at the beginning of Morning.
        void triggerEventMorning();
        /// This event is triggered at the beginning of the Day.
        void triggerEventDay();
        /// This event is triggered at the beginning of Dusk.
        void triggerEventDusk();
        /// This event is triggered at the beginning of Night.
        void triggerEventNight();
        /// This event is triggered when the mobile dies.
        void triggerEventDeath();
        /// This event is triggered at every time tick.
        void triggerEventMain();
        ///@}

    private:
        /// @brief Disable copy constructor.
        Mobile(const Mobile& source);

        /// @brief Disable assign operator.
        Mobile& operator=(const Mobile&);
};

/// Mobile list handler.
typedef std::vector<Mobile *> MobileList;

/// MobileList iterator.
typedef MobileList::iterator MobileListIterator;

/// Mobile map handler.
typedef std::map<std::string, Mobile *> MobileMap;

/// MobileMap iterator.
typedef MobileMap::iterator MobileMapIterator;

#endif
