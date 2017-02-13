/// @file   stringBuilder.hpp
/// @author Enrico Fraccaroli
/// @date   Feb 11 2017
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

#include <type_traits>
#include <sstream>

/// @brief Allows to build a string from a series of arguments.
class StringBuilder
{
private:
    StringBuilder()
    {
        // Nothing to do.
    }

public:
    /// @brief Replaces all the occurrences of the pattern '%s' with
    ///         the arguments.
    /// @param str The string that has to be built.
    /// @return The customized string.
    static std::string build(const std::string & str)
    {
        return str;
    }

    /// @brief Replaces all the occurrences of the pattern '%s' with
    ///         the arguments.
    /// @param str   The string that has to be built.
    /// @param first The first unpacked argument.
    /// @param args  Packed arguments.
    template<typename ... Args>
    static std::string build(const std::string & str,
                             const std::string & first,
                             const Args & ... args)
    {
        std::string::size_type pos = str.find("%s");
        if (pos == std::string::npos)
        {
            return str;
        }
        else
        {
            std::string working = str;
            working.replace(pos, 2, first);
            return build(working, args ...);
        }
    }

    /// @brief Cover the case in which the first argument is a signed value.
    template<
        typename T,
        typename ... Args,
        typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
    static std::string build(const std::string & str,
                             const T & first,
                             const Args & ... args)
    {
        std::stringstream ss;
        ss << first;
        return build(str, ss.str(), args ...);
    }

};