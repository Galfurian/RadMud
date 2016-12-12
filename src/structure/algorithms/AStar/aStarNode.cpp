/// @file   aStarNode.cpp
/// @brief  Contains the imlementation of the class of nodes use by the astar pathfinder.
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

#include "aStarNode.hpp"
#include "room.hpp"
#include "area.hpp"

template<>
bool AStarNode<Room *>::isEqualTo(std::shared_ptr<AStarNode<Room *>> other)
{
    if (other == nullptr) return false;
    return (this->element->vnum == other->getElement()->vnum);
}

template<>
int AStarNode<Room *>::getDistance(std::shared_ptr<AStarNode<Room *>> other)
{
    auto distance = element->area->getDistance(element->coord, other->getElement()->coord);
    return distance;
}

template<>
std::vector<std::shared_ptr<AStarNode<Room *>>> AStarNode<Room *>::getNeighbours(
    std::vector<std::shared_ptr<AStarNode<Room *>>> & nodes,
    std::shared_ptr<AStarNode<Room *>> endNode,
    const std::function<bool(Room * from, Room * to)> & checkFunction)
{
    std::vector<std::shared_ptr<AStarNode<Room *>>> neighbours;
    for (auto it : element->exits)
    {
        if (checkFunction)
        {
            if (!checkFunction(element, it->destination)) continue;
        }
        bool found = false;
        for (auto it2 : nodes)
        {
            if (it->destination->vnum == it2->getElement()->vnum)
            {
                neighbours.emplace_back(it2);
                found = true;
                break;
            }
        }
        if (!found)
        {
            auto node = std::make_shared<AStarNode<Room *>>(it->destination);
            node->setG(g + this->getDistance(node));
            node->setH(node->getDistance(endNode));
            nodes.emplace_back(node);
            neighbours.emplace_back(node);
        }
    }
    return neighbours;
}
