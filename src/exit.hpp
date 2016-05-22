/// @file   exit.hpp
/// @brief  An exit connects two rooms and store information about: doors and exit's flags.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#ifndef EXIT_HPP
#define EXIT_HPP

#include <string>
#include <vector>

#include "defines.hpp"

class Room;

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
        int flags;

        /// Create an empty exit.
        Exit();

        /// Exit deconstructor.
        ~Exit();

        /// @brief Create a custom exit.
        /// @param _source      The source room.
        /// @param _destination The destination room.
        /// @param _direction   The direction of the exit.
        /// @param _flags       The exit flags.
        Exit(Room * _source, Room * _destination, Direction _direction, int _flags);

        /// @brief Check the correctness of the exit.
        /// @return <b>True</b> if it has correct values,<br><b>False</b> otherwise.
        bool check();

        /// @brief Get the opposite direction.
        /// @return The opposite direction.
        Direction getOppositeDirection();

        /// @brief Get the direction.
        /// @return The direction.
        std::string getDirection();

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);
};

/// Exit list handler.
typedef std::vector<Exit *> ExitList;

/// ExitList iterator.
typedef ExitList::iterator ExitListIterator;

#endif