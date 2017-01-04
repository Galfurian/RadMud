/// @file   aStar.hpp
/// @brief  Contains the astar pathfinder.
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

#include "pathFinder.hpp"
#include "aStarNode.hpp"

/// @brief The AStar algorithm.
template<typename ElementType>
class AStar :
    public PathFinder<ElementType>
{
private:
    /// List of AStar nodes.
    std::vector<std::shared_ptr<AStarNode<ElementType>>> nodes;

    /// The starting node.
    std::shared_ptr<AStarNode<ElementType>> startNode;

    /// The ending node.
    std::shared_ptr<AStarNode<ElementType>> endNode;

    /// The function which is used to determine if, during the exploration,
    ///  the algorithm can move from the element "from" to the "to".
    std::function<bool(ElementType from, ElementType to)> checkFunction;

public:
    /// @brief Create a new instance of PathFinder
    AStar();

    bool findPath(ElementType start,
                  ElementType end,
                  std::vector<ElementType> & path,
                  const std::function<bool(ElementType from,
                                           ElementType to)> & _checkFunction) override;

private:
    /// @brief Recursive function used to search the path from the
    ///         current node to the ending node.
    /// @param currentNode The current node beeing visited.
    /// @return <b>True</b> if, by visiting the current node, we have reached the end node,<br>
    ///         <b>False</b> otherwise.
    bool search(std::shared_ptr<AStarNode<ElementType>> currentNode);
};

#include "aStar.i.hpp"
