/// @file   updater.cpp
/// @brief  Implements classes and method used for update everything in the mud.
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

#include "updater.hpp"
#include "logger.hpp"
#include "mud.hpp"

// //////////////////////////////////////////////////////////
// Timings Values
//    50      MS -> 1 SECOND in real world.
//    3000    MS -> 1 MINUTE in real world.
//    180000  MS -> 1 HOUR in real world.
//    4320000 MS -> 1 DAY in real world.

MudUpdater::MudUpdater() :
    bandwidth_in(),
    bandwidth_out(),
    bandwidth_uncompressed(),
    ticTime(std::chrono::system_clock::now()),
    mudTime(std::chrono::system_clock::now()),
    ticSize(10),
    secondSize(50),
    hourSize(secondSize * 3600),
    daySize(secondSize * 86400),
    mudHour(),
    mudDayPhase(DayPhase::Morning)
{
    // Nothing to do.
}

MudUpdater::~MudUpdater()
{
    // Nothing to do.
}

MudUpdater & MudUpdater::instance()
{
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static MudUpdater instance;
    // Return a reference to our instance.
    return instance;
}

void MudUpdater::initTimers()
{
    ticTime = std::chrono::system_clock::now();
    mudTime = std::chrono::system_clock::now();
}

bool MudUpdater::hasTicPassed()
{
    // Get the current time.
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    // Return the check if a tic has passed.
    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - ticTime).count() >= ticSize)
    {
        // Reset Tic Time.
        ticTime = std::chrono::system_clock::now();
        return true;
    }
    return false;
}

bool MudUpdater::hasHourPassed()
{
    // Get the current time.
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    // Return the check if a hour is passed.
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - mudTime).count() >= hourSize)
    {
        // Reset Mud Time.
        mudTime = std::chrono::system_clock::now();
        return true;
    }
    return false;
}

void MudUpdater::updateTime()
{
    // Increment the current mud hour.
    mudHour++;
    if (mudHour == 6)
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() + "The sun rises from the east.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Morning;
    }
    else if (mudHour == 12)
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() + "The sun is just above you.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Day;
    }
    else if (mudHour == 18)
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() + "The sun begins to set.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Dusk;
    }
    else if (mudHour == 24)
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() + "Darkness engulfs you.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Night;
        mudHour = 0;
    }
    else
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() + "Another hour has passed." + Formatter::reset());
    }
}

void MudUpdater::updatePlayers()
{
    for (auto iterator : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        // Update the player conditions.
        iterator->updateHealth();
        iterator->updateStamina();
        iterator->updateHunger();
        iterator->updateThirst();
        iterator->updateExpiredEffects();
        iterator->updateActivatedEffects();
    }
}

void MudUpdater::updateMobiles()
{
    for (auto iterator : Mud::instance().mudMobiles)
    {
        auto mobile = iterator.second;
        if (!mobile->isAlive())
        {
            if (mobile->canRespawn())
            {
                // Initialize the mobile, good as new.
                mobile->respawn();
            }
        }
        else
        {
            // Update the condition.
            mobile->updateHealth();
            mobile->updateStamina();
            mobile->updateHunger();
            mobile->updateThirst();
            mobile->updateExpiredEffects();
            mobile->updateActivatedEffects();
        }
    }
}

void MudUpdater::updateMobilesHour()
{
    for (auto iterator : Mud::instance().mudMobiles)
    {
        auto mobile = iterator.second;
        if (mobile->isAlive())
        {
            if (mudHour == 6)
            {
                mobile->triggerEventMorning();
            }
            else if (mudHour == 12)
            {
                mobile->triggerEventDay();
            }
            else if (mudHour == 18)
            {
                mobile->triggerEventDusk();
            }
            else if (mudHour == 24)
            {
                mobile->triggerEventNight();
            }
            else
            {
                mobile->triggerEventRandom();
            }
        }
    }
}

void MudUpdater::updateItems()
{
    std::list<Item *> itemToDestroy;
    Logger::log(LogLevel::Debug, "Updating corpses...");
    for (auto it : Mud::instance().mudCorpses)
    {
        auto corpse = it.second;
        if (HasFlag(corpse->model->modelFlags, ModelFlag::Unbreakable))
        {
            continue;
        }
        // Trigger decay function.
        if (corpse->updateCondition())
        {
            //First take everything out from the item.
            if ((corpse->room != nullptr) && (!corpse->isEmpty()))
            {
                for (auto it2: corpse->content)
                {
                    corpse->room->addItem(it2, true);
                }
            }
            itemToDestroy.insert(itemToDestroy.end(), corpse);
        }
    }
    Logger::log(LogLevel::Debug, "Updating items...");
    for (auto it : Mud::instance().mudItems)
    {
        auto item = it.second;
        Logger::log(LogLevel::Debug, "Updating '" + item->getName() + "'...");
        if (HasFlag(item->model->modelFlags, ModelFlag::Unbreakable))
        {
            continue;
        }
        // Trigger decay function.
        if (item->updateCondition())
        {
            //First take everything out from the item.
            if ((item->room != nullptr) && (!item->isEmpty()))
            {
                for (auto it2: item->content)
                {
                    item->room->addItem(it2, true);
                }
            }
            itemToDestroy.insert(itemToDestroy.end(), item);
        }
    }
    for (auto it : itemToDestroy)
    {
        it->removeFromMud();
        it->removeOnDB();
        delete (it);
    }
    Logger::log(LogLevel::Debug, "Done!");
}

void MudUpdater::performActions()
{
    for (auto iterator : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        if (iterator->getAction()->getType() == ActionType::Wait)
        {
            continue;
        }
        ActionStatus actionStatus = iterator->getAction()->perform();
        if ((actionStatus == ActionStatus::Finished) || (actionStatus == ActionStatus::Error))
        {
            // Remove the from action.
            iterator->popAction();
        }
    }
    for (auto iterator : Mud::instance().mudMobiles)
    {
        auto mobile = iterator.second;
        if (!mobile->isAlive())
        {
            continue;
        }
        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(mobile->nextActionCooldown - end).count();
        if (elapsed < 0)
        {
            mobile->triggerEventRandom();
            mobile->nextActionCooldown = end + std::chrono::seconds(TRandInteger<int>(30, 60));
        }
        if (mobile->getAction()->getType() == ActionType::Wait)
        {
            continue;
        }
        ActionStatus actionStatus = mobile->getAction()->perform();
        if ((actionStatus == ActionStatus::Finished) || (actionStatus == ActionStatus::Error))
        {
            // Remove the last action.
            mobile->popAction();
        }
    }
}

void MudUpdater::updateBandIn(const size_t & size)
{
    bandwidth_in += size;
}

void MudUpdater::updateBandOut(const size_t & size)
{
    bandwidth_out += size;
}

void MudUpdater::updateBandUncompressed(const size_t & size)
{
    bandwidth_uncompressed += size;
}

DayPhase MudUpdater::getDayPhase()
{
    return mudDayPhase;
}

unsigned int MudUpdater::getSecondSize()
{
    return secondSize;
}

unsigned int MudUpdater::getHourSize()
{
    return hourSize;
}

unsigned int MudUpdater::getDaySize()
{
    return daySize;
}

size_t MudUpdater::getBandIn()
{
    return bandwidth_in;
}

size_t MudUpdater::getBandOut()
{
    return bandwidth_out;
}

size_t MudUpdater::getBandUncompressed()
{
    return bandwidth_uncompressed;
}
