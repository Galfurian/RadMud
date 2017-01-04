/// @file   aStar.i.hpp
/// @brief  Contains the imlementation of the class for the astar pathfinder.
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
AStar<ElementType>::AStar() :
    nodes(),
    startNode(),
    endNode()
{
    // Nothing to do.
}

template<typename ElementType>
bool AStar<ElementType>::findPath(
    ElementType start,
    ElementType end,
    std::vector<ElementType> & path,
    const std::function<bool(ElementType from,
                             ElementType to)> & _checkFunction)
{
    // Create the starting and end nodes.
    startNode = std::make_shared<AStarNode<ElementType >>(start);
    endNode = std::make_shared<AStarNode<ElementType >>(end);
    // Add the starting and ending nodes to the list of nodes.
    nodes.emplace_back(startNode);
    nodes.emplace_back(endNode);
    // Rise the endNode flag for the end node.
    endNode->setIsEndNode();
    // Set the G-Value and H-Value for the starting node.
    startNode->setG(0);
    startNode->setH(startNode->getDistance(endNode));
    // Set the checking function.
    checkFunction = _checkFunction;
    // Start the search.
    if (this->search(startNode))
    {
        // If a path was found, follow the parents from the end node to build a list of locations.
        auto node = endNode;
        while (node->getParentNode() != nullptr)
        {
            path.emplace_back(node->element);
            node = node->getParentNode();
        }
        std::reverse(path.begin(), path.end());
        return true;
    }
    return false;
}

template<typename ElementType>
bool AStar<ElementType>::search(
    std::shared_ptr<AStarNode<ElementType>> currentNode)
{
    // Set the current node to Closed since it cannot be traversed more than once
    currentNode->setNodeState(AStarNodeState::Closed);
    // Get the next nodes.
    auto neighbours = currentNode->getNeighbours(nodes, endNode, checkFunction);
    // Reorder the neighbours so that the one with the lesser G-Value comes first.
    std::sort(neighbours.begin(),
              neighbours.end(),
              [](const std::shared_ptr<AStarNode<ElementType>> & left,
                 const std::shared_ptr<AStarNode<ElementType>> & right)
              {
                  return left->getF() < right->getF();
              });
    // Continue the search for each neighbour.
    for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        // Get the neighbour at the given iterator.
        auto neighbour = (*it);
        // Ignore already-closed nodes
        if (neighbour->getNodeState() == AStarNodeState::Closed) continue;
        // Evaluate the G-value for the neighbour.
        int gTemp = currentNode->getG() + currentNode->getDistance(neighbour);
        // Already-open nodes are only added to the list if their G-value is lower going via this route.
        if (neighbour->getNodeState() == AStarNodeState::Open)
        {
            if (gTemp < neighbour->getG())
            {
                // Change the parent of the neighbour node.
                neighbour->setParentNode(currentNode);
                // Set the new G-Value.
                neighbour->setG(gTemp);
                // Check whether the end node has been reached.
                if (endNode->isEqualTo(neighbour)) return true;
                // If not, check the next set of nodes.
                if (this->search(neighbour)) return true;
            }
        }
        else
        {
            // Change the state of the node as Open.
            neighbour->setNodeState(AStarNodeState::Open);
            // Change the parent of the neighbour node.
            neighbour->setParentNode(currentNode);
            // Set the new G-Value.
            neighbour->setG(gTemp);
            // Check whether the end node has been reached.
            if (endNode->isEqualTo(neighbour)) return true;
            // If not, check the next set of nodes.
            if (this->search(neighbour)) return true;
        }
    }
    // The method returns false if this path leads to be a dead end.
    return false;
}
