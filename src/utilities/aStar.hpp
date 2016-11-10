//
// Created by enrico on 11/10/16.
//

#pragma once

#include "aStarNode.hpp"
#include "room.hpp"

class PathFinder
{
public:
    std::map<Coordinates, std::shared_ptr<AStarNode>> nodes;
    Coordinates start;
    Coordinates end;

    /// @brief Create a new instance of PathFinder
    PathFinder() :
        nodes(),
        start(),
        end()
    {
        // Nothing to do.
    }

    std::vector<Coordinates> findPath(Room * startRoom, Room * endRoom);

    /// Attempts to find a path to the destination node using <paramref name="currentNode"/> as the starting location
    bool search(std::shared_ptr<AStarNode> currentNode);

    /// Returns any nodes that are adjacent to <paramref name="fromNode"/> and may be considered to form the next step in the path
    std::vector<std::shared_ptr<AStarNode>> getNeighbours(std::shared_ptr<AStarNode> fromNode);
};
