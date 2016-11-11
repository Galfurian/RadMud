//
// Created by enrico on 11/10/16.
//

#pragma once

#include "pathFinder.hpp"
#include "aStarNode.hpp"
#include "graphNode.hpp"

template<typename ElementType>
class AStar :
    public PathFinder<ElementType>
{
public:
    std::vector<std::shared_ptr<AStarNode<ElementType>>> nodes;
    std::shared_ptr<AStarNode<ElementType>> startNode;
    std::shared_ptr<AStarNode<ElementType>> endNode;
    std::function<bool(ElementType from, ElementType to)> checkFunction;

    /// @brief Create a new instance of PathFinder
    AStar() :
        nodes(),
        startNode(),
        endNode()
    {
        // Nothing to do.
    }

    bool findPath(ElementType start,
                  ElementType end,
                  std::vector<ElementType> & path,
                  const std::function<bool(ElementType from, ElementType to)> & _checkFunction)
    {
        startNode = std::make_shared<AStarNode<ElementType>>();
        startNode->setG(start->getDistance(end));
        startNode->setElement(start);
        nodes.emplace_back(startNode);

        endNode = std::make_shared<AStarNode<ElementType>>();
        endNode->setElement(end);
        endNode->setIsEndNode();
        nodes.emplace_back(endNode);

        checkFunction = _checkFunction;

        Logger::log(LogLevel::Debug, "Searching...");
        if (this->search(startNode))
        {
            Logger::log(LogLevel::Debug, "Found!");
            // If a path was found, follow the parents from the end node to build a list of locations.
            auto node = endNode;
            while (node->getParentNode() != nullptr)
            {
                path.emplace_back(node->element);
                node = node->getParentNode();
            }
            std::reverse(path.begin(), path.end());
        }
        else
        {
            Logger::log(LogLevel::Debug, "No path.");
            return false;
        }
        return true;
    }

    /// Attempts to find a path to the destination node using <paramref name="currentNode"/> as the starting location
    bool search(std::shared_ptr<AStarNode<ElementType>> currentNode)
    {
        // Set the current node to Closed since it cannot be traversed more than once
        currentNode->setNodeState(AStarNodeState::Closed);
        // Get the next nodes.
        auto neighbours = this->getNeighbours(currentNode);
        std::sort(neighbours.begin(), neighbours.end(), [](const std::shared_ptr<AStarNode<ElementType>> & left,
                                                           const std::shared_ptr<AStarNode<ElementType>> & right)
        {
            if (left->getF() < right->getF()) return true;
            if (left->getF() > right->getF()) return false;
            return false;
        });
        for (auto nextNode : neighbours)
        {
            // Check whether the end node has been reached
            if (endNode->getElement()->isEqualTo(nextNode->getElement()))
            {
                Logger::log(LogLevel::Debug, "        End!");
                return true;
            }
            else
            {
                Logger::log(LogLevel::Debug, "        Continue...");
                // If not, check the next set of nodes
                if (this->search(nextNode))
                {
                    return true;
                }
            }
        }
        // The method returns false if this path leads to be a dead end.
        return false;
    }

    /// Returns any nodes that are adjacent to <paramref name="fromNode"/> and may be considered to form the next step in the path
    std::vector<std::shared_ptr<AStarNode<ElementType>>> getNeighbours(std::shared_ptr<AStarNode<ElementType>> fromNode)
    {
        std::vector<std::shared_ptr<AStarNode<ElementType>>> neighbours;
        std::vector<ElementType> unwrappedNeighbours = fromNode->getElement()->getNeighbours(checkFunction);
        for (auto it : unwrappedNeighbours)
        {
            auto it2 = nodes.begin();
            while (it2 != nodes.end())
            {
                if ((*it2)->getElement()->isEqualTo(it)) break;
                it2++;
            }
            std::shared_ptr<AStarNode<ElementType>> node;
            if (it2 != nodes.end())
            {
                node = (*it2);
            }
            else
            {
                node = std::make_shared<AStarNode<ElementType>>();
                node->setG(it->getDistance(endNode->getElement()));
                node->setElement(it);
                nodes.emplace_back(node);
            }
            // Ignore already-closed nodes
            if (node->getNodeState() == AStarNodeState::Closed)
            {
                continue;
            }
            // Already-open nodes are only added to the list if their G-value is lower going via this route.
            if (node->getNodeState() == AStarNodeState::Open)
            {
                auto traversalCost = fromNode->getElement()->getDistance(it);
                int gTemp = fromNode->getG() + traversalCost;
                if (gTemp < node->getG())
                {
                    node->setParentNode(fromNode);
                    node->setG(fromNode->getG() + traversalCost);
                    neighbours.push_back(node);
                }
            }
            else
            {
                // If it's untested, set the parent and flag it as 'Open' for consideration
                node->setNodeState(AStarNodeState::Open);
                node->setParentNode(fromNode);
                node->setG(fromNode->getG() + fromNode->getElement()->getDistance(it));
                neighbours.push_back(node);
            }
        }
        return neighbours;
    }
};
