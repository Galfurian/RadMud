/// @file   argumentHandler.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 25, 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include "argument.hpp"

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

private:
    /// Given that original string has been set, it fills the vector of arguments.
    void evaluateArguments();
};
