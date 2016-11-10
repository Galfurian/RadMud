//
// Created by enrico on 11/10/16.
//

#include "aStarNode.hpp"
#include "area.hpp"

AStarNode::AStarNode() :
    room(),
    state(),
    g(),
    h(),
    parentNode()
{
    // Nothing to do.
}

AStarNode::AStarNode(Room * _room, Coordinates _target) :
    room(_room),
    state(),
    g(),
    h(static_cast<float>(Area::getDistance(_room->coord, _target))),
    parentNode()
{
    // Nothing to do.
}

float AStarNode::getF() const
{
    return g + h;
}

std::string AStarNode::toString() const
{
    if (state == Open)
    {
        return "[O] " + room->coord.toString();
    }
    if (state == Closed)
    {
        return "[C] " + room->coord.toString();
    }
    return "[U] " + room->coord.toString();
}