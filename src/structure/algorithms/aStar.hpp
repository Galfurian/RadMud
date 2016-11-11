//
// Created by enrico on 11/10/16.
//

#pragma once

#include "pathFinder.hpp"
#include "aStarNode.hpp"
#include "graphNode.hpp"

template<typename ElementType>
class AStar :
    public PathFinder<ElementType>
{
public:
    std::vector<std::shared_ptr<AStarNode<ElementType>>> nodes;
    std::shared_ptr<AStarNode<ElementType>> startNode;
    std::shared_ptr<AStarNode<ElementType>> endNode;
    std::function<bool(ElementType from, ElementType to)> checkFunction;

    /// @brief Create a new instance of PathFinder
    AStar();

    bool findPath(ElementType start,
                  ElementType end,
                  std::vector<ElementType> & path,
                  const std::function<bool(ElementType from, ElementType to)> & _checkFunction);

private:
    /// Attempts to find a path to the destination node using <paramref name="currentNode"/> as the starting location
    bool search(std::shared_ptr<AStarNode<ElementType>> currentNode);

    /// Returns any nodes that are adjacent to <paramref name="fromNode"/> and may be considered to form the next step in the path
    std::vector<std::shared_ptr<AStarNode<ElementType>>> getNeighbours(
        std::shared_ptr<AStarNode<ElementType>> fromNode);
};

#include "aStar.i.hpp"