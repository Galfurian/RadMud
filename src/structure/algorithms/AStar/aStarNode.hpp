/// @file   aStarNode.hpp
/// @brief  Contains the class of nodes use by the astar pathfinder.
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

#include <memory>
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
    bool endNodeFlag;

public:
    /// @brief Constructor.
    AStarNode(ElementType _element);

    void setElement(ElementType _element);

    void setNodeState(const AStarNodeState & _nodeState);

    void setG(const int & _g);

    void setH(const int & _h);

    void setParentNode(std::shared_ptr<AStarNode<ElementType>> _parentNode);

    void setIsEndNode();

    ElementType getElement();

    AStarNodeState getNodeState() const;

    int getG() const;

    int getF() const;

    std::shared_ptr<AStarNode<ElementType>> getParentNode();

    bool isEndNode() const;

    bool isEqualTo(std::shared_ptr<AStarNode<ElementType>> other);

    int getDistance(std::shared_ptr<AStarNode<ElementType>> other);

    std::vector<std::shared_ptr<AStarNode<ElementType>>> getNeighbours(
        std::vector<std::shared_ptr<AStarNode<ElementType>>> & nodes,
        std::shared_ptr<AStarNode<ElementType>> endNode,
        const std::function<bool(ElementType from, ElementType to)> & checkFunction);
};

#include "aStarNode.i.hpp"
