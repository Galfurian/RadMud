//
// Created by enrico on 11/11/16.
//

#pragma once

#include <vector>
#include <functional>

template<typename ElementType>
class PathFinder
{
public:
    virtual bool findPath(ElementType start,
                          ElementType end,
                          std::vector<ElementType> & path,
                          const std::function<bool(ElementType from, ElementType to)> & _checkFunction) = 0;
};