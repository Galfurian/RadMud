//
// Created by enrico on 11/10/16.
//

#include "aStar.hpp"
#include "area.hpp"

std::vector<Coordinates> PathFinder::findPath(Room * startRoom, Room * endRoom)
{
    start = startRoom->coord;
    end = endRoom->coord;
    // The start node is the first entry in the 'open' list
    std::vector<Coordinates> path;
    // Create the start and end locations.
    nodes[start] = std::make_shared<AStarNode>(startRoom, end);
    Logger::log(LogLevel::Debug, "Searching...");
    if (this->search(nodes[start]))
    {
        Logger::log(LogLevel::Debug, "Found!");
        // If a path was found, follow the parents from the end node to build a list of locations.
        auto node = nodes[end];
        while (node->getParentNode() != nullptr)
        {
            path.emplace_back(node->getRoom()->coord);
            node = node->getParentNode();
        }
        std::reverse(path.begin(), path.end());
    }
    else
    {
        Logger::log(LogLevel::Debug, "No path.");
    }
    return path;
}

bool PathFinder::search(std::shared_ptr<AStarNode> currentNode)
{
    Logger::log(LogLevel::Debug, "Current Node : %s", currentNode->toString());
    // Set the current node to Closed since it cannot be traversed more than once
    currentNode->setNodeState(AStarNode::Closed);
    // Get the next nodes.
    auto neighbours = this->getNeighbours(currentNode);
    std::sort(neighbours.begin(), neighbours.end(), [](const std::shared_ptr<AStarNode> & left,
                                                       const std::shared_ptr<AStarNode> & right)
    {
        return left->getF() < right->getF();
    });
    for (auto nextNode : neighbours)
    {
        Logger::log(LogLevel::Debug, "    Next Node : %s", nextNode->toString());
        // Check whether the end node has been reached
        if (nextNode->getRoom()->coord == end)
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

std::vector<std::shared_ptr<AStarNode>> PathFinder::getNeighbours(std::shared_ptr<AStarNode> fromNode)
{
    // Retrieve the area.
    Area * area = fromNode->getRoom()->area;
    std::vector<std::shared_ptr<AStarNode>> neighbours;
    for (auto it : fromNode->getRoom()->getAvailableDirections())
    {
        // Evaluate the coordinates.
        Coordinates coordinates = fromNode->getRoom()->coord + it.getCoordinates();
        // Ignore non-walkable nodes.
        if (!area->isValid(coordinates))
        {
            continue;
        }
        // Retrieve the room at the given coordinates.
        Room * room = area->getRoom(coordinates);
        // First check if there is already an AStarNode at the given coordinates.
        auto it2 = nodes.find(coordinates);
        std::shared_ptr<AStarNode> aStarNode;
        if (it2 != nodes.end())
        {
            aStarNode = it2->second;
        }
        else
        {
            aStarNode = nodes[coordinates] = std::make_shared<AStarNode>(room, end);
        }
        // Ignore already-closed nodes
        if (aStarNode->getNodeState() == AStarNode::Closed)
        {
            continue;
        }
        // Already-open nodes are only added to the list if their G-value is lower going via this route.
        if (aStarNode->getNodeState() == AStarNode::Open)
        {
            auto traversalCost = Area::getDistance(aStarNode->getRoom()->coord, coordinates);
            float gTemp = fromNode->getG() + static_cast<float>(traversalCost);
            if (gTemp < aStarNode->getG())
            {
                aStarNode->setParentNode(fromNode);
                neighbours.push_back(aStarNode);
            }
        }
        else
        {
            // If it's untested, set the parent and flag it as 'Open' for consideration
            aStarNode->setParentNode(fromNode);
            aStarNode->setNodeState(AStarNode::Open);
            neighbours.push_back(aStarNode);
        }
    }
    return neighbours;
}