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

#include "character/mobile.hpp"

#include "character/behaviour/generalBehaviour.hpp"
#include "lua/lua_script.hpp"
#include "utilities/logger.hpp"
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
    managedItem(),
    behaviourQueue(),
    behaviourTimer(std::chrono::system_clock::now()),
    behaviourDelay(500000)
{
    // Nothing to do.
}

Mobile::~Mobile()
{
    // Delete only the temporary items.
    for (auto item : equipment)
    {
        if (HasFlag(item->flags, ItemFlag::Temporary))
        {
            delete (item);
        }
    }
    for (auto item : inventory)
    {
        if (HasFlag(item->flags, ItemFlag::Temporary))
        {
            delete (item);
        }
    }
    if (this->isAlive())
    {
        room->removeCharacter(this);
    }
    Logger::log(LogLevel::Debug,
                "Deleted mobile\t\t\t\t(%s)",
                this->getNameCapital());
}

bool Mobile::setAbilities(const std::string & source)
{
    if (source.empty()) return false;
    std::vector<std::string> charList = SplitString(source, ";");
    if (charList.size() != 5) return false;
    if (!this->setAbility(Ability::Strength,
                          ToNumber<unsigned int>(charList[0])))
    {
        return false;
    }
    if (!this->setAbility(Ability::Agility,
                          ToNumber<unsigned int>(charList[1])))
    {
        return false;
    }
    if (!this->setAbility(Ability::Perception,
                          ToNumber<unsigned int>(charList[2])))
    {
        return false;
    }
    if (!this->setAbility(Ability::Constitution,
                          ToNumber<unsigned int>(charList[3])))
    {
        return false;
    }
    if (!this->setAbility(Ability::Intelligence,
                          ToNumber<unsigned int>(charList[4])))
    {
        return false;
    }
    return true;
}

void Mobile::respawn()
{
    // Delete the models loaded as equipment.
    for (auto item : equipment)
    {
        if (HasFlag(item->flags, ItemFlag::Temporary))
        {
            delete (item);
        }
    }
    equipment.clear();
    // Delete the models loaded in the inventory.
    for (auto item : inventory)
    {
        if (HasFlag(item->flags, ItemFlag::Temporary))
        {
            delete (item);
        }
    }
    inventory.clear();
    // Intialize the lua state.
    L = luaL_newstate();
    // Load the lua environment.
    LoadLuaEnvironmet(L, lua_script);
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
    this->room->sendToAll("%s apear from somewhere.\n",
                          exceptions,
                          this->getNameCapital());
    // Set the next action time.
    behaviourTimer = std::chrono::system_clock::now() +
                     std::chrono::seconds(level);
    // Log to the mud.
    //Logger::log(LogLevel::Debug, "Respawning " + this->id);
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

std::string Mobile::getName() const
{
    return ToLower(staticdesc);
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
        auto min = (item->maxCondition / 100) * 10;
        auto max = (item->maxCondition / 100) * 50;
        // Set a random condition for the new item.
        item->condition = TRandReal<double>(min, max);
    };
    // Before calling the character kill function, set the vnum for the new
    //  items, and set the item condition to a random value from 10% to 50%.
    for (auto item : this->inventory) generateItem(item);
    for (auto item : this->equipment) generateItem(item);
    // Call the method of the father class.
    Character::kill();
    // Set to 0 the cycle that this mobile has passed dead.
    nextRespawn = std::chrono::system_clock::now() +
                  std::chrono::seconds(10 * this->level);
    // Call the LUA function: Event_Death.
    this->triggerEventDeath();
}

int64_t Mobile::getRespawnTime() const
{
    // Return the check if the mobile can be respawned.
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - nextRespawn).count();
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
        if (HasFlag(item->flags, ItemFlag::Temporary))
        {
            delete (item);
        }
    }
    equipment.clear();
    // Delete the models loaded in the inventory.
    for (auto item : inventory)
    {
        if (HasFlag(item->flags, ItemFlag::Temporary))
        {
            delete (item);
        }
    }
    inventory.clear();
    // Completely clear the stack.
    lua_settop(L, 0);
    // Empty the behaviours queue.
    behaviourQueue.clear();
    // Reset the lua state.
    L = luaL_newstate();
    // Load the lua environment.
    LoadLuaEnvironmet(L, lua_script);
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

