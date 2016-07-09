/// @file   updater.hpp
/// @brief  Define updates methods.
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

#pragma once

#include "constants.hpp"
#include "utils.hpp"

/// @brief Enumerator which identifies the day phase.
typedef enum class DayPhases
{
    /// General logging level.
    Morning,
    /// Log events useful for back-tracing.
    Day,
    /// Log events useful for developers.
    Dusk,
    /// Log events which may lead to errors.
    Night,
} DayPhase;

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
        TimeClock ticTime;
        /// Mud current time.
        TimeClock mudTime;

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
