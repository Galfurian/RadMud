/// @file   tableLoader.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 18 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

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
    std::function<bool(ResultSet * result)> loadFunction;
    /// The custom loading query.
    std::string loadQuery;

    /// @brief Constructor.
    TableLoader(
        std::string _table,
        std::function<bool(ResultSet * result)> _loadFunction,
        std::string _loadQuery = "");

    /// @brief Destructor.
    ~TableLoader();

    /// Provide the query required to load the table.
    std::string getQuery() const;
};
