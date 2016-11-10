//
// Created by enrico on 11/10/16.
//

#pragma once

#include "room.hpp"
#include <vector>

class AStarNode
{
public:
    using NodeState = enum
    {
        Untested,
        Open,
        Closed
    };
    /// The corresponding room.
    Room * room;
    /// The length of the path from the start node to this node.
    float g;
    /// The straight-line distance from this node to the end node.
    float h;
    /// Node state.
    NodeState state;
    /// The previous node in path.
    std::shared_ptr<AStarNode> parentNode;

    AStarNode();

    AStarNode(Room * _room, Coordinates _target);

    /// Estimated total distance/cost.
    float getF() const;

    std::string toString() const;
};