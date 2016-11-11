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
    AStarNode() :
        nodeState(),
        g(),
        h(),
        parentNode(),
        endNode()
    {
        // Nothing to do.
    }

    void setElement(ElementType _element)
    {
        element = _element;
    }

    void setNodeState(const AStarNodeState & _nodeState)
    {
        nodeState = _nodeState;
    }


    void setG(const int & _g)
    {
        g = _g;
    }

    void setH(const int & _h)
    {
        h = _h;
    }

    void setParentNode(std::shared_ptr<AStarNode<ElementType>> _parentNode)
    {
        parentNode = _parentNode;
    }

    void setIsEndNode()
    {
        endNode = true;
    }

    ElementType getElement()
    {
        return element;
    }

    AStarNodeState getNodeState() const
    {
        return nodeState;
    }

    /// Estimated total distance/cost.
    int getG() const
    {
        return g;
    }

    /// Estimated total distance/cost.
    int getF() const
    {
        return g + h;
    }

    std::shared_ptr<AStarNode<ElementType>> getParentNode()
    {
        return parentNode;
    }

    bool isEndNode() const
    {
        return endNode;
    }
};
