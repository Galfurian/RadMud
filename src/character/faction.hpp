/// @file   faction.hpp
/// @brief  Define the faction variables.
/// @author Enrico Fraccaroli
/// @date   Sep 30 2014
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

#include <list>
#include <string>
#include <vector>
#include <map>
#include <memory>

class CurrencyModel;
class Table;

/// @brief Holds details about a faction.
class Faction
{
public:
    /// The faction identification number.
    int vnum;
    /// The name of the faction.
    std::string name;
    /// The description of the faction.
    std::string description;
    /// The currency used by this faction.
    std::shared_ptr<CurrencyModel> currency;

    /// @brief Contructor.
    Faction();

    /// @brief Disable Copy Construct.
    Faction(Faction const &) = delete;

    /// @brief Disable Move construct.
    Faction(Faction &&) = delete;

    /// @brief Disable Copy assign.
    Faction & operator=(Faction const &) = delete;

    /// @brief Disable Move assign.
    Faction & operator=(Faction &&) = delete;

    /// @brief Destructor.
    ~Faction();

    /// @brief Check the correctness of the faction.
    /// @return <b>True</b> if the faction has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Fills the provided table with the faction informations.
    /// @param sheet The table that has to be filled.
    void getSheet(Table & sheet) const;

    /// @brief Return the name of the faction.
    /// @return The name of the faction.
    std::string getName();

    /// @brief Return the name of the faction with the first letter capitalized.
    /// @return The name of the faction.
    std::string getNameCapital();
};
