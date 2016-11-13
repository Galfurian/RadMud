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

/// @brief Enumerator which identifies the day phase.
#include <chrono>

using DayPhase = enum class DayPhase_t
{
    Morning,    ///< General logging level.
    Day,        ///< Log events useful for back-tracing.
    Dusk,       ///< Log events useful for developers.
    Night,      ///< Log events which may lead to errors.
};

/// @brief Handle everything that it's considered dynamic inside the mud, like player or mobile.
/// @details
/// Even items could be considered dynamic, for example we can think about object's degradation.
class MudUpdater
{
private:
    /// The number of bytes received from players.
    size_t bandwidth_in;
    /// The number of bytes sent to players.
    size_t bandwidth_out;
    /// The number of bytes without compression.
    size_t bandwidth_uncompressed;

    /// Mud current time.
    std::chrono::time_point<std::chrono::system_clock> ticTime;
    /// Mud current time.
    std::chrono::time_point<std::chrono::system_clock> mudTime;

    /// Mud tic length.
    unsigned int ticSize;
    /// Mud seconds length.
    unsigned int secondSize;
    /// Mud hour length.
    unsigned int hourSize;
    /// Mud day length.
    unsigned int daySize;

    /// Mud current hour.
    unsigned int mudHour;
    /// Mud current day phase.
    DayPhase mudDayPhase;

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

    /// @brief Initialize the timers.
    void initTimers();

    /// @brief Check if the mud tic has passed.
    bool hasTicPassed();

    /// @brief Check if the mud hour has passed.
    bool hasHourPassed();

    /// @brief Update the day phase and the hour of the mud.
    void updateTime();

    /// @brief Update every player in the mud.
    void updatePlayers();

    /// @brief Update every mobile in the mud.
    void updateMobiles();

    /// @brief Update every mobile in the mud.
    void updateMobilesHour();

    /// @brief Update every item in the mud.
    void updateItems();

    /// @brief Perform pending actions.
    void performActions();

    /// @brief Update input bandwidth.
    void updateBandIn(const size_t & size);

    /// @brief Update output bandwidth.
    void updateBandOut(const size_t & size);

    /// @brief Update uncompressed bandwidth.
    void updateBandUncompressed(const size_t & size);

    /// @brief Provides the current mud day phase.
    /// @return The enumberator which identifies the day phase.
    DayPhase getDayPhase();

    /// @brief Provides the size of a second in terms of milliseconds (MS).
    /// @return The size of a second.
    unsigned int getSecondSize();

    /// @brief Provides the size of a hour in terms of milliseconds (MS).
    /// @return The size of a hour.
    unsigned int getHourSize();

    /// @brief Provides the size of a day in terms of milliseconds (MS).
    /// @return The size of a day.
    unsigned int getDaySize();

    /// @brief Provides the total input (from clients) bandwidth.
    /// @return The input bandwidth.
    size_t getBandIn();

    /// @brief Provides the total output (to clients) bandwidth.
    /// @return The output bandwidth.
    size_t getBandOut();

    /// @brief Provides the total uncompressed (to clients) bandwidth.
    /// @return The uncompressed bandwidth.
    size_t getBandUncompressed();
};
