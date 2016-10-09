/// @file   tableLoader.cpp
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

#include "tableLoader.hpp"

TableLoader::TableLoader(
    std::string _table,
    std::function<bool(ResultSet * result)> _loadFunction,
    std::string _loadQuery) :
    table(_table),
    loadFunction(_loadFunction),
    loadQuery(_loadQuery)
{
    // Nothing to do.
}

std::string TableLoader::getQuery() const
{
    if (loadQuery.empty())
    {
        return "SELECT * FROM " + table + ";";
    }
    else
    {
        return loadQuery;
    }
}
