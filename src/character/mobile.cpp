/// @file   mobile.cpp
/// @brief  Implements mobile methods.
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

#include "mobile.hpp"

#include "logger.hpp"
#include "mud.hpp"

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
    //Logger::log(LogLevel::Debug, "Deleted mobile\t\t\t\t(%s)", this->getNameCapital());
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
    std::vector<Character *> exceptions;
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
    sheet.addRow({"Id", this->id});
    sheet.addRow({"Respawn Room", ToString(this->respawnRoom->vnum)});
    std::string keyGroup;
    for (auto it : this->keys)
    {
        keyGroup += " " + it;
    }
    sheet.addRow({"Keys", keyGroup});
    sheet.addRow({"Short Desc.", this->shortdesc});
    sheet.addRow({"Static Desc.", this->staticdesc});
    std::string actionGroup;
    for (auto it : this->actions)
    {
        actionGroup += " " + it;
    }
    sheet.addRow({"Actions", actionGroup});
    sheet.addRow({"Is Alive", ToString(this->isAlive())});
    if (!this->isAlive())
    {
        sheet.addRow({"Respawn Time", ToString(this->getRespawnTime())});

    }
    if (this->controller != nullptr)
    {
        sheet.addRow({"Controller", this->controller->getName()});
    }
    sheet.addRow({"Lua Script", this->lua_script});
}

bool Mobile::hasKey(const std::string & key) const
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

bool Mobile::hasAction(const std::string & _action) const
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
    auto generateItem = [&](Item * item)
    {
        // Set the item vnum.
        item->vnum = Mud::instance().getMaxVnumItem() + 1;
        // Add the item to the mud.
        Mud::instance().addItem(item);
        // Evaluate the minimum and maximum condition.
        unsigned int min = (item->getMaxCondition() / 100) * 10;
        unsigned int max = (item->getMaxCondition() / 100) * 50;
        // Set a random condition for the new item.
        item->condition = TRandInteger<unsigned int>(min, max);
    };
    // Before calling the character kill function, set the vnum for the new
    //  items, and set the item condition to a random value from 10% to 50%.
    for (auto item : this->inventory) generateItem(item);
    for (auto item : this->equipment) generateItem(item);
    // Call the method of the father class.
    Character::kill();
    // Set to 0 the cycle that this mobile has passed dead.
    nextRespawn = std::chrono::system_clock::now() + std::chrono::seconds(10 * this->level);
    // Call the LUA function: Event_Death.
    this->triggerEventDeath();
}

int64_t Mobile::getRespawnTime() const
{
    // Return the check if the mobile can be respawned.
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - nextRespawn).count();
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

    this->loadScript(Mud::instance().getMudSystemDirectory() + "lua/" + lua_script);

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
    // Lock to the mutex.
    lua_mutex.lock();
    Logger::log(LogLevel::Trace, "Starting thread for event '%s'", event);
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
        catch (luabridge::LuaException const & e)
        {
            Logger::log(LogLevel::Error, e.what());
        }
    }
    catch (luabridge::LuaException const & e)
    {
        Logger::log(LogLevel::Error, e.what());
    }
    Logger::log(LogLevel::Trace, "Ending   thread for event '%s'", event);
    // Unlock the mutex.
    lua_mutex.unlock();
    return true;
}

void Mobile::triggerEventInit()
{
    t = std::thread(&Mobile::mobileThread, this, "EventInit", nullptr, "");
    t.detach();
}

void Mobile::triggerEventFight(Character * character)
{
    t = std::thread(&Mobile::mobileThread, this, "EventFight", character, "");
    t.detach();
}

void Mobile::triggerEventEnter(Character * character)
{
    t = std::thread(&Mobile::mobileThread, this, "EventEnter", character, "");
    t.detach();
}

void Mobile::triggerEventExit(Character * character)
{
    t = std::thread(&Mobile::mobileThread, this, "EventExit", character, "");
    t.detach();
}

void Mobile::triggerEventMessage(Character * character, std::string message)
{
    t = std::thread(&Mobile::mobileThread, this, "EventMessage", character, message);
    t.detach();
}

void Mobile::triggerEventRandom()
{
    t = std::thread(&Mobile::mobileThread, this, "EventRandom", nullptr, "");
    t.detach();
}

void Mobile::triggerEventMorning()
{
    t = std::thread(&Mobile::mobileThread, this, "EventMorning", nullptr, "");
    t.detach();
}

void Mobile::triggerEventDay()
{
    t = std::thread(&Mobile::mobileThread, this, "EventDay", nullptr, "");
    t.detach();
}

void Mobile::triggerEventDusk()
{
    t = std::thread(&Mobile::mobileThread, this, "EventDusk", nullptr, "");
    t.detach();
}

void Mobile::triggerEventNight()
{
    t = std::thread(&Mobile::mobileThread, this, "EventNight", nullptr, "");
    t.detach();
}

void Mobile::triggerEventDeath()
{
    t = std::thread(&Mobile::mobileThread, this, "EventDeath", nullptr, "");
    t.detach();
}

void Mobile::triggerEventMain()
{
    t = std::thread(&Mobile::mobileThread, this, "EventMain", nullptr, "");
    t.detach();
}
