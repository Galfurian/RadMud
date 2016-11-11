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
    // Create the starting and end nodes.
    startNode = std::make_shared<AStarNode<ElementType>>(start);
    endNode = std::make_shared<AStarNode<ElementType>>(end);
    // Add the starting and ending nodes to the list of nodes.
    nodes.emplace_back(startNode);
    nodes.emplace_back(endNode);
    // Rise the endNode flag for the end node.
    endNode->setIsEndNode();
    // Set the G-Value for the starting node.
    startNode->setG(startNode->getDistance(endNode));
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
bool AStar<ElementType>::search(std::shared_ptr<AStarNode<ElementType>> currentNode)
{
    // Set the current node to Closed since it cannot be traversed more than once
    currentNode->setNodeState(AStarNodeState::Closed);
    // Get the next nodes.
    auto neighbours = currentNode->getNeighbours(nodes, checkFunction);
    // Reorder the neighbours so that the one with the lesser G-Value comes first.
    std::sort(neighbours.begin(), neighbours.end(), [](const std::shared_ptr<AStarNode<ElementType>> & left,
                                                       const std::shared_ptr<AStarNode<ElementType>> & right)
    {
        return left->getF() < right->getF();
    });
    for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        // Get the neighbour at the given iterator.
        auto neighbour = (*it);
        Logger::log(LogLevel::Debug, "        Neighbour->F:%s", ToString(neighbour->getF()));
    }
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