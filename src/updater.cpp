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
#include "logger.hpp"
#include "actions.hpp"
#include "constants.hpp"

using namespace std;

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
        ticSize(30),
        secondSize(50),
        hourSize(secondSize * 3600),
        daySize(secondSize * 86400),
        mudHour(),
        mudDayPhase()
{
    // Nothing to do.
}

MudUpdater::~MudUpdater()
{
    // Nothing to do.
}

void MudUpdater::initTimers()
{
    ticTime = std::chrono::system_clock::now();
    mudTime = std::chrono::system_clock::now();
}

bool MudUpdater::hasTicPassed()
{
    // Get the current time.
    TimeClock currentTime = std::chrono::system_clock::now();
    // Return the check if a tic has passed.
    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - ticTime).count() >= ticSize)
    {
        // Reset Tic Time.
        ticTime = std::chrono::system_clock::now();
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
    TimeClock currentTime = std::chrono::system_clock::now();
    // Return the check if a hour is passed.
    if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - mudTime).count() >= hourSize)
    {
        // Reset Mud Time.
        mudTime = std::chrono::system_clock::now();
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
    switch (mudHour)
    {
        default:
            Mud::getInstance().broadcastMsg(0, Telnet::yellow() + "Another hour has passed." + Telnet::reset());
            break;
        case 0:
            // MORNING
            Mud::getInstance().broadcastMsg(0, Telnet::yellow() + "The sun rises from the east.\n" + Telnet::reset());
            mudDayPhase = 0;
            break;
        case 6:
            // DAY
            Mud::getInstance().broadcastMsg(0, Telnet::yellow() + "The sun is just above you.\n" + Telnet::reset());
            mudDayPhase = 1;
            break;
        case 12:
            // DUSK
            Mud::getInstance().broadcastMsg(0, Telnet::yellow() + "The sun begins to set.\n" + Telnet::reset());
            mudDayPhase = 2;
            break;
        case 18:
            // NIGHT
            Mud::getInstance().broadcastMsg(0,
                Telnet::yellow() + "The sun disappears behind the horizon, darkness engulfs you.\n" + Telnet::reset());
            mudDayPhase = 3;
            break;
        case 24:
            // Day length overflow, return to Morning.
            Logger::log(LogLevel::Info, "A day has passed!");
            mudDayPhase = 0;
            mudHour = 0;
            break;
    }
}

void MudUpdater::updatePlayers()
{
    for (auto iterator : Mud::getInstance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        // Update the modifiers.
        std::vector<std::string> expirationMessages;
        if (iterator->effects.effectUpdate(expirationMessages))
        {
            for (auto message : expirationMessages)
            {
                iterator->sendMsg(message + "\n");
            }
        }

        // Activate the modifiers.
        std::vector<std::string> activationMessages;
        if (iterator->effects.effectActivate(activationMessages))
        {
            for (auto message : activationMessages)
            {
                iterator->sendMsg(message + "\n");
            }
        }

        // Update the player resources (health, stamina).
        iterator->updateResources();
    }
}

void MudUpdater::updateMobiles()
{
    for (auto iterator : Mud::getInstance().mudMobiles)
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
            // Update the modifiers.
            std::vector<std::string> expirationMessages;
            if (mobile->effects.effectUpdate(expirationMessages))
            {
                for (auto message : expirationMessages)
                {
                    mobile->sendMsg(message + "\n");
                }
            }

            // Activate the modifiers.
            std::vector<std::string> activationMessages;
            if (mobile->effects.effectActivate(activationMessages))
            {
                for (auto message : activationMessages)
                {
                    mobile->sendMsg(message + "\n");
                }
            }

            // Update the resources (health, stamina).
            mobile->updateResources();
        }
    }
}

void MudUpdater::updateMobilesHour()
{
    for (auto iterator : Mud::getInstance().mudMobiles)
    {
        if (iterator.second->alive)
        {
            switch (mudHour)
            {
                case 0:
                    // MORNING
                    iterator.second->triggerEventMorning();
                    break;
                case 6:
                    // DAY
                    iterator.second->triggerEventDay();
                    break;
                case 12:
                    // DUSK
                    iterator.second->triggerEventDusk();
                    break;
                case 18:
                    // NIGHT
                    iterator.second->triggerEventNight();
                    break;
                default:
                    // EVERY HOUR
                    iterator.second->triggerEventRandom();
                    break;
            }
        }
    }
}

void MudUpdater::updateItems()
{
    ItemList itemToDestroy;
    Logger::log(LogLevel::Debug, "Updating corpses...");
    for (ItemList::iterator it = Mud::getInstance().mudCorpses.begin(); it != Mud::getInstance().mudCorpses.end(); ++it)
    {
        Item * corpse = *it;
        if (HasFlag(corpse->model->flags, ModelFlag::Unbreakable))
        {
            continue;
        }
        // Trigger decay function.
        corpse->decayCondition();
        if (corpse->condition <= 0)
        {
            itemToDestroy.insert(itemToDestroy.end(), corpse);
        }
    }
    Logger::log(LogLevel::Debug, "Updating items...");
    for (ItemList::iterator it = Mud::getInstance().mudItems.begin(); it != Mud::getInstance().mudItems.end(); ++it)
    {
        Item * item = *it;
        Logger::log(LogLevel::Debug, "Updating '" + item->getName() + "'...");
        if (HasFlag(item->model->flags, ModelFlag::Unbreakable))
        {
            continue;
        }
        // Trigger decay function.
        item->decayCondition();
        if (item->condition <= 0)
        {
            itemToDestroy.insert(itemToDestroy.end(), item);
        }
    }
    for (auto it : itemToDestroy)
    {
        FindErase(Mud::getInstance().mudItems, it);
        it->destroy();
    }
    Logger::log(LogLevel::Debug, "Done!");
}

void MudUpdater::performActions()
{
    for (auto iterator : Mud::getInstance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }

        // ////////////////////////////////////////////////////////////////////
        iterator->action.perform();
    }

    for (auto iterator : Mud::getInstance().mudMobiles)
    {
        Mobile * mobile = iterator.second;
        if (!mobile->alive)
        {
            continue;
        }

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(mobile->nextActionCooldown - end).count();
        if (elapsed < 0)
        {
            mobile->triggerEventRandom();
            mobile->nextActionCooldown = end + std::chrono::seconds(RandInteger(30, 60));
        }

        mobile->action.perform();
    }
}

void MudUpdater::updateBandWidth(int type, const size_t & size)
{
    if (type == 0)
    {
        bandwidth_in += size;
    }
    else if (type == 1)
    {
        bandwidth_out += size;
    }
    else if (type == 2)
    {
        bandwidth_uncompressed += size;
    }
}
