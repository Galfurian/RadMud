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
#include "constants.hpp"
#include "lua/lua_script.hpp"
#include "utilities/logger.hpp"
#include "luabridge/LuaBridge.h"

using namespace std;
using namespace std::chrono;

Mobile::Mobile() :
        id(),
        respawnRoom(),
        keys(),
        shortdesc(),
        staticdesc(),
        actions(),
        message_buffer(),
        nextRespawn(),
        controller(),
        lua_script(),
        lua_mutex(),
        nextActionCooldown(std::chrono::system_clock::now()),
        managedItem()
{
    // Nothing to do.
}

Mobile::~Mobile()
{
    for (auto item : equipment)
    {
        delete (item);
    }
    for (auto item : inventory)
    {
        delete (item);
    }
    if (this->isAlive())
    {
        room->removeCharacter(this);
    }
    Logger::log(LogLevel::Debug, "Deleted mobile\t\t\t\t(%s)", this->getNameCapital());
}

bool Mobile::setAbilities(const std::string & source)
{
    if (source.empty())
    {
        return false;
    }
    std::vector<std::string> charList = SplitString(source, ";");
    if (charList.size() != 5)
    {
        return false;
    }
    bool result = true;
    result &= this->setAbility(Ability::Strength, ToNumber<unsigned int>(charList[0]));
    result &= this->setAbility(Ability::Agility, ToNumber<unsigned int>(charList[1]));
    result &= this->setAbility(Ability::Perception, ToNumber<unsigned int>(charList[2]));
    result &= this->setAbility(Ability::Constitution, ToNumber<unsigned int>(charList[3]));
    result &= this->setAbility(Ability::Intelligence, ToNumber<unsigned int>(charList[4]));
    return true;
}

void Mobile::respawn()
{
    // Set the mobile to Alive.
    this->setHealth(this->getMaxHealth(), true);
    this->setStamina(this->getMaxStamina(), true);
    // Trigger the init lua function.
    this->triggerEventInit();
    // Add the mobile to the respawn room.
    this->respawnRoom->addCharacter(this);
    // Set the list of exceptions.
    CharacterVector exceptions;
    exceptions.push_back(this);
    // Send the message inside the room.
    this->room->sendToAll("%s apear from somewhere.\n", exceptions, this->getNameCapital());
    // Set the next action time.
    nextActionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(10 * level);
    // Log to the mud.
    Logger::log(LogLevel::Debug, "Respawning " + this->id);
}

bool Mobile::isAlive() const
{
    return (this->room != nullptr);
}

bool Mobile::check() const
{
    bool safe = Character::check();
    safe &= CorrectAssert(!id.empty());
    safe &= CorrectAssert(respawnRoom != nullptr);
    safe &= CorrectAssert(!keys.empty());
    safe &= CorrectAssert(!shortdesc.empty());
    safe &= CorrectAssert(!staticdesc.empty());
    safe &= CorrectAssert(!actions.empty());
    safe &= CorrectAssert(!lua_script.empty());
    return safe;
}

bool Mobile::isMobile() const
{
    return true;
}

void Mobile::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Character::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Id", this->id });
    sheet.addRow( { "Respawn Room", ToString(this->respawnRoom->vnum) });
    std::string keyGroup;
    for (auto it : this->keys)
    {
        keyGroup += " " + it;
    }
    sheet.addRow( { "Keys", keyGroup });
    sheet.addRow( { "Short Desc.", this->shortdesc });
    sheet.addRow( { "Static Desc.", this->staticdesc });
    std::string actionGroup;
    for (auto it : this->actions)
    {
        actionGroup += " " + it;
    }
    sheet.addRow( { "Actions", actionGroup });
    sheet.addRow( { "Is Alive", ToString(this->isAlive()) });
    if (!this->isAlive())
    {
        sheet.addRow( { "Respawn Time", ToString(this->getRespawnTime()) });

    }
    if (this->controller != nullptr)
    {
        sheet.addRow( { "Controller", this->controller->getName() });
    }
    sheet.addRow( { "Lua Script", this->lua_script });
}

bool Mobile::hasKey(const string & key) const
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

bool Mobile::hasAction(const string & _action) const
{
    bool found = false;
    for (auto iterator : actions)
    {
        if (BeginWith(iterator, _action))
        {
            found = true;
        }
    }
    return found;
}

void Mobile::kill()
{
    // Before calling the character kill function, set the vnum for the new
    //  items, and set the item condition to a random value from 10% to 50%.
    for (auto it = this->inventory.begin(); it != this->inventory.end(); ++it)
    {
        Item * item = (*it);
        // Set the item vnum.
        item->vnum = Mud::instance().getMaxVnumItem() + 1;
        // Add the item to the mud.
        Mud::instance().addItem(item);
        // Evaluate the minimum and maximum condition.
        unsigned int min = (item->getMaxCondition() / 100) * 10;
        unsigned int max = (item->getMaxCondition() / 100) * 50;
        // Set a random condition for the new item.
        item->condition = TRandInteger<unsigned int>(min, max);
    }
    for (auto it = this->equipment.begin(); it != this->equipment.end(); ++it)
    {
        Item * item = (*it);
        // Set the item vnum.
        item->vnum = Mud::instance().getMaxVnumItem() + 1;
        // Add the item to the mud.
        Mud::instance().addItem(item);
        // Evaluate the minimum and maximum condition.
        unsigned int min = (item->getMaxCondition() / 100) * 10;
        unsigned int max = (item->getMaxCondition() / 100) * 50;
        // Set a random condition for the new item.
        item->condition = TRandInteger<unsigned int>(min, max);
        // Create the entry for the item on the database.
    }
    // Call the method of the father class.
    Character::kill();
    // Set to 0 the cycle that this mobile has passed dead.
    nextRespawn = system_clock::now() + seconds(10 * this->level);
    // Call the LUA function: Event_Death.
    this->triggerEventDeath();
}

int64_t Mobile::getRespawnTime() const
{
    // Return the check if the mobile can be respawned.
    return duration_cast<seconds>(system_clock::now() - nextRespawn).count();
}

bool Mobile::canRespawn()
{
    // Return the check if the mobile can be respawned.
    return (this->getRespawnTime() >= 0);
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

void Mobile::sendMsg(const std::string & msg)
{
    if (controller != NULL)
    {
        controller->sendMsg(msg);
    }
    else
    {
        message_buffer += msg;
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
        Logger::log(LogLevel::Error, e.what());
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
