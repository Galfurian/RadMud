/// @file   aStar.hpp
/// @brief  Contains the astar pathfinder.
/// @author Enrico Fraccaroli
/// @date   Nov 11 2016
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

#include "pathFinder.hpp"
#include "aStarNode.hpp"

template<typename ElementType>
class AStar :
    public PathFinder<ElementType>
{
private:
    std::vector<std::shared_ptr<AStarNode<ElementType>>> nodes;
    std::shared_ptr<AStarNode<ElementType>> startNode;
    std::shared_ptr<AStarNode<ElementType>> endNode;
    std::function<bool(ElementType from, ElementType to)> checkFunction;

public:
    /// @brief Create a new instance of PathFinder
    AStar();

    bool findPath(ElementType start,
                  ElementType end,
                  std::vector<ElementType> & path,
                  const std::function<bool(ElementType from, ElementType to)> & _checkFunction) override;

private:
    /// Attempts to find a path to the destination node using <paramref name="currentNode"/> as the starting location
    bool search(std::shared_ptr<AStarNode<ElementType>> currentNode);
};

#include "aStar.i.hpp"
