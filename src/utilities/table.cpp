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

#include <sstream>
#include <iomanip>

#include "logger.hpp"

TableColumn::TableColumn(std::string _title, StringAlign _alignment, size_t _width) :
        title(_title),
        alignment(_alignment),
        width(_width)
{
    if (width == 0)
    {
        width = static_cast<size_t>(title.size()) + 2;
    }
}

std::string TableColumn::getTitle()
{
    return title;
}

size_t TableColumn::getWidth()
{
    return width;
}

StringAlign TableColumn::getAlignment()
{
    return alignment;
}

void TableColumn::setWidth(size_t _width)
{
    if ((_width + 2) > width)
    {
        width = (_width + 2);
    }
}

Table::Table() :
        title(),
        columns(),
        rows()
{
    // Nothing to do.
}

Table::Table(std::string _title) :
        title(_title),
        columns(),
        rows()
{
    // Nothing to do.
}

Table::~Table()
{
    // Nothing to do.
}

void Table::addColumn(std::string columnTitle, StringAlign columnAlignment, size_t columnWidth)
{
    columns.push_back(TableColumn(columnTitle, columnAlignment, columnWidth));
}

void Table::addRow(TableRow row)
{
    if (row.size() != columns.size())
    {
        Logger::log(LogLevel::Warning, "Column number and provided row cells are different.");
        return;
    }
    for (size_t idx = 0; idx < columns.size(); ++idx)
    {
        columns[idx].setWidth(row[idx].size());
    }
    rows.push_back(row);
}

std::string Table::getTable(bool withoutHeaders)
{
    std::string output;
    if (!title.empty())
    {
        output += getDivider();
        output += getTitle();
    }
    if (!withoutHeaders)
    {
        output += getDivider();
        output += getHeaders();
    }
    output += getDivider();
    for (auto row : rows)
    {
        unsigned int column = 0;
        for (auto cell : row)
        {
            output += "#"
                + AlignString(cell, columns[column].getAlignment(), columns[column].getWidth());
            column++;
        }
        output += "#\n";
    }
    output += getDivider();
    return output;
}

size_t Table::getNumRows()
{
    return rows.size();
}

void Table::addDivider()
{
    std::vector<std::string> divider;
    for (auto column : columns)
    {
        divider.push_back("");
    }
    this->addRow(divider);
}

void Table::popRow()
{
    this->rows.pop_back();
}

std::string Table::getDivider()
{
    std::string output;
    for (auto column : columns)
    {
        output += "#";
        for (size_t w = 0; w < column.getWidth(); ++w)
        {
            output += '-';
        }
    }
    return output += "#\n";
}

std::string Table::getTitle()
{
    return "#" + AlignString(title, StringAlign::Center, getTotalWidth() + (columns.size() - 1))
        + "#\n";
}

std::string Table::getHeaders()
{
    std::string output;
    for (auto it : columns)
    {
        output += "#" + AlignString(it.getTitle(), StringAlign::Center, it.getWidth());
    }
    output += "#\n";
    return output;
}

size_t Table::getTotalWidth()
{
    size_t totalWidth = 0;
    for (auto it : columns)
    {
        totalWidth += it.getWidth();
    }
    return totalWidth;
}
