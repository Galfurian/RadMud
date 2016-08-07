/// @file   updater.cpp
/// @brief  Implements classes and method used for update everything in the mud.
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
#include "updater.hpp"

// Other Include.
#include "mud.hpp"
#include "constants.hpp"
#include "utilities/logger.hpp"

using namespace std;
using namespace std::chrono;

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
        ticTime(system_clock::now()),
        mudTime(system_clock::now()),
        ticSize(30),
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
    ticTime = system_clock::now();
    mudTime = system_clock::now();
}

bool MudUpdater::hasTicPassed()
{
    // Get the current time.
    TimeClock currentTime = system_clock::now();
    // Return the check if a tic has passed.
    if (duration_cast<seconds>(currentTime - ticTime).count() >= ticSize)
    {
        // Reset Tic Time.
        ticTime = system_clock::now();
        return true;
    }
    else
    {
        return false;
    }
}

bool MudUpdater::hasHourPassed()
{
    // Get the current time.
    TimeClock currentTime = system_clock::now();
    // Return the check if a hour is passed.
    if (duration_cast<milliseconds>(currentTime - mudTime).count() >= hourSize)
    {
        // Reset Mud Time.
        mudTime = system_clock::now();
        return true;
    }
    else
    {
        return false;
    }
}

void MudUpdater::updateTime()
{
    // Increment the current mud hour.
    mudHour++;
    if (mudHour == 6)
    {
        Mud::instance().broadcastMsg(
            0,
            Formatter::yellow() + "The sun rises from the east.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Morning;
    }
    else if (mudHour == 12)
    {
        Mud::instance().broadcastMsg(
            0,
            Formatter::yellow() + "The sun is just above you.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Day;
    }
    else if (mudHour == 18)
    {
        Mud::instance().broadcastMsg(
            0,
            Formatter::yellow() + "The sun begins to set.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Dusk;
    }
    else if (mudHour == 24)
    {
        Mud::instance().broadcastMsg(
            0,
            Formatter::yellow() + "Darkness engulfs you.\n" + Formatter::reset());
        mudDayPhase = DayPhase::Night;
        mudHour = 0;
    }
    else
    {
        Mud::instance().broadcastMsg(
            0,
            Formatter::yellow() + "Another hour has passed." + Formatter::reset());
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
        Mobile * mobile = iterator.second;
        if (!mobile->alive)
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
        if (iterator.second->alive)
        {
            if (mudHour == 6)
            {
                iterator.second->triggerEventMorning();
            }
            else if (mudHour == 12)
            {
                iterator.second->triggerEventDay();
            }
            else if (mudHour == 18)
            {
                iterator.second->triggerEventDusk();
            }
            else if (mudHour == 24)
            {
                iterator.second->triggerEventNight();
            }
            else
            {
                iterator.second->triggerEventRandom();
            }
        }
    }
}

void MudUpdater::updateItems()
{
    ItemList itemToDestroy;
    Logger::log(LogLevel::Debug, "Updating corpses...");
    for (auto it : Mud::instance().mudCorpses)
    {
        Item * corpse = it.second;
        if (HasFlag(corpse->model->modelFlags, ModelFlag::Unbreakable))
        {
            continue;
        }
        // Trigger decay function.
        if (corpse->triggerDecay())
        {
            itemToDestroy.insert(itemToDestroy.end(), corpse);
        }
    }
    Logger::log(LogLevel::Debug, "Updating items...");
    for (auto it : Mud::instance().mudItems)
    {
        Item * item = it.second;
        Logger::log(LogLevel::Debug, "Updating '" + item->getName() + "'...");
        if (HasFlag(item->model->modelFlags, ModelFlag::Unbreakable))
        {
            continue;
        }
        // Trigger decay function.
        if (item->triggerDecay())
        {
            itemToDestroy.insert(itemToDestroy.end(), item);
        }
    }
    for (auto it : itemToDestroy)
    {
        Mud::instance().remItem(it);
        it->destroy();
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
        Mobile * mobile = iterator.second;
        if (!mobile->alive)
        {
            continue;
        }
        auto end = system_clock::now();
        auto elapsed = duration_cast<seconds>(mobile->nextActionCooldown - end).count();
        if (elapsed < 0)
        {
            mobile->triggerEventRandom();
            mobile->nextActionCooldown = end + seconds(TRandInteger<int>(30, 60));
        }
        if (mobile->getAction()->getType() == ActionType::Wait)
        {
            continue;
        }
        ActionStatus actionStatus = mobile->getAction()->perform();
        if ((actionStatus == ActionStatus::Finished) || (actionStatus == ActionStatus::Error))
        {
            // Remove the from action.
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
