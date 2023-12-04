/// @file   baseEnumerator.hpp
/// @author Enrico Fraccaroli
/// @date   Feb 14 2017
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

#include <string>

/// @brief A base class used to implement complex enumerators.
class BaseEnumerator
{
protected:
    /// Internal value.
    unsigned int value;

public:

    /// @brief Constructor.
    BaseEnumerator();

    /// @brief Constructor from unsigned int.
    BaseEnumerator(const unsigned int & _value);

    /// @brief Constructor from uint.
    virtual ~BaseEnumerator();

    /// @brief Returns the value as string.
    virtual std::string toString() const;

    /// @brief Returns the value as number.
    unsigned int toUInt() const
    {
        return value;
    }

    /// @brief Returns the value as number.
    template<typename T>
    T to() const
    {
        return static_cast<T>(value);
    }

    /// @brief Equality operator.
    bool operator==(const BaseEnumerator & rhs) const
    {
        return value == rhs.value;
    }

    /// @brief Inequality operator.
    bool operator!=(const BaseEnumerator & rhs) const
    {
        return value != rhs.value;
    }

    /// @brief Lesser operator.
    bool operator<(const BaseEnumerator & rhs) const
    {
        return value < rhs.value;
    }

    /// @brief Greater operator.
    bool operator>(const BaseEnumerator & rhs) const
    {
        return value > rhs.value;
    }

    /// @brief Lesser-Equal operator.
    bool operator<=(const BaseEnumerator & rhs) const
    {
        return value <= rhs.value;
    }

    /// @brief Greater-Equal operator.
    bool operator>=(const BaseEnumerator & rhs) const
    {
        return value >= rhs.value;
    }
};

/// @brief Operator which allows to print the base enumerator in a stream.
std::ostream & operator<<(std::ostream & os, const BaseEnumerator & enumerator);