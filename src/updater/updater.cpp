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

#include "utilities/logger.hpp"
#include "updater/updater.hpp"
#include "character/behaviour/generalBehaviour.hpp"
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
    ticSize(10),
    hourTicSize(2),
    hourTicCounter(),
    mudHour(),
    mudDayPhase(DayPhase::Day),
    itemToDestroy()
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

void MudUpdater::addItemToDestroy(Item * item)
{
    itemToDestroy.insert(itemToDestroy.end(), item);
}

unsigned int MudUpdater::getTicSize() const
{
    return ticSize;
}

unsigned int MudUpdater::getHourTicSize() const
{
    return hourTicSize;
}

size_t MudUpdater::getBandIn() const
{
    return bandwidth_in;
}

size_t MudUpdater::getBandOut() const
{
    return bandwidth_out;
}

size_t MudUpdater::getBandUncompressed() const
{
    return bandwidth_uncompressed;
}

unsigned int MudUpdater::getMudHour() const
{
    return mudHour;
}

DayPhase MudUpdater::getDayPhase() const
{
    return mudDayPhase;
}

void MudUpdater::advanceTime()
{
    // Check if a tic is passed.
    if (this->hasTicPassed())
    {
        // [TIC] Update the Players.
        for (auto iterator : Mud::instance().mudPlayers)
        {
            iterator->updateTic();
        }
        // [TIC] Update the Mobiles.
        for (auto iterator : Mud::instance().mudMobiles)
        {
            iterator->updateTic();
        }
        // [TIC] Update the Items.
        for (auto iterator : Mud::instance().mudItems)
        {
            iterator.second->updateTic();
        }
        // [TIC] Update the Rooms.
        for (auto iterator : Mud::instance().mudRooms)
        {
            iterator.second->updateTic();
        }
        // Check if a hour is passed.
        if (hourTicCounter++ >= hourTicSize)
        {
            // [HOUR] Update the day phase.
            this->updateDayPhase();
            // [HOUR] Update the Players.
            for (auto iterator : Mud::instance().mudPlayers)
            {
                iterator->updateHour();
            }
            // [HOUR] Update the Mobiles.
            for (auto iterator : Mud::instance().mudMobiles)
            {
                iterator->updateHour();
            }
            // [HOUR] Update the Corpses.
            for (auto it : Mud::instance().mudCorpses)
            {
                it.second->updateHour();
            }
            // [HOUR] Update the Items.
            for (auto it : Mud::instance().mudItems)
            {
                it.second->updateHour();
            }
            // [HOUR] Update the Rooms.
            for (auto iterator : Mud::instance().mudRooms)
            {
                iterator.second->updateHour();
            }
            // [HOUR] Reset the hour counter.
            hourTicCounter = 0;
        }
    }
    // [DELTA] Perform characters pending actions.
    this->performActions();
    // [DELTA] Destroy all the registered items.
    for (auto it = itemToDestroy.begin(); it != itemToDestroy.end(); ++it)
    {
        // Back-up the iterator.
        auto currentIt = it++;
        // Delete the item.
        auto item = (*currentIt);
        item->removeFromMud();
        item->removeOnDB();
        delete (item);
        // Erase the element.
        itemToDestroy.erase(currentIt);
    }
}

bool MudUpdater::hasTicPassed()
{
    // Check if the tic is passed.
    if (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() -
        ticTime).count() >= ticSize)
    {
        // Reset Tic Time.
        ticTime = std::chrono::system_clock::now();
        return true;
    }
    return false;
}

void MudUpdater::updateDayPhase()
{
    // Increment the current mud hour.
    mudHour++;
    if (mudHour == static_cast<unsigned int>(DayPhase::Morning))
    {
        Mud::instance().broadcastMsg(0,
                                     Formatter::yellow() +
                                     "The sun rises from the east.\n" +
                                     Formatter::reset());
        mudDayPhase = DayPhase::Morning;
    }
    else if (mudHour == static_cast<unsigned int>(DayPhase::Day))
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() +
                                        "The sun is just above you.\n" +
                                        Formatter::reset());
        mudDayPhase = DayPhase::Day;
    }
    else if (mudHour == static_cast<unsigned int>(DayPhase::Dusk))
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() +
                                        "The sun begins to set.\n" +
                                        Formatter::reset());
        mudDayPhase = DayPhase::Dusk;
    }
    else if (mudHour == static_cast<unsigned int>(DayPhase::Night))
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() +
                                        "Darkness engulfs you.\n" +
                                        Formatter::reset());
        mudDayPhase = DayPhase::Night;
        // Reset the mud hour.
        mudHour = 0;
    }
    else
    {
        Mud::instance().broadcastMsg(0, Formatter::yellow() +
                                        "Another hour has passed." +
                                        Formatter::reset());
    }
}

void MudUpdater::performActions()
{
    for (auto player : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!player->isPlaying())
        {
            continue;
        }
        // Perform the action.
        player->performAction();
    }
    for (auto mobile : Mud::instance().mudMobiles)
    {
        // If the mobile is not alive, continue.
        if (!mobile->isAlive())
        {
            continue;
        }
        // Perform the behaviour.
        mobile->performBehaviour();
        // Perform the action.
        mobile->performAction();
    }
}
