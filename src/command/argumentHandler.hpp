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

        std::string getOriginal();

        size_t size() const;

        bool empty() const;

        Argument & get(const size_t & position);

        Argument & operator[](const size_t & position);

        std::string substr(const size_t & startingArgument);

        void erase(const size_t & position);

    private:
        void evaluateArguments();
};
