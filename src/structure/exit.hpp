/// @file   exit.hpp
/// @brief  An exit connects two rooms and store information about exits.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
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

#include "direction.hpp"

#include <string>
#include <vector>
#include <memory>

class Room;

/// Used to determine the flag of the exit.
using ExitFlag = enum class ExitFlag_t
{
    None = 0,   ///< No flag.
    NoMob = 1,  ///< A mob can't move through this exit.
    Hidden = 2, ///< The exit it's hidden.
    Stairs = 4, ///< The exit has stairs.
};

/// @brief Holds details about room.
class Exit
{
public:
    /// The source room.
    Room * source;
    /// The destination room.
    Room * destination;
    /// The direction.
    Direction direction;
    /// The flags of the exit.
    unsigned int flags;

    /// Create an empty exit.
    Exit();

    /// @brief Create a custom exit.
    /// @param _source      The source room.
    /// @param _destination The destination room.
    /// @param _direction   The direction of the exit.
    /// @param _flags       The exit flags.
    Exit(Room * _source,
         Room * _destination,
         Direction _direction,
         unsigned int _flags);

    /// Exit deconstructor.
    ~Exit();

    /// @brief Check the correctness of the exit.
    /// @return <b>True</b> if it has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check() const;

    /// @brief Get the exit on the opposite side.
    /// @return The opposite exit.
    std::shared_ptr<Exit> getOppositeExit() const;

    /// @brief Unlink the current exit from its source.
    /// @return <b>True</b> if the operation succeeded,<br>
    ///         <b>False</b> otherwise.
    bool unlink() const;

    /// @brief Define equal operator.
    /// @param right The comparison exit.
    /// @return <b>True</b> if this and the right element has the
    ///                      same direction,<br>
    ///         <b>False</b> otherwise.
    bool operator==(const Exit & right) const;
};

/// @addtogroup FlagsToList
/// @{

/// Return a list of string containg the Exit flags contained inside the value.
std::string GetExitFlagString(unsigned int flags);

/// @}
