/// @file   tableLoader.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 18 2016
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

#include <functional>

#include "sqliteWrapper.hpp"

/// @brief A customizable table loader.
class TableLoader
{
public:
    /// The name of the table.
    std::string table;
    /// The loading function.
    std::function<bool(std::shared_ptr<ResultSet> result)> loadFunction;
    /// The custom loading query.
    std::string loadQuery;

    /// @brief Constructor.
    TableLoader(
        std::string _table,
        std::function<bool(std::shared_ptr<ResultSet> result)> _loadFunction,
        std::string _loadQuery = "");

    /// Provide the query required to load the table.
    std::string getQuery() const;
};
