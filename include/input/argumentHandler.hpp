/// @file   argumentHandler.hpp
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

#include "argument.hpp"
#include "utilities/logger.hpp"

#include <vector>

/// @brief Allows to simply handle players inputs.
class ArgumentHandler {
private:
	/// The original string.
	std::string original;
	/// List of arguments.
	std::vector<Argument> arguments;

public:
	typedef std::vector<Argument>::iterator iterator;
	typedef std::vector<Argument>::const_iterator const_iterator;

	/// @brief Constructor.
	explicit ArgumentHandler(const char *_argument,
							 bool skip_fill_words = false) :
		original(),
		arguments()
	{
		if (_argument != nullptr) {
			if (_argument[0] != 0) {
				original = std::string(_argument);
				// First, evaluate the arguments.
				this->evaluateArguments(skip_fill_words);
			}
		}
	}

	/// @brief Constructor.
	explicit ArgumentHandler(std::string const &_argument,
							 bool skip_fill_words = false) :
		original(),
		arguments()
	{
		if (!_argument.empty()) {
			original = _argument;
			// First, evaluate the arguments.
			this->evaluateArguments(skip_fill_words);
		}
	}

	/// @brief Destructor.
	~ArgumentHandler() = default;

	/// Provides the original input string.
	std::string getOriginal() const
	{
		return original;
	}

	/// Recompose the list of arguments to make the sentence.
	std::string getRecombineString() const
	{
		return this->substr(0);
	}

	/// Returns the number of arguments.
	size_t size() const
	{
		return arguments.size();
	}

	/// Checks if the vector of arguments is empty.
	bool empty() const
	{
		return arguments.empty();
	}

	inline iterator begin()
	{
		return arguments.begin();
	}

	inline const_iterator begin() const
	{
		return arguments.begin();
	}

	inline iterator end()
	{
		return arguments.end();
	}

	inline const_iterator end() const
	{
		return arguments.end();
	}

	inline Argument const *find(std::string const &s, bool exact = false)
	{
		for (const_iterator it = arguments.begin(); it != arguments.end();
			 ++it) {
			if (exact) {
				if (it->content == s)
					return &(*it);
			} else {
				if (BeginWith(it->content, s))
					return &(*it);
			}
		}
		return nullptr;
	}

	/// Allows to retrieve the argument at the given position.
	Argument &get(const size_t &position)
	{
		return arguments.at(position);
	}

	/// Allows to retrieve the argument at the given position.
	Argument &operator[](const size_t &position)
	{
		return arguments[position];
	}

	/// Returns the string from the given argument to the end.
	std::string substr(size_t _start, size_t _end = std::string::npos) const
	{
		if (_start >= this->size()) {
			MudLog(LogLevel::Error, "Starting argument out of bound!");
			return original;
		}
		if (_end == std::string::npos)
			_end = arguments.size();
		std::string result;
		for (size_t it = _start; it < _end; ++it) {
			result.append(arguments[it].getContent());
			if (it != (arguments.size() - 1))
				result.push_back(' ');
		}
		return result;
	}

	/// Erase the argument at the given position.
	void erase(const size_t &position)
	{
		if (position < arguments.size()) {
			auto it = arguments.begin();
			std::advance(it, position);
			arguments.erase(it);
		} else {
			MudLog(LogLevel::Error, "Position out of bound!");
		}
	}

	/// @brief Prints a log of all the contained arguments.
	void dump() const
	{
		for (size_t it = 0; it < arguments.size(); ++it) {
			MudLog(LogLevel::Debug, "[%-2d][IDX:%-2d][MUL:%-2d] %s {%s}", it,
				   arguments.at(it).getOriginalIndex(),
				   arguments.at(it).getMultiplier(),
				   arguments.at(it).getContent().c_str(),
				   arguments.at(it).getOriginal().c_str());
		}
	}

private:
	/// @brief Checks if the given argument is a fill word.
	static bool IsFillWord(const std::string &argument)
	{
		static std::string fill_words[] = { "in", "from", "with", "and", "the",
											"on", "at",	  "to",	  "da",	 "su",
											"a",  "e",	  "il",	  "la",	 "lo",
											"un", "uno",  "una",  "con", "\n" };
		std::string argument_lower = ToLower(argument);
		for (int i = 0; fill_words[i] != "\n"; i++)
			if (argument_lower == fill_words[i])
				return true;
		return false;
	}

	/// Given that original string has been set, fills the vector of arguments.
	void evaluateArguments(bool skip_fill_words = false)
	{
		std::vector<std::string> words = SplitString(original, " ");
		for (std::vector<std::string>::const_iterator it = words.begin();
			 it != words.end(); ++it) {
			if (skip_fill_words && IsFillWord(*it))
				continue;
			arguments.emplace_back(Argument(*it));
		}
	}
};
