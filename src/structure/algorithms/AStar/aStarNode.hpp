/// @file   aStarNode.hpp
/// @brief  Contains the class of nodes use by the astar pathfinder.
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
