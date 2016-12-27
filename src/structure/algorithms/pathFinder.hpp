/// @file   pathFinder.hpp
/// @brief  Contains the interface for a pathfinder.
/// @author Enrico Fraccaroli
/// @date   Nov 11 2016
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

#include <vector>
#include <functional>

/// @brief The abstract class used to implement a pathfinding algorithm.
template<typename ElementType>
class PathFinder
{
public:
    /// @brief Constructor.
    virtual ~PathFinder()
    {
        // Nothing to do.
    }

    /// @brief Functions which allows to find a path between the "start" and "end" element.
    /// @param start    The starting position.
    /// @param end      The ending position.
    /// @param path     Where the found path is stored.
    /// @param _checkFunction The function which is used to determine if,
    ///                        during the exploration, the algorithm can move
    ///                        from the element "from" and "to".
    /// @return <b>True</b> if there is a path between the positions,<br>
    ///         <b>False</b> otherwise.
    virtual bool findPath(ElementType start,
                          ElementType end,
                          std::vector<ElementType> & path,
                          const std::function<bool(ElementType from,
                                                   ElementType to)> & _checkFunction) = 0;
};
