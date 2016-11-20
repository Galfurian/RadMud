/// @file   aStarNode.i.hpp
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

//template<>
//bool AStarNode<Room *>::isEqualTo(std::shared_ptr<AStarNode<Room *>> other);
//
//template<>
//int AStarNode<Room *>::getDistance(std::shared_ptr<AStarNode<Room *>> other);
//
//template<>
//std::vector<std::shared_ptr<AStarNode<Room *>>> AStarNode<Room *>::getNeighbours(
//    std::vector<std::shared_ptr<AStarNode<Room *>>> & nodes,
//    std::shared_ptr<AStarNode<Room *>> endNode,
//    const std::function<bool(Room * from, Room * to)> & checkFunction);
