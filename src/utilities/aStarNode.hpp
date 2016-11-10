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
    /// Node state.
    NodeState state;
    /// The length of the path from the start node to this node.
    float g;
    /// The straight-line distance from this node to the end node.
    float h;
    /// The previous node in path.
    std::shared_ptr<AStarNode> parentNode;

    /// @brief Constructor.
    AStarNode();

    /// @brief Constructor.
    /// @param _room   The wrapped room.
    /// @param _target The target coordinates.
    AStarNode(Room * _room, Coordinates _target);

    /// Estimated total distance/cost.
    float getF() const;

    /// Print the main infos regarding the node.
    std::string toString() const;
};