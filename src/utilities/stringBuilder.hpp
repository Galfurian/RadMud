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

#include <sstream>

inline std::string BuildStr(std::string const &format)
{
	return format;
}

inline std::string BuildStr(std::stringstream &ss, size_t cpos,
							std::string const &format)
{
	ss << format.substr(cpos);
	return ss.str();
}

template <typename T, typename... Targs>
inline std::string BuildStr(std::stringstream &ss, size_t cpos,
							std::string const &format, T value, Targs... args)
{
	std::string::size_type pos = format.find("%s", cpos);
	if (pos == std::string::npos) {
		return BuildStr(ss, cpos, format);
	} else {
		ss << format.substr(cpos, pos - cpos);
		ss << value;
		return BuildStr(ss, pos + 2, format, args...);
	}
}

template <typename T, typename... Targs>
inline std::string BuildStr(std::string const &format, T value, Targs... args)
{
	std::stringstream ss;
	return BuildStr(ss, 0, format, value, args...);
}
