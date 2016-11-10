//
// Created by enrico on 11/10/16.
//

#include "aStarNode.hpp"
#include "area.hpp"

AStarNode::AStarNode() :
    room(),
    nodeState(),
    g(),
    h(),
    parentNode()
{
    // Nothing to do.
}

AStarNode::AStarNode(Room * _room, Coordinates _target) :
    room(_room),
    nodeState(),
    g(),
    h(static_cast<float>(Area::getDistance(_room->coord, _target))),
    parentNode()
{
    // Nothing to do.
}

void AStarNode::setNodeState(const NodeState & _nodeState)
{
    nodeState = _nodeState;
}

void AStarNode::setParentNode(std::shared_ptr<AStarNode> _parentNode)
{
    parentNode = _parentNode;
    g = _parentNode->getG() + static_cast<float>(Area::getDistance(room->coord, _parentNode->room->coord));
}

Room * AStarNode::getRoom()
{
    return room;
}

AStarNode::NodeState AStarNode::getNodeState() const
{
    return nodeState;
}

std::shared_ptr<AStarNode> AStarNode::getParentNode()
{
    return parentNode;
}

float AStarNode::getG() const
{
    return g;
}

float AStarNode::getF() const
{
    return g + h;
}

std::string AStarNode::toString() const
{
    if (nodeState == Open)
    {
        return "[O] " + room->coord.toString();
    }
    if (nodeState == Closed)
    {
        return "[C] " + room->coord.toString();
    }
    return "[U] " + room->coord.toString();
}
