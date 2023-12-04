/// @file   argumentHandler.cpp
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

#include "input/argumentHandler.hpp"
#include "utilities/utils.hpp"
#include "utilities/logger.hpp"

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
    if (startingArgument < this->size())
    {
        std::string result;
        for (size_t it = startingArgument; it < arguments.size(); ++it)
        {
            result += arguments[it].getContent();
            if (it != (arguments.size() - 1))
            {
                result += " ";
            }
        }
        return result;
    }
    else
    {
        Logger::log(LogLevel::Error, "Starting argument out of bound!");
        return original;
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
    else
    {
        Logger::log(LogLevel::Error, "Position out of bound!");
    }
}

void ArgumentHandler::dump() const
{
    for (size_t it = 0; it < arguments.size(); ++it)
    {
        Logger::log(LogLevel::Debug,
                    "[%s] %s",
                    it,
                    arguments.at(it).getOriginal());
    }
}
