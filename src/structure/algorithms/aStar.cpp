//
// Created by enrico on 11/10/16.
//

#include "aStar.hpp"
#include "area.hpp"
/*
std::vector<Coordinates> AStar::findPath(Room * startRoom, Room * endRoom)
{
    start = startRoom->coord;
    end = endRoom->coord;
    // The start node is the first entry in the 'open' list
    std::vector<Coordinates> path;
    // Create the start and end locations.
    nodes[start] = std::make_shared<AStarNode<Room *>>();
    nodes[start]->setElement(startRoom);
    nodes[start]->setG(static_cast<float>(Area::getDistance(start, end)));
    Logger::log(LogLevel::Debug, "Searching...");
    if (this->search(nodes[start]))
    {
        Logger::log(LogLevel::Debug, "Found!");
        // If a path was found, follow the parents from the end node to build a list of locations.
        auto node = nodes[end];
        while (node->getParentNode() != nullptr)
        {
            path.emplace_back(node->getElement()->coord);
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

bool AStar::search(std::shared_ptr<AStarNode<Room *>> currentNode)
{
    // Set the current node to Closed since it cannot be traversed more than once
    currentNode->setNodeState(AStarNodeState::Closed);
    // Get the next nodes.
    auto neighbours = this->getNeighbours(currentNode);
    std::sort(neighbours.begin(), neighbours.end(), [](const std::shared_ptr<AStarNode<Room *>> & left,
                                                       const std::shared_ptr<AStarNode<Room *>> & right)
    {
        if (left->getF() < right->getF()) return true;
        if (left->getF() > right->getF()) return false;
        return false;
    });
    for (auto nextNode : neighbours)
    {
        // Check whether the end node has been reached
        if (nextNode->getElement()->coord == end)
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

std::vector<std::shared_ptr<AStarNode<Room *>>> AStar::getNeighbours(std::shared_ptr<AStarNode<Room *>> fromNode)
{
    // Retrieve the area.
    Area * area = fromNode->getElement()->area;
    std::vector<std::shared_ptr<AStarNode<Room *>>> neighbours;
    for (auto it : fromNode->getElement()->getAvailableDirections())
    {
        // Evaluate the coordinates.
        Coordinates coordinates = fromNode->getElement()->coord + it.getCoordinates();
        // Ignore non-walkable nodes.
        if (!area->isValid(coordinates))
        {
            continue;
        }
        // Retrieve the room at the given coordinates.
        Room * room = area->getRoom(coordinates);
        // First check if there is already an AStarNode<Room *> at the given coordinates.
        auto it2 = nodes.find(coordinates);
        std::shared_ptr<AStarNode<Room *>> aStarNode;
        if (it2 != nodes.end())
        {
            aStarNode = it2->second;
        }
        else
        {
            aStarNode = nodes[coordinates] = std::make_shared<AStarNode<Room *>>();
            aStarNode->setElement(room);
            aStarNode->setG(static_cast<float>(Area::getDistance(room->coord, end)));
        }
        // Ignore already-closed nodes
        if (aStarNode->getNodeState() == AStarNodeState::Closed)
        {
            continue;
        }
        // Already-open nodes are only added to the list if their G-value is lower going via this route.
        if (aStarNode->getNodeState() == AStarNodeState::Open)
        {
            auto traversalCost = Area::getDistance(aStarNode->getElement()->coord, coordinates);
            float gTemp = fromNode->getG() + static_cast<float>(traversalCost);
            if (gTemp < aStarNode->getG())
            {
                aStarNode->setParentNode(fromNode);
                aStarNode->setG(fromNode->getG() +
                                static_cast<float>(Area::getDistance(room->coord,
                                                                     fromNode->getElement()->coord)));
                neighbours.push_back(aStarNode);
            }
        }
        else
        {
            // If it's untested, set the parent and flag it as 'Open' for consideration
            aStarNode->setParentNode(fromNode);
            aStarNode->setG(fromNode->getG() +
                            static_cast<float>(Area::getDistance(room->coord,
                                                                 fromNode->getElement()->coord)));
            aStarNode->setNodeState(AStarNodeState::Open);
            neighbours.push_back(aStarNode);
        }
    }
    return neighbours;
}
*/