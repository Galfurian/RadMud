//
// Created by enrico on 11/10/16.
//

#pragma once

#include "room.hpp"

#include "graphNode.hpp"

#include <vector>

using AStarNodeState = enum class AStarNodeState_t
{
    Untested,
    Open,
    Closed
};

template<typename ElementType>
class AStarNode
{
public:
    /// Wrapped element.
    ElementType element;

private:
    /// Node state.
    AStarNodeState nodeState;
    /// The length of the path from the start node to this node.
    int g;
    /// The straight-line distance from this node to the end node.
    int h;
    /// The previous node in path.
    std::shared_ptr<AStarNode<ElementType>> parentNode;
    /// Identifies the end node.
    bool endNode;

public:
    /// @brief Constructor.
    AStarNode();

    void setElement(ElementType _element);

    void setNodeState(const AStarNodeState & _nodeState);

    void setG(const int & _g);

    void setH(const int & _h);

    void setParentNode(std::shared_ptr<AStarNode<ElementType>> _parentNode);

    void setIsEndNode();

    ElementType getElement();

    AStarNodeState getNodeState() const;

    /// Estimated total distance/cost.
    int getG() const;

    /// Estimated total distance/cost.
    int getF() const;

    std::shared_ptr<AStarNode<ElementType>> getParentNode();

    bool isEndNode() const;
};

#include "aStarNode.i.hpp"