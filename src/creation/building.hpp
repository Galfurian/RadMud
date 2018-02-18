/// @file   building.hpp
/// @brief  Define building variables and methods.
/// @author Enrico Fraccaroli
/// @date   Feb 24 2016
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

#include <memory>
#include <string>
#include <vector>
#include <map>

// Forward declarations.
class ToolType;

class ItemModel;

class ResourceType;

class Knowledge;

/// @brief Holds details about a building.
class Building
{
public:
    /// The virtual number of the building.
    unsigned int vnum;
    /// The name of the building.
    std::string name;
    /// The difficulty of the building.
    unsigned int difficulty;
    /// The time required to perform the building.
    unsigned int time;
    /// A flag which indicates if the maker can be assisted by someone.
    bool assisted;
    /// The model that has to be built.
    std::shared_ptr<ItemModel> buildingModel;
    /// The list of needed type of tools.
    std::vector<ToolType> tools;
    /// The list of needed type of ingredients and their quantity.
    std::map<ResourceType, unsigned int> ingredients;
    /// The required knowledge levels for the given building.
    std::vector<Knowledge> requiredKnowledge;

    /// @brief Constructor.
    Building();

    /// @brief Copy Constructor.
    Building(const Building & source);

    /// @brief Destructor.
    ~Building();

    /// @brief Return the name of the building.
    /// @return The name of the building.
    std::string getName();

    /// @brief Return the name with the first letter capitalized.
    /// @return The name of the building capitalized.
    std::string getNameCapital();
};
