/// @file   pathFinderNode.hpp
/// @author Enrico Fraccaroli
/// @date   Feb 11 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#pragma once

/// @brief A supporting node for all the pathfinding algorithms.
template<typename T>
class PathFinderNode
{
private:
    /// Wrapped element.
    T element;

public:
    /// @brief Constructor.
    PathFinderNode(T _element) :
        element(_element)
    {
        // Nothing to do.
    }

    /// @brief Allows to set the wrapped element.
    /// @param _element The element to set.
    void setElement(T _element)
    {
        element = _element;
    }

    /// @brief Provides the wrapped element.
    T getElement()
    {
        return element;
    }
};
