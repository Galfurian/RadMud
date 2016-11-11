/// @file   argument.hpp
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

#include <string>

/// @brief Allows to easely manage input arguments from players.
class Argument
{
public:
    /// The original argument string.
    std::string original;
    /// The string with both the index and the multiplier removed.
    std::string content;
    /// The provided index.
    int index;
    /// The provided multiplier.
    unsigned int multiplier;

public:
    /// @brief Constructor.
    Argument(const std::string & _original);

    /// Provides the original argument.
    std::string getOriginal() const;

    /// Provides the content with both index and multiplier removed.
    std::string getContent() const;

    /// Provides the index.
    int & getIndex();

    /// Provides the multiplier.
    unsigned int getMultiplier() const;

private:
    /// Evaluates the index.
    void evaluateIndex();

    /// Evaluates the multiplier.
    void evaluateMultiplier();
};
