/// @file   production.hpp
/// @brief  Define production variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
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

class Profession;

class Character;

class ItemModel;

#include "resourceType.hpp"
#include "knowledge.hpp"
#include "toolType.hpp"

#include <string>
#include <vector>
#include <memory>
#include <map>

/// @brief Holds details about a production.
class Production {
public:
	/// The virtual number of the production.
	unsigned int vnum;
	/// The name of the production.
	std::string name;
	/// The profession needed to perform the production.
	Profession *profession;
	/// The difficulty of the production.
	unsigned int difficulty;
	/// The time required to perform the production.
	double time;
	/// A flag which indicates if the maker can be assisted by someone.
	bool assisted;
	/// A pointer to the outcome model.
	std::shared_ptr<ItemModel> outcome;
	/// The quantity of the outcome.
	unsigned int quantity;
	/// The list of needed type of tools.
	std::vector<ToolType> tools;
	/// The list of needed type of ingredients and their quantity.
	std::map<ResourceType, unsigned int> ingredients;
	/// The type of the workbench.
	ToolType workbench;
	/// The required knowledge levels for the given production.
	std::vector<Knowledge> requiredKnowledge;

	/// @brief Constructor.
	Production();

	/// @brief Disable Copy Construct.
	Production(Production const &) = delete;

	/// @brief Disable Move construct.
	Production(Production &&) = delete;

	/// @brief Disable Copy assign.
	Production &operator=(Production const &) = delete;

	/// @brief Disable Move assign.
	Production &operator=(Production &&) = delete;

	/// @brief Destructor.
	~Production();

	/// @brief Check the correctness of the production.
	/// @return <b>True</b> if the profession has correct values,<br>
	///         <b>False</b> otherwise.
	bool check();

	/// @brief Return the name of the production.
	/// @return The name of the production.
	std::string getName();

	/// @brief Return the name with the first letter capitalized.
	/// @return The name of the production capitalized.
	std::string getNameCapital();
};
