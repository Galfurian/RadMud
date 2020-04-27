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

#include "utilities/utils.hpp"

#include <string>
#include <climits>

/// @brief Allows to easily manage input arguments from players.
class Argument {
private:
	/// The original argument string.
	std::string original;
	/// The string with both the index and the quantity removed.
	std::string content;
	/// The provided index.
	unsigned int index;
	/// The provided quantity.
	unsigned int quantity;
	/// Struct containing all the flags associated with this argument.
	struct prefix_t {
		/// The 'all.' prefix was specified.
		bool all;
		/// The '<quantity>*' postfix was specified.
		bool quantity;
		/// The '<index>.' postfix was specified.
		bool index;
		/// @brief Constructor.
		prefix_t(bool _all, bool _quantity, bool _index) :
			all(_all),
			quantity(_quantity),
			index(_index) { }
	} prefix;

public:
	/// @brief Constructor.
	explicit Argument(const std::string& _original) :
		original(_original),
		content(_original),
		index(1),
		quantity(1),
		prefix(false, false, false)
	{
		// First, evaluate the quantity.
		this->evaluateQuantity();
		// Then, evaluate the index.
		this->evaluateIndex();
	}

	/// Provides the original argument.
	inline std::string getOriginal() const
	{
		return original;
	}

	/// Provides the content with both index and quantity removed.
	inline std::string getContent() const
	{
		return content;
	}

	/// Provides the index.
	inline unsigned int getIndex() const
	{
		return index;
	}

	/// Provides the quantity.
	inline unsigned int getQuantity() const
	{
		return quantity;
	}

	/// @brief Checks if there is only one prefix, or there is no prefix.
	inline bool hasOnlyOnePrefix() const
	{
		return ((prefix.all + prefix.quantity + prefix.index) == 1) ||
			   !(prefix.all & prefix.quantity & prefix.index);
	}

	/// Checks if the prefix "all." was specified.
	inline bool hasPrefixAll() const
	{
		return prefix.all;
	}

	/// Checks if the prefix "<quantity>*" was specified.
	inline bool hasQuantity() const
	{
		return prefix.quantity;
	}

	/// Checks if the prefix "<index>." was specified.
	inline bool hasIndex() const
	{
		return prefix.index;
	}

	/// Checks if the prefix "all." was present.
	inline bool meansAll() const
	{
		std::string lower_original = ToLower(original);
		return lower_original == "all" || lower_original == "tutto";
	}

	/// Sets the content of the argument.
	inline void setString(std::string const& s)
	{
		original = content = s;
		// First, evaluate the quantity.
		this->evaluateQuantity();
		// Then, evaluate the index.
		this->evaluateIndex();
	}

	template<typename T>
	inline T toNumber() const
	{
		return ToNumber<T>(content);
	}

	inline bool isNumber() const
	{
		return IsNumber(content);
	}

private:
	/// Evaluates the index.
	void evaluateIndex();

	/// Evaluates the quantity.
	void evaluateQuantity();
};

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
	explicit ArgumentHandler(const char* _argument, bool skip_fill_words = false) :
		original(),
		arguments()
	{
		if (_argument != NULL) {
			if (_argument[0] != 0) {
				original = std::string(_argument);
				// First, evaluate the arguments.
				this->evaluateArguments(skip_fill_words);
			}
		}
	}

	/// @brief Constructor.
	explicit ArgumentHandler(std::string const& _argument, bool skip_fill_words = false) :
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
	~ArgumentHandler()
	{
		// Nothing to do.
	}

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

	const Argument* find(std::string const& s, bool exact = false);

	/// Allows to retrieve the argument at the given position.
	Argument& get(const size_t& position)
	{
		return arguments.at(position);
	}

	/// Allows to retrieve the argument at the given position.
	Argument& operator[](const size_t& position)
	{
		if (position >= arguments.size()) {
			static Argument empty("");
			return empty;
		}
		return arguments[position];
	}

	/// Allows to retrieve const reference to argument at given position.
	Argument const& operator[](const size_t& position) const
	{
		if (position >= arguments.size()) {
			static Argument empty("");
			return empty;
		}
		return arguments[position];
	}

	/// Returns the string from the given argument to the end.
	std::string substr(size_t _start, size_t _end = std::string::npos) const;

	/// Erase the argument at the given position.
	void erase(const size_t& position);

	/// @brief Prints a log of all the contained arguments.
	void dump() const;

private:
	/// @brief Checks if the given argument is a fill word.
	bool isFillWord(const std::string& argument);

	/// Given that original string has been set, fills the vector of arguments.
	void evaluateArguments(bool skip_fill_words = false);
};
