//
// Created by enrico on 11/11/16.
//

template<typename ElementType>
AStarNode<ElementType>::AStarNode(ElementType _element) :
    element(_element),
    nodeState(),
    g(),
    h(),
    parentNode(),
    endNode()
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
    endNode = true;
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
    return endNode;
}

template<>
int AStarNode<Room *>::getDistance(std::shared_ptr<AStarNode<Room *>> other)
{
    auto distance = element->area->getDistance(element->coord, other->getElement()->coord);
    return distance;
}

template<>
bool AStarNode<Room *>::isEqualTo(std::shared_ptr<AStarNode<Room *>> other)
{
    if (other == nullptr) return false;
    return (this->element->vnum == other->getElement()->vnum);
}

template<>
std::vector<std::shared_ptr<AStarNode<Room *>>> AStarNode<Room *>::getNeighbours(
    std::vector<std::shared_ptr<AStarNode<Room *>>> & nodes,
    const std::function<bool(Room * from, Room * to)> & checkFunction)
{
    Logger::log(LogLevel::Debug, "Node : %s", element->name);
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
                Logger::log(LogLevel::Debug, "    Taking   [%s]: %s", it2->getF(), it->destination->name);
                break;
            }
        }
        if (!found)
        {
            auto node = std::make_shared<AStarNode<Room *>>(it->destination);
            node->setG(g + this->getDistance(node));
            nodes.emplace_back(node);
            neighbours.emplace_back(node);
            Logger::log(LogLevel::Debug, "    Taking   [%s]: %s", node->getF(), it->destination->name);
        }
    }
    return neighbours;
}
