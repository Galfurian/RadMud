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

#include "structure/algorithms/pathFinderNode.hpp"

/// @brief The abstract class used to implement a pathfinding algorithm.
template<typename T>
class PathFinder
{
protected:
    /// Checks if there is a connection between the two elements.
    std::function<bool(T e1, T e2)> checkConnection;
    /// Functions used to get the distance between the two elements.
    std::function<int(T e1, T e2)> getDistance;
    /// Function used to check the equality between two elements.
    std::function<bool(T e1, T e2)> areEqual;
    /// Function used to get the neighbours of the given element.
    std::function<std::vector<T>(T e)> getNeighbours;

public:
    /// @brief Constructor.
    /// @param _checkConnection Function used to check if there is a valid
    ///                         connection between two elements.
    /// @param _getDistance     Function which provides the distance between
    ///                         two elements.
    /// @param _areEqual        Function which checks if two elements are the
    ///                         same.
    /// @param _getNeighbours   Function which provides the list of
    ///                         neighbours of an element.
    PathFinder(const std::function<bool(T e1, T e2)> & _checkConnection,
               const std::function<int(T e1, T e2)> & _getDistance,
               const std::function<bool(T e1, T e2)> & _areEqual,
               const std::function<std::vector<T>(T e)> & _getNeighbours) :
        checkConnection(_checkConnection),
        getDistance(_getDistance),
        areEqual(_areEqual),
        getNeighbours(_getNeighbours)
    {
        // Nothing to do.
    }

    /// @brief Constructor.
    virtual ~PathFinder()
    {
        // Nothing to do.
    }

    /// @brief Functions which allows to find a path between the "start"
    ///         and "end" element.
    /// @param start The starting position.
    /// @param end   The ending position.
    /// @param path  Where the found path is stored.
    /// @return <b>True</b> if there is a path between the positions,<br>
    ///         <b>False</b> otherwise.
    virtual bool findPath(T start, T end, std::vector<T> & path) = 0;
};
