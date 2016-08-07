/// @file   stopwatch.hpp
/// @brief  Implement a stopwatch.
/// @author Enrico Fraccaroli
/// @date   Aug 7 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include <chrono>
#include <string>
#include <iostream>

/// @brief Class which allows to gather timings.
template<typename TimeT, typename ClockT = std::chrono::high_resolution_clock,
    typename DurationT = double>
class Stopwatch
{
    private:
        /// The header used during the printing of the elapsed time.
        std::string header;
        /// The starting time.
        std::chrono::time_point<ClockT> timeStart;
        /// The ending time.
        std::chrono::time_point<ClockT> timeEnd;
    public:
        /// @brief Constructor.
        /// @param _header The header of the stopwatch.
        Stopwatch(std::string _header) :
                header(_header),
                timeStart(ClockT::now()),
                timeEnd(ClockT::now())
        {
            // Nothing to do.
        }
        /// @brief Starts the stopwatch.
        void start()
        {
            timeStart = timeEnd = ClockT::now();
        }
        /// @brief Stops the stopwatch.
        /// @return The elapsed time.
        DurationT stop()
        {
            timeEnd = ClockT::now();
            return elapsed();
        }
        /// @brief Restarts the stopwatch and provides the elapsed time.
        /// @return The elapsed time.
        DurationT restart()
        {
            DurationT ret = stop();
            start();
            return ret;
        }

        /// @brief Provides the current elapsed time.
        /// @return The elapsed time.
        DurationT elapsed() const
        {
            auto delta = std::chrono::duration_cast<TimeT>(timeEnd - timeStart);
            return static_cast<DurationT>(delta.count());
        }

        /// @brief Print the current elapsed time.
        void print() const
        {
            std::cout << header << " : " << elapsed() << std::endl;
        }
};
