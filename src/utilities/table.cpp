/// @file   table.cpp
/// @brief  Implements the methods of the class Table.
/// @author Enrico Fraccaroli
/// @date   Dec 15 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "table.hpp"
#include "logger.hpp"

std::string Table::getTable(bool withoutHeaders,
                            bool withoutDividers)
{
    std::string output;
    // Add the title, if necessary.
    if (!title.empty())
        output += ((withoutDividers) ? "" : getDivider()) + getTitle();
    // Add the headers, if necessary.
    if (!withoutHeaders)
        output += ((withoutDividers) ? "" : getDivider()) + getHeaders();
    // Star with the table.
    output += ((withoutDividers) ? "" : getDivider());
    for (auto const & row : rows)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            output += "|" + AlignString(
                row.at(i),
                columns.at(i).getAlignment(),
                columns.at(i).getWidth());
        }
        output += "|\n";
    }
    output += ((withoutDividers) ? "" : getDivider());
    return output;
}