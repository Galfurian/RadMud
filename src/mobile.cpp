/// @file   mobile.cpp
/// @brief  Implements mobile methods.
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

// Basic Include.
#include "mobile.hpp"

// Local Includes.

// Other Include.
#include "mud.hpp"
#include "room.hpp"
#include "logger.hpp"
#include "constants.hpp"
#include "lua/lua_script.hpp"
#include "luabridge/LuaBridge.h"

using namespace std;

Mobile::Mobile() :
        id(),
        respawnRoom(),
        keys(),
        shortdesc(),
        staticdesc(),
        actions(),
        message_buffer(),
        alive(),
        nextRespawn(),
        controller(),
        lua_script(),
        lua_mutex(),
        nextActionCooldown()
{
    // Nothing to do.
}

Mobile::~Mobile()
{
    Logger::log(LogLevel::Debug, "Deleted: Mobile (" + this->getNameCapital() + ").");
    for (auto item : equipment)
    {
        delete (item);
    }
    for (auto item : inventory)
    {
        delete (item);
    }
    if ((room != nullptr) && this->alive)
    {
        room->removeCharacter(this);
    }
}

void Mobile::respawn()
{
    // Set the mobile to Alive.
    this->alive = true;
    this->health = this->getMaxHealth();
    this->stamina = this->getMaxStamina();
    // Trigger the init lua function.
    this->triggerEventInit();
    // Add the mobile to the respawn room.
    this->respawnRoom->addCharacter(this);
    // Log the room.
    this->room->sendToAll(this->getNameCapital() + " apear from somewhere.\n");
    // Log to the mud.
    Logger::log(LogLevel::Debug, "Respawning " + this->id);
}

bool Mobile::check()
{
    bool safe = Character::check();
    safe &= SafeAssert(!id.empty());
    safe &= SafeAssert(respawnRoom != nullptr);
    safe &= SafeAssert(!keys.empty());
    safe &= SafeAssert(!shortdesc.empty());
    safe &= SafeAssert(!staticdesc.empty());
    safe &= SafeAssert(!actions.empty());
    safe &= SafeAssert(!lua_script.empty());
    safe &= SafeAssert(lua_mutex.try_lock());
    return safe;
}

bool Mobile::isMobile()
{
    return true;
}

bool Mobile::hasKey(const string & key)
{
    bool found = false;
    for (auto iterator : keys)
    {
        if (BeginWith(iterator, key))
        {
            found = true;
        }
    }
    return found;
}

bool Mobile::hasAction(const string & action) const
{
    bool found = false;
    for (auto iterator : actions)
    {
        if (BeginWith(iterator, action))
        {
            found = true;
        }
    }
    return found;
}

void Mobile::triggerDeath()
{
    // Call the method of the father class.
    Character::triggerDeath();
    // Set the mobile as dead.
    alive = false;
    // Set to 0 the cycle that this mobile has passed dead.
    nextRespawn = std::chrono::system_clock::now() + std::chrono::seconds(10 * this->level);
    // Call the LUA function: Event_Death.
    this->triggerEventDeath();
}

bool Mobile::canRespawn()
{
    // Return the check if the mobile can be respawned.
    return (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - nextRespawn).count()
        >= 0);
}

void Mobile::reloadLua()
{
    // Delete the models loaded as equipment.
    for (auto item : equipment)
    {
        delete (item);
    }
    equipment.clear();

    // Delete the models loaded in the inventory.
    for (auto item : inventory)
    {
        delete (item);
    }
    inventory.clear();

    L = luaL_newstate();

    this->loadScript(kSystemDir + "lua/" + lua_script);

    // Call the LUA function: Event_Init in order to prepare the mobile.
    this->triggerEventInit();
}

void Mobile::sendMsg(const std::string & message)
{
    if (controller != NULL)
    {
        controller->sendMsg(message);
    }
    else
    {
        message_buffer += message;
    }
}

bool Mobile::mobileThread(std::string event, Character * character, std::string message)
{
    (void) character;
    (void) message;

    // Lock to the mutex.
    lua_mutex.lock();

    try
    {
        luabridge::LuaRef f = luabridge::getGlobal(L, event.c_str());
        try
        {
            if (character != nullptr)
            {
                if (!message.empty())
                {
                    f(this, character, message);
                }
                else
                {
                    f(this, character);
                }
            }
            else
            {
                f(this);
            }
        }
        catch (luabridge::LuaException const& e)
        {
            Logger::log(LogLevel::Error, e.what());
        }
    }
    catch (luabridge::LuaException const & e)
    {
        std::cerr && e.what();
    }

    // Unlock the mutex.
    lua_mutex.unlock();
    return true;
}

void Mobile::triggerEventInit()
{
    std::thread t(&Mobile::mobileThread, this, "EventInit", nullptr, "");
    t.detach();
}

void Mobile::triggerEventFight(Character * character)
{
    std::thread t(&Mobile::mobileThread, this, "EventFight", character, "");
    t.detach();
}

void Mobile::triggerEventEnter(Character * character)
{
    std::thread t(&Mobile::mobileThread, this, "EventEnter", character, "");
    t.detach();
}

void Mobile::triggerEventExit(Character * character)
{
    std::thread t(&Mobile::mobileThread, this, "EventExit", character, "");
    t.detach();
}

void Mobile::triggerEventMessage(Character * character, string message)
{
    std::thread t(&Mobile::mobileThread, this, "EventMessage", character, message);
    t.detach();
}

void Mobile::triggerEventRandom()
{
    std::thread t(&Mobile::mobileThread, this, "EventRandom", nullptr, "");
    t.detach();
}

void Mobile::triggerEventMorning()
{
    std::thread t(&Mobile::mobileThread, this, "EventMorning", nullptr, "");
    t.detach();
}

void Mobile::triggerEventDay()
{
    std::thread t(&Mobile::mobileThread, this, "EventDay", nullptr, "");
    t.detach();
}

void Mobile::triggerEventDusk()
{
    std::thread t(&Mobile::mobileThread, this, "EventDusk", nullptr, "");
    t.detach();
}

void Mobile::triggerEventNight()
{
    std::thread t(&Mobile::mobileThread, this, "EventNight", nullptr, "");
    t.detach();
}

void Mobile::triggerEventDeath()
{
    std::thread t(&Mobile::mobileThread, this, "EventDeath", nullptr, "");
    t.detach();
}

void Mobile::triggerEventMain()
{
    std::thread t(&Mobile::mobileThread, this, "EventMain", nullptr, "");
    t.detach();
}
