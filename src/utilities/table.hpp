/// @file    table.hpp
/// @brief   Define the class Table.
/// @details This class allows to hadle a printable table.
/// @author  Enrico Fraccaroli
/// @date    Dec 15 2015
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

#pragma once

#include "alignString.hpp"
#include <vector>

/// The structure used to provide a row.
using TableRow = std::vector<std::string>;

/// @brief A class which provide access and means to manage a table column.
class TableColumn
{
private:
    /// Column Title.
    std::string title;
    /// Column alignment.
    StringAlign alignment;
    /// Column width.
    size_t width;

public:
    /// @brief Constructor.
    /// @param _title     The column title.
    /// @param _alignment The column alignment.
    /// @param _width     The column width.
    TableColumn(std::string _title, StringAlign _alignment, size_t _width = 0) :
        title(std::move(_title)),
        alignment(_alignment),
        width(_width)
    {
        if (width == 0) width = static_cast<size_t>(title.size()) + 2;
    }

    /// @brief Provide access to the title of the column.
    /// @return The title of the column
    inline std::string getTitle() const
    {
        return title;
    }

    /// @brief Provides the width of the column.
    /// @return The width of the column.
    inline size_t getWidth() const
    {
        return width;
    }

    /// @brief Provides access to the allignment of the column.
    /// @return The allignment of the column.
    inline StringAlign getAlignment()
    {
        return alignment;
    }

    /// @brief Allows to set the width of the column.
    /// @param _width The new width.
    inline void setWidth(size_t _width)
    {
        if (width < (_width + 2)) width = (_width + 2);
    }
};

/// @brief A simple formatted table.
class Table
{
private:
    /// Title of the table.
    std::string title;
    /// List of columns of the table.
    std::vector<TableColumn> columns;
    /// List of rows of the table.
    std::vector<TableRow> rows;

public:
    /// @brief Constructor.
    Table() = default;

    /// @brief Constructor.
    /// @param _title The title of the table.
    explicit Table(std::string _title) :
        title(std::move(_title)),
        columns(),
        rows()
    {
        // Nothing to do.
    }

    /// @brief Allows to add a column.
    /// @param columnTitle     The column title.
    /// @param columnAlignment The column alignment.
    /// @param columnWidth     The column width.
    inline void addColumn(std::string _title,
                          StringAlign _align,
                          size_t _width = 0)
    {
        columns.emplace_back(TableColumn(std::move(_title), _align, _width));
    }

    /// @brief Allows to add a row of values.
    /// @param row The vector which containts the row values.
    inline void addRow(TableRow row)
    {
        if (row.size() == columns.size())
        {
            for (size_t i = 0; i < columns.size(); ++i)
            {
                columns[i].setWidth(row[i].size());
            }
            rows.push_back(row);
        }
    }
    /// @brief Provides the number of rows inside the table.
    /// @return The number of rows.
    inline size_t getNumRows() const
    {
        return rows.size();
    }

    /// @brief Adds an empty row as divider.
    inline void addDivider()
    {
        this->addRow(std::vector<std::string>(columns.size(), ""));
    }

    /// @brief Removes the last row.
    inline void popRow()
    {
        this->rows.pop_back();
    }

    /// @brief Provides the table.
    /// @param withoutHeaders Allows to hide the header of the columns.
    /// @return The table.
    std::string getTable(bool withoutHeaders = false);

private:
    /// @brief Provides a divided of the same width of the table.
    /// @return The divider.
    inline std::string getDivider() const
    {
        std::string output;
        for (const auto & column : columns)
        {
            output += "#" + std::string(column.getWidth(), '#');
        }
        return output += "#\n";
    }

    /// @brief Provides the table title.
    /// @return The table title.
    inline std::string getTitle() const
    {
        return "#" +
               AlignString(title, StringAlign::Center,
                           getTotalWidth() + (columns.size() - 1)) + "#\n";
    }

    /// @brief Provides the columns headers.
    /// @return The headers.
    inline std::string getHeaders() const
    {
        std::string output;
        for (const auto & it : columns)
        {
            output += "|" +
                      AlignString(it.getTitle(),
                                  StringAlign::Center,
                                  it.getWidth());
        }
        return output + "|\n";
    }

    /// @brief Provides the total width of th table.
    /// @return The total width.
    inline size_t getTotalWidth() const
    {
        size_t totalWidth = 0;
        for (auto const & it : columns)
        {
            totalWidth += it.getWidth();
        }
        return totalWidth;
    }
};
