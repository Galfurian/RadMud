/// @file   argumentHandler.cpp
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

#include "argumentHandler.hpp"
#include "../utils.hpp"

ArgumentHandler::ArgumentHandler(const std::string & _original) :
        original(_original),
        arguments()
{
    // First, evaluate the arguments.
    this->evaluateArguments();
}

ArgumentHandler::ArgumentHandler(std::istream & _original) :
        original(),
        arguments()
{
    // First, get the content.
    std::getline(_original, original);
    // First, evaluate the arguments.
    this->evaluateArguments();
}

ArgumentHandler::~ArgumentHandler()
{
    // Nothing to do.
}

void ArgumentHandler::evaluateArguments()
{
    auto words = GetWords(original);
    for (auto it : words)
    {
        arguments.push_back(Argument(it));
    }
}

std::string ArgumentHandler::getOriginal()
{
    return original;
}

size_t ArgumentHandler::size() const
{
    return arguments.size();
}

bool ArgumentHandler::empty() const
{
    return arguments.empty();
}

Argument & ArgumentHandler::get(const size_t & position)
{
    return arguments.at(position);
}

Argument & ArgumentHandler::operator[](const size_t & position)
{
    return arguments[position];
}

std::string ArgumentHandler::substr(const size_t & startingArgument)
{
    if (startingArgument >= this->size())
    {
        return original;
    }
    else
    {
        size_t pos = 0;
        for (size_t it = 0; it < startingArgument; ++it)
        {
            pos += arguments[it].getOriginal().size() + 1;
        }
        return original.substr(pos);
    }
}

void ArgumentHandler::erase(const size_t & position)
{
    if (position < arguments.size())
    {
        auto it = arguments.begin();
        std::advance(it, position);
        arguments.erase(it);
    }
}
