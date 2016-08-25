/*
 * argumentHandler.cpp
 *
 *  Created on: Aug 25, 2016
 *      Author: enrico
 */

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
