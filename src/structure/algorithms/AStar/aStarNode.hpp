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

/// @brief The states of an AStar node.
using AStarNodeState = enum class AStarNodeState_t
{
    Untested,   /// <! The node has not been tested yet.
    Open,       /// <! The node is on the 'open' state.
    Closed      /// <! The node is on the 'closed' state.
};

/// @brief A supporting node for the AStart algorithm.
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

    /// The previous node in path. It is used when recontructing the path
    ///  from the end node to the beginning.
    std::shared_ptr<AStarNode<ElementType>> parentNode;

    /// Identifies the end node.
    bool endNodeFlag;

public:
    /// @brief Constructor.
    AStarNode(ElementType _element);

    /// @brief Allows to set the wrapped element.
    void setElement(ElementType _element);

    /// @brief Allows to set the state of the node.
    void setNodeState(const AStarNodeState & _nodeState);

    /// @brief Allows to set the 'g' value.
    void setG(const int & _g);

    /// @brief Allows to set the 'h' value.
    void setH(const int & _h);

    /// @brief Allows to set the parent node.
    void setParentNode(std::shared_ptr<AStarNode<ElementType>> _parentNode);

    /// @brief Allows to set if this node is the end node.
    void setIsEndNode();

    /// @brief Provides the wrapped element.
    ElementType getElement();

    /// @brief Provides the state of the node.
    AStarNodeState getNodeState() const;

    /// @brief Provides the 'g' value.
    int getG() const;

    /// @brief Provides the 'f' value.
    int getF() const;

    /// @brief Privdes the parent node.
    std::shared_ptr<AStarNode<ElementType>> getParentNode();

    /// @brief Gives information if this node is the end node.
    bool isEndNode() const;

    /// @brief Equality between this and the 'other' AStar node.
    bool isEqualTo(std::shared_ptr<AStarNode<ElementType>> other);

    /// @brief Provides the distance between this and the 'other' AStar node.
    int getDistance(std::shared_ptr<AStarNode<ElementType>> other);

    /// @brief Provides all the neighbours of this node.
    /// @param nodes         The nodes of the AStar algorithm.
    /// @param endNode       The end node.
    /// @param checkFunction The checking function.
    /// @return A vector containing all the 'valid' neighbours.
    std::vector<std::shared_ptr<AStarNode<ElementType>>> getNeighbours(
        std::vector<std::shared_ptr<AStarNode<ElementType>>> & nodes,
        std::shared_ptr<AStarNode<ElementType>> endNode,
        const std::function<bool(ElementType from,
                                 ElementType to)> & checkFunction);
};

#include "aStarNode.i.hpp"
