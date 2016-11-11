/// @file   aStarNode.i.hpp
/// @brief  Contains the imlementation of the class of nodes use by the astar pathfinder.
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

#include "room.hpp"
#include "area.hpp"

template<typename ElementType>
AStarNode<ElementType>::AStarNode(ElementType _element) :
    element(_element),
    nodeState(),
    g(),
    h(),
    parentNode(),
    endNodeFlag()
{
    // Nothing to do.
}

template<typename ElementType>
void AStarNode<ElementType>::setElement(ElementType _element)
{
    element = _element;
}

template<typename ElementType>
void AStarNode<ElementType>::setNodeState(const AStarNodeState & _nodeState)
{
    nodeState = _nodeState;
}

template<typename ElementType>
void AStarNode<ElementType>::setG(const int & _g)
{
    g = _g;
}

template<typename ElementType>
void AStarNode<ElementType>::setH(const int & _h)
{
    h = _h;
}

template<typename ElementType>
void AStarNode<ElementType>::setParentNode(std::shared_ptr<AStarNode<ElementType>> _parentNode)
{
    parentNode = _parentNode;
}

template<typename ElementType>
void AStarNode<ElementType>::setIsEndNode()
{
    endNodeFlag = true;
}

template<typename ElementType>
ElementType AStarNode<ElementType>::getElement()
{
    return element;
}

template<typename ElementType>
AStarNodeState AStarNode<ElementType>::getNodeState() const
{
    return nodeState;
}

template<typename ElementType>
int AStarNode<ElementType>::getG() const
{
    return g;
}

template<typename ElementType>
int AStarNode<ElementType>::getF() const
{
    return g + h;
}

template<typename ElementType>
std::shared_ptr<AStarNode<ElementType>> AStarNode<ElementType>::getParentNode()
{
    return parentNode;
}

template<typename ElementType>
bool AStarNode<ElementType>::isEndNode() const
{
    return endNodeFlag;
}

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
