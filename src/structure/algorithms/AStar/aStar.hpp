//
// Created by enrico on 11/10/16.
//

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
