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

private:
    /// The corresponding room.
    Room * room;
    /// Node state.
    NodeState nodeState;
    /// The length of the path from the start node to this node.
    float g;
    /// The straight-line distance from this node to the end node.
    float h;
    /// The previous node in path.
    std::shared_ptr<AStarNode> parentNode;

public:
    /// @brief Constructor.
    AStarNode();

    /// @brief Constructor.
    /// @param _room   The wrapped room.
    /// @param _target The target coordinates.
    AStarNode(Room * _room, Coordinates _target);

    void setNodeState(const NodeState & _nodeState);

    void setParentNode(std::shared_ptr<AStarNode> _parentNode);

    Room * getRoom();

    NodeState getNodeState() const;

    std::shared_ptr<AStarNode> getParentNode();

    /// Estimated total distance/cost.
    float getG() const;

    /// Estimated total distance/cost.
    float getF() const;

    /// Print the main infos regarding the node.
    std::string toString() const;
};