void Mobile::performBehaviour()
{
    if (behaviourQueue.empty())
    {
        this->triggerEventMain();
    }
    if (this->checkBehaviourTimer())
    {
        auto status = behaviourQueue.front()->perform();
        if ((status == BehaviourStatus::Finished) ||
            (status == BehaviourStatus::Error))
        {
            behaviourQueue.pop_front();
        }
    }
}

bool Mobile::checkBehaviourTimer()
{
    // Check if the tic is passed.
    if (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - behaviourTimer) >= behaviourDelay)
    {
        behaviourTimer = std::chrono::system_clock::now();
        return true;
    }
    return false;
}

void Mobile::triggerEventInit()
{
    this->mobileThread("EventInit", nullptr, "");
}

void Mobile::triggerEventFight(Character * character)
{
    this->mobileThread("EventFight", character, "");
}

void Mobile::triggerEventEnter(Character * character)
{
    Logger::log(LogLevel::Trace, "Activating EventEnter.");
    this->mobileThread("EventEnter", character, "");
    behaviourTimer = std::chrono::system_clock::now() -
                     std::chrono::seconds(1);
}

void Mobile::triggerEventExit(Character * character)
{
    this->mobileThread("EventExit", character, "");
}

void Mobile::triggerEventMessage(Character * character, std::string message)
{
    this->mobileThread("EventMessage", character, message);
}

void Mobile::triggerEventRandom()
{
    this->mobileThread("EventRandom", nullptr, "");
}

void Mobile::triggerEventMorning()
{
    this->mobileThread("EventMorning", nullptr, "");
}

void Mobile::triggerEventDay()
{
    this->mobileThread("EventDay", nullptr, "");
}

void Mobile::triggerEventDusk()
{
    this->mobileThread("EventDusk", nullptr, "");
}

void Mobile::triggerEventNight()
{
    this->mobileThread("EventNight", nullptr, "");
}

void Mobile::triggerEventDeath()
{
    this->mobileThread("EventDeath", nullptr, "");
}

void Mobile::triggerEventMain()
{
    this->mobileThread("EventMain", nullptr, "");
}

bool Mobile::mobileThread(std::string event,
                          Character * character,
                          std::string message)
{
    if (!event.empty())
    {
        try
        {
            luabridge::LuaRef func = luabridge::getGlobal(L, event.c_str());
            if (func.isFunction())
            {
                if (character != nullptr)
                {
                    if (!message.empty())
                    {
                        behaviourQueue.emplace_back(
                            std::make_shared<BehaviourP3<
                                Character *,
                                Character *,
                                std::string>>(event,
                                              func,
                                              this,
                                              character,
                                              message));
                    }
                    else
                    {
                        behaviourQueue.emplace_back(
                            std::make_shared<
                                BehaviourP2<
                                    Character *,
                                    Character *>>(event,
                                                  func,
                                                  this,
                                                  character));
                    }
                }
                else
                {
                    behaviourQueue.emplace_back(
                        std::make_shared<
                            BehaviourP1<Character *>>(event,
                                                      func,
                                                      this));
                }
            }
        }
        catch (luabridge::LuaException const & e)
        {
            Logger::log(LogLevel::Error, e.what());
        }
    }
    return true;
}

void Mobile::updateTicImpl()
{
    // Check if the mobile is alive.
    if (this->isAlive())
    {
        Character::updateTicImpl();
    }
    else
    {
        // Check if the mobile can be respawned.
        if (this->canRespawn())
        {
            // Respawn the mobile.
            this->respawn();
        }
    }
}

void Mobile::updateHourImpl()
{
    if (this->isAlive())
    {
        auto mudHour = MudUpdater::instance().getMudHour();
        if (mudHour == 6)
        {
            this->triggerEventMorning();
        }
        else if (mudHour == 12)
        {
            this->triggerEventDay();
        }
        else if (mudHour == 18)
        {
            this->triggerEventDusk();
        }
        else if (mudHour == 24)
        {
            this->triggerEventNight();
        }
        else
        {
            this->triggerEventRandom();
        }
    }
}
