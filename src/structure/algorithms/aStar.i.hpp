//
// Created by enrico on 11/11/16.
//

template<typename ElementType>
AStar<ElementType>::AStar() :
    nodes(),
    startNode(),
    endNode()
{
    // Nothing to do.
}

template<typename ElementType>
bool AStar<ElementType>::findPath(ElementType start,
                                  ElementType end,
                                  std::vector<ElementType> & path,
                                  const std::function<bool(ElementType from, ElementType to)> & _checkFunction)
{
    startNode = std::make_shared<AStarNode<ElementType>>(start);
    endNode = std::make_shared<AStarNode<ElementType>>(end);

    nodes.emplace_back(startNode);
    nodes.emplace_back(endNode);

    endNode->setIsEndNode();

    startNode->setG(startNode->getDistance(endNode));

    checkFunction = _checkFunction;

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
    }
    else
    {
        return false;
    }
    return true;
}

template<typename ElementType>
bool AStar<ElementType>::search(std::shared_ptr<AStarNode<ElementType>> currentNode)
{
    // Set the current node to Closed since it cannot be traversed more than once
    currentNode->setNodeState(AStarNodeState::Closed);
    // Get the next nodes.
    std::vector<std::shared_ptr<AStarNode<ElementType>>> neighbours = this->getNeighbours(currentNode);
    std::sort(neighbours.begin(), neighbours.end(), [](const std::shared_ptr<AStarNode<ElementType>> & left,
                                                       const std::shared_ptr<AStarNode<ElementType>> & right)
    {
        if (left->getF() < right->getF()) return true;
        if (left->getF() > right->getF()) return false;
        return false;
    });
    for (auto nextNode : neighbours)
    {
        // Check whether the end node has been reached
        if (endNode->isEqualTo(nextNode))
        {
            return true;
        }
        else
        {
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

template<typename ElementType>
std::vector<std::shared_ptr<AStarNode<ElementType>>>
AStar<ElementType>::getNeighbours(std::shared_ptr<AStarNode<ElementType>> fromNode)
{
    std::vector<std::shared_ptr<AStarNode<ElementType>>> neighbours;
    std::vector<std::shared_ptr<AStarNode<ElementType>>> possibleNeighbours = fromNode->getNeighbours(nodes,
                                                                                                      checkFunction);
    for (auto it = possibleNeighbours.begin(); it != possibleNeighbours.end(); ++it)
    {
        auto neighbour = (*it);
        // Ignore already-closed nodes
        if (neighbour->getNodeState() == AStarNodeState::Closed)
        {
            continue;
        }
        auto traversalCost = fromNode->getDistance(neighbour);
        // Already-open nodes are only added to the list if their G-value is lower going via this route.
        if (neighbour->getNodeState() == AStarNodeState::Open)
        {
            int gTemp = fromNode->getG() + traversalCost;
            if (gTemp < neighbour->getG())
            {
                neighbour->setParentNode(fromNode);
                neighbour->setG(fromNode->getG() + traversalCost);
                neighbours.push_back(*it);
            }
        }
        else
        {
            // If it's untested, set the parent and flag it as 'Open' for consideration
            neighbour->setNodeState(AStarNodeState::Open);
            neighbour->setParentNode(fromNode);
            neighbour->setG(fromNode->getG() + traversalCost);
            neighbours.push_back(*it);
        }
    }
    return neighbours;
}