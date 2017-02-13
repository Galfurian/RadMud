/// @file   argument.cpp
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

#include <climits>
#include "argumentHandler.hpp"

#include "logger.hpp"

Argument::Argument(const std::string & _original) :
    original(_original),
    content(_original),
    index(1),
    multiplier(1)
{
    // First, evaluate the multiplier.
    this->evaluateMultiplier();
    // Then, evaluate the index.
    this->evaluateIndex();
    //Logger::log(LogLevel::Debug, "Content    :" + content + ";");
    //Logger::log(LogLevel::Debug, "Index      :" + ToString(index) + ";");
    //Logger::log(LogLevel::Debug, "Multiplier :" + ToString(multiplier) + ";");
}

std::string Argument::getOriginal() const
{
    return original;
}

std::string Argument::getContent() const
{
    return content;
}

int & Argument::getIndex()
{
    return index;
}

unsigned int Argument::getMultiplier() const
{
    return multiplier;
}

void Argument::evaluateIndex()
{
    // If the entire string is a number, set it.
    if (!IsNumber(content))
    {
        // Otherwise try to find a number if there is one.
        auto pos = content.find('.');
        if (pos != std::string::npos)
        {
            // Extract the digits.
            auto digits = content.substr(0, pos);
            // Check the digits.
            if (IsNumber(digits))
            {
                // Get the number.
                auto number = atol(digits.c_str());
                if (number < INT_MAX)
                {
                    // Set the number.
                    index = static_cast<int>(number);
                }
                // Remove the digits.
                content = content.substr(pos + 1, content.size());
            }
        }
    }
}

void Argument::evaluateMultiplier()
{
    // If the entire string is a number, set it.
    if (!IsNumber(content))
    {
        // Otherwise try to find a number if there is one.
        auto pos = content.find('*');
        if (pos != std::string::npos)
        {
            // Extract the digits.
            auto digits = content.substr(0, pos);
            // Check the digits.
            if (IsNumber(digits))
            {
                // Get the number.
                auto number = atol(digits.c_str());
                if (number < INT_MAX)
                {
                    // Set the number.
                    multiplier = static_cast<unsigned int>(number);
                }
                // Remove the digits.
                content = content.substr(pos + 1, content.size());
            }
        }
    }
}
