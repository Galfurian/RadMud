/// @file   argument.cpp
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

#include "../utilities/logger.hpp"
#include "../utils.hpp"

#include <limits.h>

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
    //Logger::log(LogLevel::Debug, "Content    : '" + content + "';");
    //Logger::log(LogLevel::Debug, "Index      : '" + ToString(index) + "';");
    //Logger::log(LogLevel::Debug, "Multiplier : '" + ToString(multiplier) + "';");
}

Argument::~Argument()
{
    // Nothing to do.
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
