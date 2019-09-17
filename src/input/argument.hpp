/// @file   argument.hpp
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

#pragma once

#include <climits>
#include "utils.hpp"

/// @brief Allows to easily manage input arguments from players.
class Argument {
public:
	/// The original argument string.
	std::string original;
	/// The string with both the index and the multiplier removed.
	std::string content;
	/// The provided index.
	int index;
	/// The provided multiplier.
	int multiplier;
	/// The provided index, which can be changed by algorithms.
	int temporary_index;

public:
	/// @brief Constructor.
	explicit Argument(const std::string &_original) :
		original(_original),
		content(_original),
		index(1),
		multiplier(1),
		temporary_index(1)
	{
		// First, evaluate the multiplier.
		this->evaluateMultiplier();
		// Then, evaluate the index.
		this->evaluateIndex();
	}

	/// Provides the original argument.
	inline std::string getOriginal() const
	{
		return original;
	}

	/// Provides the content with both index and multiplier removed.
	inline std::string getContent() const
	{
		return content;
	}

	/// Provides the index.
	inline int getOriginalIndex() const
	{
		return index;
	}

	/// Provides the index.
	inline int &getIndex()
	{
		return temporary_index;
	}

	/// Provides the multiplier.
	inline int getMultiplier() const
	{
		return multiplier;
	}

	/// Sets the content of the argument.
	inline void setString(std::string const &s)
	{
		original = content = s;
		// First, evaluate the multiplier.
		this->evaluateMultiplier();
		// Then, evaluate the index.
		this->evaluateIndex();
	}

private:
	/// Evaluates the index.
	void evaluateIndex()
	{
		// If the entire string is a number, skip it.
		if (IsNumber(content))
			return;
		// Otherwise try to find a number if there is one.
		std::string::size_type pos = content.find('.');
		if (pos == std::string::npos)
			return;
		// Extract the digits.
		std::string digits = content.substr(0, pos);
		// Check the digits.
		if (IsNumber(digits)) {
			// Get the number and set it.
			int number = ToNumber<int>(digits);
			if ((number >= 0) && (number < INT_MAX)) {
				// Set the number.
				temporary_index = index = number;
			}
			// Remove the digits.
			content = content.substr(pos + 1, content.size());
		} else if ((digits == "all") || (digits == "tutto")) {
			multiplier = -1;
			// Remove the quantity.
			content = content.substr(pos + 1, content.size());
		}
	}

	/// Evaluates the multiplier.
	void evaluateMultiplier()
	{
		// If the entire string is a number, skip it.
		if (IsNumber(content))
			return;
		// Otherwise try to find a number if there is one.
		std::string::size_type pos = content.find('*');
		if (pos == std::string::npos)
			return;
		// Extract the digits.
		std::string digits = content.substr(0, pos);
		// Check the digits.
		if (IsNumber(digits)) {
			// Get the number and set it.
			int number = ToNumber<int>(digits);
			if ((number >= 0) && (number < INT_MAX)) {
				multiplier = number;
			}
			// Remove the digits.
			content = content.substr(pos + 1, content.size());
		}
	}
};
