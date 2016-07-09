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

#include <string>
#include <vector>

#include "../utils.hpp"

/// The structure used to provide a row.
typedef std::vector<std::string> TableRow;

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
        TableColumn(std::string _title, StringAlign _alignment, size_t _width = 0);

        /// @brief Provide access to the title of the column.
        /// @return The title of the column
        std::string getTitle();

        /// @brief Provides the width of the column.
        /// @return The width of the column.
        size_t getWidth();

        /// @brief Provides access to the allignment of the column.
        /// @return The allignment of the column.
        StringAlign getAlignment();

        /// @brief Allows to set the width of the column.
        /// @param _width The new width.
        void setWidth(size_t _width);
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
        std::vector<std::vector<std::string> > rows;

    public:
        /// @brief Constructor.
        Table();

        /// @brief Constructor.
        /// @param _title The title of the table.
        Table(std::string _title);

        /// @brief Destructor.
        ~Table();

        /// @brief Allows to add a column.
        /// @param columnTitle     The column title.
        /// @param columnAlignment The column alignment.
        /// @param columnWidth     The column width.
        void addColumn(std::string columnTitle, StringAlign columnAlignment, size_t columnWidth = 0);

        /// @brief Allows to add a row of values.
        /// @param row The vector which containts the row values.
        void addRow(std::vector<std::string> row);

        /// @brief Provides the table.
        /// @param withoutHeaders Allows to hide the header of the columns.
        /// @return The table.
        std::string getTable(bool withoutHeaders = false);

        /// @brief Provides the number of rows inside the table.
        /// @return The number of rows.
        size_t getNumRows();

    private:
        /// @brief Provides a divided of the same width of the table.
        /// @return The divider.
        std::string getDivider();

        /// @brief Provides the table title.
        /// @return The table title.
        std::string getTitle();

        /// @brief Provides the columns headers.
        /// @return The headers.
        std::string getHeaders();

        /// @brief Provides the total width of th table.
        /// @return The total width.
        size_t getTotalWidth();
};
