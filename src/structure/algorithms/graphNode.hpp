//
// Created by enrico on 11/11/16.
//

#pragma once

#include <vector>
#include <functional>

template<typename ElementType>
class GraphNode
{
public:
    virtual bool isEqualTo(ElementType other) = 0;

    virtual std::vector<ElementType> getNeighbours(const std::function<bool(ElementType from, ElementType to)> & checkFunction) = 0;

    virtual int getDistance(ElementType other) = 0;
};
