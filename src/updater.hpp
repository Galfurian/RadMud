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

#ifndef UPDATE_HPP
#define UPDATE_HPP

#include "constants.hpp"
#include "utils.hpp"

/// @brief Handle everything that it's considered dynamic inside the mud, like player or mobile.
/// @details
/// Even items could be considered dynamic, for example we can think about object's degradation.
class MudUpdater
{
    public:
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
        unsigned int mudDayPhase;

        /// @brief Constructor.
        MudUpdater();

        /// @brief Destructor.
        ~MudUpdater();

        /// @brief Disable Copy Constructor.
        MudUpdater(MudUpdater const &) = delete;

        /// @brief Disable Assign Operator.
        void operator=(MudUpdater const&) = delete;

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

        /// @brief Update bandwidth statistics.
        void updateBandWidth(int type, const size_t & size);

};

#endif
