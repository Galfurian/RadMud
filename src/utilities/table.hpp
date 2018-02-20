/// @file    table.hpp
/// @brief   Define the class Table.
/// @details This class allows to hadle a printable table.
/// @author  Enrico Fraccaroli
/// @date    Dec 15 2015
/// @copyright
/// Copyright (c) 2015, 2016, 2017, 2018
///  Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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
#include "utils.hpp"

#include <vector>
#include <sstream>
#include <iomanip>
#include <map>

/// @brief Structure used to manage a row of the table.
class TableRow :
    public std::vector<std::string>
{
public:
    TableRow() :
        divider(),
        header()
    {
        // Nothing to do.
    }

    TableRow(std::vector<std::string>::size_type const & size,
             std::string const & initializer) :
        std::vector<std::string>(size, initializer),
        divider(),
        header()
    {
        // Nothing to do.
    }

    TableRow(std::initializer_list<std::string> const & initializer) :
        std::vector<std::string>(initializer),
        divider(),
        header()
    {
        // Nothing to do.
    }

    virtual inline bool isDivider() const
    {
        return divider;
    }

    virtual inline bool isHeader() const
    {
        return header;
    }

private:
    bool divider;
    bool header;

    friend class Table;
};

/// @brief A class which provide access and means to manage a table column.
class TableColumn
{
public:
    /// Column Title.
    std::string title;
    /// Column alignment.
    align::align_t alignment;
    /// Column width.
    std::string::size_type width;
    /// If the column needs to autoadjust to the lenght of the content.
    const bool autoAdjust;

    /// @brief Constructor.
    /// @param _title     The column title.
    /// @param _alignment The column alignment.
    /// @param _width     The column width.
    TableColumn(std::string _title,
                align::align_t _alignment,
                std::string::size_type _width = 0,
                const bool _autoAdjust = true) :
        title(std::move(_title)),
        alignment(_alignment),
        width(_width),
        autoAdjust(_autoAdjust)
    {
        if (autoAdjust)
        {
            this->adjust(title);
        }
    }

    /// @brief Provide access to the title of the column.
    /// @return The title of the column
    inline std::string getTitle() const
    {
        return title;
    }

    /// @brief Provides the width of the column.
    /// @return The width of the column.
    inline std::string::size_type getWidth() const
    {
        return width;
    }

    /// @brief Provides access to the allignment of the column.
    /// @return The allignment of the column.
    inline align::align_t getAlignment()
    {
        return alignment;
    }

    inline void adjust(std::string const & reference)
    {
        if (autoAdjust)
        {
            auto _width = reference.size();
            if (_width > width) width = _width;
        }
    }
};

/// @brief A simple formatted table.
class Table
{
public:
    enum SymbolType
    {
        HorizontalDivider,
        VerticalDivider,
        Crossings,
    };

    /// @brief Constructor.
    explicit Table() :
        columns(),
        rows(),
        marginSize(1),
        symbols()
    {
        // Set the default symbols.
        this->resetSymbols();
    }

    /// @brief Allows to set the margins.
    inline void setMarginSize(std::string::size_type const & s)
    {
        marginSize = s;
    }

    /// @brief Allows to set a character to use for a specific place.
    inline void setSymbol(SymbolType type, char symbol)
    {
        symbols[type] = symbol;
    }

    /// @brief Sets the default the symbols.
    inline void resetSymbols()
    {
        symbols[HorizontalDivider] = '-';
        symbols[VerticalDivider] = '|';
        symbols[Crossings] = '+';
    }

    /// @brief Allows to add a column.
    /// @param columnTitle     The column title.
    /// @param columnAlignment The column alignment.
    /// @param columnWidth     The column width.
    inline void addColumn(TableColumn column)
    {
        columns.emplace_back(column);
    }

    /// @brief Allows to add a column.
    /// @param columnTitle     The column title.
    /// @param columnAlignment The column alignment.
    /// @param columnWidth     The column width.
    inline void addColumn(std::string const & _title,
                          align::align_t _alignment,
                          std::string::size_type _width = 0,
                          const bool _autoAdjust = true)
    {
        columns.emplace_back(
            TableColumn(_title, _alignment, _width, _autoAdjust));
    }

