/// @file   aStar.hpp
/// @brief  Contains the astar pathfinder.
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

#include "structure/algorithms/pathFinder.hpp"
#include "structure/algorithms/AStar/aStarNode.hpp"
#include <algorithm>

/// @brief The AStar algorithm.
template<typename T>
class AStar :
    public PathFinder<T>
{
private:
    /// List of AStar nodes.
    std::vector<std::shared_ptr<AStarNode<T>>> nodes;
    /// The starting node.
    std::shared_ptr<AStarNode<T>> startNode;
    /// The ending node.
    std::shared_ptr<AStarNode<T>> endNode;

public:
    /// @brief Create a new instance of AStar.
    AStar(const std::function<bool(T e1, T e2)> & _checkConnection,
          const std::function<int(T e1, T e2)> & _getDistance,
          const std::function<bool(T e1, T e2)> & _areEqual,
          const std::function<std::vector<T>(T e)> & _getNeighbours) :
        PathFinder<T>(
            _checkConnection,
            _getDistance,
            _areEqual,
            _getNeighbours),
        nodes(),
        startNode(),
        endNode()
    {
        // Nothing to do.
    }

    bool findPath(T start, T end, std::vector<T> & path) override
    {
        // Create the starting and end nodes.
        startNode = std::make_shared<AStarNode<T>>(start);
        endNode = std::make_shared<AStarNode<T>>(end);
        // Add the starting and ending nodes to the list of nodes.
        nodes.emplace_back(startNode);
        nodes.emplace_back(endNode);
        // Rise the endNode flag for the end node.
        endNode->setIsEndNode();
        // Set the G-Value and H-Value for the starting node.
        startNode->setG(0);
        startNode->setH(this->getDistance(start, end));
        // Start the search.
        if (this->search(startNode))
        {
            // If a path was found, follow the parents from the end node to build a list of locations.
            auto node = endNode;
            while (node->getParentNode() != nullptr)
            {
                path.emplace_back(node->getElement());
                node = node->getParentNode();
            }
            std::reverse(path.begin(), path.end());
            return true;
        }
        return false;
    }

private:
    /// @brief Recursive function used to search the path from the
    ///         current node to the ending node.
    /// @param currentNode The current node beeing visited.
    /// @return <b>True</b> if, by visiting the current node,
    ///                     we have reached the end node,<br>
    ///         <b>False</b> otherwise.
    bool search(std::shared_ptr<AStarNode<T>> currentNode)
    {
        // Set the current node to Closed since it cannot be traversed more than once
        currentNode->setNodeState(AStarNodeState::Closed);
        auto neighbours(this->getAStarNeighbours(currentNode));
        // Continue the search for each neighbour.
        for (std::shared_ptr<AStarNode<T>> & neighbour : neighbours)
        {
            // Ignore already-closed nodes
            if (neighbour->getNodeState() == AStarNodeState::Closed)
            {
                continue;
            }
            // Evaluate the G-value for the neighbour.
            int gTemp = currentNode->getG() +
                        this->getDistance(currentNode->getElement(),
                                          neighbour->getElement());
            // Already-open nodes are only added to the list if their G-value
            // is lower going via this route.
            if (neighbour->getNodeState() == AStarNodeState::Open)
            {
                if (gTemp < neighbour->getG())
                {
                    // Change the parent of the neighbour node.
                    neighbour->setParentNode(currentNode);
                    // Set the new G-Value.
                    neighbour->setG(gTemp);
                    // Check whether the end node has been reached.
                    if (this->areEqual(endNode->getElement(),
                                       neighbour->getElement()))
                    {
                        return true;
                    }
                    // If not, check the next set of nodes.
                    if (this->search(neighbour))
                    {
                        return true;
                    }
                }
            }
            else
            {
                // Change the state of the node as Open.
                neighbour->setNodeState(AStarNodeState::Open);
                // Change the parent of the neighbour node.
                neighbour->setParentNode(currentNode);
                // Set the new G-Value.
                neighbour->setG(gTemp);
                // Check whether the end node has been reached.
                if (this->areEqual(endNode->getElement(),
                                   neighbour->getElement()))
                {
                    return true;
                }
                // If not, check the next set of nodes.
                if (this->search(neighbour))
                {
                    return true;
                }
            }
        }
        // The method returns false if this path leads to be a dead end.
        return false;
    }

    /// @brief Provides the list of neighbours of the given node wrapped by
    /// an AStarNode.
    std::vector<std::shared_ptr<AStarNode<T>>> getAStarNeighbours(
        std::shared_ptr<AStarNode<T>> currentNode)
    {
        // Prepare a structure which will contain all the neighbours.
        std::vector<std::shared_ptr<AStarNode<T>>> neighbours;
        // Check if the given neighbour.
        for (T neighbour : this->getNeighbours(currentNode->getElement()))
        {
            // Check if the neighbour is valid.
            if (this->checkConnection(currentNode->getElement(), neighbour))
            {
                // Check if there is already a wrapper for the given node.
                bool found = false;
                for (const std::shared_ptr<AStarNode<T>> & aStarNode : nodes)
                {
                    if (this->areEqual(aStarNode->getElement(), neighbour))
                    {
                        neighbours.emplace_back(aStarNode);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    auto newNode = std::make_shared<AStarNode<T>>(neighbour);
                    newNode->setG(currentNode->getG() +
                                  this->getDistance(currentNode->getElement(),
                                                    neighbour));
                    newNode->setH(
                        this->getDistance(neighbour,
                                          endNode->getElement()));
                    nodes.emplace_back(newNode);
                    neighbours.emplace_back(newNode);
                }
            }
        }
        // Reorder the neighbours so that the one with the lesser
        // G-Value comes first.
        std::sort(neighbours.begin(),
                  neighbours.end(),
                  [](const std::shared_ptr<AStarNode<T>> & left,
                     const std::shared_ptr<AStarNode<T>> & right)
                  {
                      return left->getF() < right->getF();
                  });
        return neighbours;
    }
};

