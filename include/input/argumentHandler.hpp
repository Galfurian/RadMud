/// @file   argumentHandler.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 25, 2016
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

#pragma once

#include "input/argument.hpp"

#include <vector>

/// @brief Allows to simply handle players inputs.
class ArgumentHandler
{
private:
    /// The original string.
    std::string original;
    /// List of arguments.
    std::vector<Argument> arguments;

public:
    /// @brief Constructor.
    ArgumentHandler(const std::string & _original);

    /// @brief Constructor.
    ArgumentHandler(std::istream & _original);

    /// @brief Destructor.
    virtual ~ArgumentHandler();

    /// Provides the original input string.
    std::string getOriginal();

    /// Returns the number of arguments.
    size_t size() const;

    /// Checks if the vector of arguments is empty.
    bool empty() const;

    /// Allows to retrieve the argument at the given position.
    Argument & get(const size_t & position);

    /// Allows to retrieve the argument at the given position.
    Argument & operator[](const size_t & position);

    /// Returns the string from the given argument to the end.
    std::string substr(const size_t & startingArgument);

    /// Erase the argument at the given position.
    void erase(const size_t & position);

    /// @brief Prints a log of all the contained arguments.
    void dump() const;

private:
    /// Given that original string has been set, fills the vector of arguments.
    void evaluateArguments();
};
