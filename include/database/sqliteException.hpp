/// @file   sqliteException.hpp
/// @brief  Declaration of SQLiteException class.
/// @author Enrico Fraccaroli
/// @date   09/12/2016
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

#include <stdexcept>

/// @brief A custom exception handler for the loading function used by the mud.
class SQLiteException : public std::runtime_error {
private:
	/// The error message associated with the exception.
	std::string errorMessage;

public:
	/// @brief Constructor.
	/// @param _errorMessage The error message.
	explicit SQLiteException(std::string const &_message);

	/// @brief Constructor.
	/// @param _errorCode    The SQLITE error code.
	/// @param _errorMessage The error message.
	SQLiteException(int _code, std::string const &_message);

	/// @brief Return what is the subject of the exception.
	char const *what() const noexcept override;
};