    /// @brief Allows to add a row of values.
    /// @param row The vector which containts the row values.
    inline void addRow(TableRow row)
    {
        if (row.size() != columns.size())
        {
            return;
        }
        std::vector<TableRow> new_rows;
        new_rows.emplace_back(row);

        unsigned int row_it = 0;
        auto column_it = columns.begin();
        for (; row_it < row.size(); ++row_it, ++column_it)
        {
            auto cell = row[row_it];

            if (column_it->autoAdjust)
            {
                column_it->adjust(cell);
            }
            else if (cell.size() > column_it->width)
            {
                auto wrappedText = TextWrap(cell, column_it->width);
                if (wrappedText.size() > new_rows.size())
                {
                    new_rows.resize(wrappedText.size(),
                                    TableRow(row.size(), ""));
                }
                size_t index = 0;
                for (auto const & wrappedLine : wrappedText)
                {
                    new_rows[index++][row_it] = wrappedLine;
                }
            }
        }
        for (auto & new_row : new_rows)
        {
            rows.emplace_back(new_row);
        }
    }

    /// @brief Adds the column headers.
    inline void addColumnHeaders()
    {
        TableRow new_row;
        for (auto const & column : columns)
        {
            new_row.emplace_back(column.getTitle());
        }
        rows.emplace_back(new_row);
    }

    /// @brief Adds a divider.
    inline void addDivider()
    {
        TableRow new_row;
        new_row.divider = true;
        rows.emplace_back(new_row);
    }

    /// @brief Adds an header with the given text.
    inline void addHeader(std::string header)
    {
        TableRow new_row;
        new_row.header = true;
        new_row.emplace_back(header);
        rows.emplace_back(new_row);
    }

    /// @brief Removes the last row.
    inline void popRow()
    {
        rows.pop_back();
    }

    /// @brief Provides the table.
    /// @param withoutHeaders Allows to hide the header of the columns.
    /// @return The table.
    std::string getTable()
    {
        auto margin = std::string(marginSize, ' ');
        auto hdiv = symbols[HorizontalDivider];
        auto vdiv = symbols[VerticalDivider];
        auto cross = symbols[Crossings];

        std::stringstream ss;
        for (auto row_it = rows.begin(); row_it != rows.end(); ++row_it)
        {
            auto row = (*row_it);
            if (row.isDivider())
            {
                ss << cross;
                if ((row_it == rows.begin()) ||
                    (std::next(row_it) == rows.end()))
                {
                    ss << std::string(
                        this->getTotalWidth()
                        + (marginSize * 2 + 1) * (columns.size()) - 1,
                        hdiv);
                    ss << cross;
                }
                else
                {
                    for (auto column : columns)
                    {
                        ss << std::string(column.width + marginSize * 2, hdiv)
                           << cross;
                    }
                }
                ss << "\n";
                continue;
            }
            if (row.isHeader())
            {
                ss << vdiv;
                ss << margin;
                ss << std::setw(
                    static_cast<int>(this->getTotalWidth()
                                     + (marginSize * 2 + 1)
                                       * (columns.size() - 1)));
                ss << std::right << align::centtered(row[0]);
                ss << margin;
                ss << vdiv;
                ss << "\n";
                continue;
            }
            ss << vdiv;
            for (size_t i = 0; i < row.size(); ++i)
            {
                auto const & column = columns[i];
                ss << margin;
                ss << std::setw(static_cast<int>(column.width));
                if (column.alignment == align::left)
                {
                    ss << std::left << row[i];
                }
                else if (column.alignment == align::right)
                {
                    ss << std::right << row[i];
                }
                else
                {
                    ss << std::right << align::centtered(row[i]);
                }
                ss << margin;
                ss << vdiv;
            }
            ss << "\n";
        }
        return ss.str();
    }

private:

    /// @brief Provides the total width of th table.
    /// @return The total width.
    inline std::string::size_type getTotalWidth() const
    {
        std::string::size_type totalWidth = 0;
        for (auto const & it : columns)
        {
            totalWidth += it.getWidth();
        }
        return totalWidth;
    }

    /// List of columns of the table.
    std::vector<TableColumn> columns;
    /// List of rows of the table.
    std::vector<TableRow> rows;
    /// The internal margins.
    std::string::size_type marginSize;
    /// Map of symbols;
    std::map<SymbolType, char> symbols;
};