//
// Created by enryf on 27/04/2020.
//

#include "input/argument.hpp"
#include "utilities/logger.hpp"

void Argument::evaluateIndex()
{
	// If the entire string is a number, skip it.
	if (IsNumber(content)) {
		return;
	}
	// Otherwise try to find a number if there is one.
	std::string::size_type pos = content.find('.');
	if (pos == std::string::npos) {
		return;
	}
	// Extract the digits.
	std::string digits = content.substr(0, pos);
	// Check the digits.
	if (IsNumber(digits)) {
		// Get the number and set it.
		int number = ToNumber<int>(digits);
		if ((number >= 0) && (number < INT_MAX)) {
			// Set the number.
			index = static_cast<unsigned int>(number);
			// Set the prefix flag.
			prefix.index = true;
		}
		// Remove the digits.
		content = content.substr(pos + 1, content.size());
	} else {
		std::string lower_digits = ToLower(digits);
		if ((lower_digits == "all") || (lower_digits == "tutto")) {
			// Set the prefix flag.
			prefix.all = true;
			// Remove the quantity.
			content = content.substr(pos + 1, content.size());
		}
	}
}

void Argument::evaluateQuantity()
{
	// If the entire string is a number, skip it.
	if (IsNumber(content)) {
		return;
	}
	// Otherwise try to find a number if there is one.
	std::string::size_type pos = content.find('*');
	if (pos == std::string::npos) {
		return;
	}
	// Extract the digits.
	std::string digits = content.substr(0, pos);
	// Check the digits.
	if (IsNumber(digits)) {
		// Get the number and set it.
		int number = ToNumber<int>(digits);
		if ((number >= 0) && (number < INT_MAX)) {
			// Set the number.
			quantity = static_cast<unsigned int>(number);
			// Set the prefix flag.
			prefix.quantity = true;
		}
		// Remove the digits.
		content = content.substr(pos + 1, content.size());
	}
}

const Argument *ArgumentHandler::find(std::string const &s, bool exact)
{
	for (const_iterator it = arguments.begin(); it != arguments.end(); ++it) {
		if (exact) {
			if (it->getContent() == s) {
				return &(*it);
			}
		} else {
			if (BeginWith(it->getContent(), s)) {
				return &(*it);
			}
		}
	}
	return NULL;
}

/// Returns the string from the given argument to the end.
std::string ArgumentHandler::substr(size_t _start, size_t _end) const
{
	if (_start >= this->size()) {
		MudLog(LogLevel::Debug, "Starting argument out of bound!");
		return original;
	}
	if (_end == std::string::npos) {
		_end = arguments.size();
	}
	std::string result;
	for (size_t it = _start; it < _end; ++it) {
		result.append(arguments[it].getOriginal());
		if (it != (arguments.size() - 1)) {
			result.push_back(' ');
		}
	}
	return result;
}

/// Erase the argument at the given position.
void ArgumentHandler::erase(const size_t &position)
{
	if (position < arguments.size()) {
		std::vector<Argument>::iterator it = arguments.begin();
		std::advance(it, position);
		arguments.erase(it);
	} else {
		MudLog(LogLevel::Error, "Position out of bound!");
	}
}

/// @brief Prints a log of all the contained arguments.
void ArgumentHandler::dump() const
{
	for (size_t it = 0; it < arguments.size(); ++it) {
		MudLog(LogLevel::Debug,
			   "[%-2d][IDX:%-2d][MUL[Q:%1d;A:%1d]:%-2d][ALL:%1d] %s {%s}", it,
			   0, arguments.at(it).hasQuantity(),
			   arguments.at(it).hasPrefixAll(), arguments.at(it).getQuantity(),
			   arguments.at(it).meansAll(),
			   arguments.at(it).getContent().c_str(),
			   arguments.at(it).getOriginal().c_str());
	}
}

/// @brief Checks if the given argument is a fill word.
bool ArgumentHandler::isFillWord(const std::string &argument)
{
	static std::string fill_words[] = { "in", "from", "with", "and", "the",
										"on", "at",	  "to",	  "da",	 "su",
										"a",  "e",	  "il",	  "la",	 "lo",
										"un", "uno",  "una",  "con", "\n" };
	std::string argument_lower = ToLower(argument);
	for (int i = 0; fill_words[i] != "\n"; i++) {
		if (argument_lower == fill_words[i]) {
			return true;
		}
	}
	return false;
}

/// Given that original string has been set, fills the vector of arguments.
void ArgumentHandler::evaluateArguments(bool skip_fill_words)
{
	std::vector<std::string> words = SplitString(original, " ");
	for (std::vector<std::string>::const_iterator it = words.begin();
		 it != words.end(); ++it) {
		if (skip_fill_words && this->isFillWord(*it)) {
			continue;
		}
		arguments.push_back(Argument(*it));
	}
}
