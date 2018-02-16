/// @file   updater.hpp
/// @brief  Define updates methods.
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

#include <chrono>
#include <list>

// Forward declarations.
class Item;

class Room;

/// @brief Enumerator which identifies the day phase.
enum class DayPhase
{
    Morning = 6,    ///< The first lights of the day.
    Day = 12,       ///< The sun is high in the sky.
    Dusk = 18,      ///< The sun begin to set.
    Night = 24,     ///< The darkness, and nothing more.
};

/// @brief Handle everything that it's considered dynamic inside the mud,
///         like player or mobile.
/// @details
/// Even items could be considered dynamic, for example we can think about
///  object's degradation.
class MudUpdater
{
private:
    /// The number of bytes received from players.
    size_t bandwidth_in;
    /// The number of bytes sent to players.
    size_t bandwidth_out;
    /// The number of bytes without compression.
    size_t bandwidth_uncompressed;

    /// The timer usd to determine if a TIC is passed.
    std::chrono::time_point<std::chrono::system_clock> ticTime;
    /// Mud TIC length in seconds.
    const unsigned int ticSize;
    /// The lenght of an hour in TIC.
    const unsigned int hourTicSize;
    /// Counter used to determine if an hour is passed.
    unsigned int hourTicCounter;

    /// Mud current hour.
    unsigned int mudHour;
    /// Mud current day phase.
    DayPhase mudDayPhase;

    // Garbage collection structures.
    /// List of item that has to be descroyed at the end of the mud cycle.
    std::list<Item *> itemToDestroy;

    /// @brief Constructor.
    MudUpdater();

    /// @brief Destructor.
    ~MudUpdater();

public:
    /// @brief Disable Copy Construct.
    MudUpdater(MudUpdater const &) = delete;

    /// @brief Disable Move construct.
    MudUpdater(MudUpdater &&) = delete;

    /// @brief Disable Copy assign.
    MudUpdater & operator=(MudUpdater const &) = delete;

    /// @brief Disable Move assign.
    MudUpdater & operator=(MudUpdater &&) = delete;

    /// @brief Get the singleton istance of the MudUpdater.
    /// @return The static and uniquie MudUpdater variable.
    static MudUpdater & instance();

    /// @brief Update input bandwidth.
    void updateBandIn(const size_t & size);

    /// @brief Update output bandwidth.
    void updateBandOut(const size_t & size);

    /// @brief Update uncompressed bandwidth.
    void updateBandUncompressed(const size_t & size);

    /// @brief Add the item to the list of items that will be destroyed at
    /// the end of the MUD TIC.
    void addItemToDestroy(Item * item);

    /// @brief Add the room to the list of rooms that will be destroyed at
    /// the end of the MUD TIC.
    void addRoomToDestroy(Room * room);

    /// @brief Provides the dimension of the TIC.
    unsigned int getTicSize() const;

    /// @brief Provides the dimension of the hour in TICs.
    unsigned int getHourTicSize() const;

    /// @brief Provides the total input (from clients) bandwidth.
    size_t getBandIn() const;

    /// @brief Provides the total output (to clients) bandwidth.
    size_t getBandOut() const;

    /// @brief Provides the total uncompressed (to clients) bandwidth.
    size_t getBandUncompressed() const;

    /// @brief Provides the current mud hour.
    unsigned int getMudHour() const;

    /// @brief Provides the current mud day phase.
    DayPhase getDayPhase() const;

    /// @brief Allows the time to advance.
    void advanceTime();

private:
    /// @brief Check if the mud tic has passed.
    bool hasTicPassed();

    /// @brief Update the day phase and the hour of the mud.
    void updateDayPhase();

    /// @brief Perform pending actions.
    void performActions();
};
