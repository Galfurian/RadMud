//
// Created by enrico on 11/11/16.
//

template<typename ElementType>
AStarNode<ElementType>::AStarNode() :
    element(),
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