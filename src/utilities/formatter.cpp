/// @file   formatter.cpp
/// @brief  Implements formatter class methods.
/// @author Enrico Fraccaroli
/// @date   Jan 25 2016
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

#include "formatter.hpp"
#include "telnetChar.hpp"

Formatter::Formatter()
{
    // Nothing to do.
}

Formatter::~Formatter()
{
    // Nothing to do.
}

Formatter::Format Formatter::getFormat()
{
    return Format::ASCII;
}

std::string Formatter::doClearMap()
{
    std::string output;
    if (getFormat() == Format::CLIENT)
    {
        output += '\0';
        output += static_cast<char>(TelnetChar::IAC);
        output += static_cast<char>(TelnetChar::DO);
        output += static_cast<char>(TelnetChar::CLR_MAP);
        output += '\0';
    }
    return output;
}

std::string Formatter::doDrawMap()
{
    std::string output;
    if (getFormat() == Format::CLIENT)
    {
        output += '\0';
        output += static_cast<char>(TelnetChar::IAC);
        output += static_cast<char>(TelnetChar::DO);
        output += static_cast<char>(TelnetChar::DRAW_MAP);
        output += '\0';
    }
    return output;
}

std::string Formatter::dontDrawMap()
{
    std::string output;
    if (getFormat() == Format::CLIENT)
    {
        output += '\0';
        output += static_cast<char>(TelnetChar::IAC);
        output += static_cast<char>(TelnetChar::DONT);
        output += static_cast<char>(TelnetChar::DRAW_MAP);
        output += '\0';
    }
    return output;
}

std::string Formatter::reset()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "RESET" + dontFormat();
    }
    else
    {
        return "\u001B[0m";
    }
}

std::string Formatter::cursorHome()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "CURSOR-HOME" + dontFormat();
    }
    else
    {
        return "\u001b[H";
    }
}

std::string Formatter::clearScreen()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "CLEAR-SCREEN" + dontFormat();
    }
    else
    {
        return "\u001B[2J";
    }
}

std::string Formatter::bold()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "BOLD" + dontFormat();
    }
    else
    {
        return "\u001B[1m";
    }
}

std::string Formatter::italic()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "ITALIC" + dontFormat();
    }
    else
    {
        return "\u001B[3m";
    }
}

std::string Formatter::black()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "BLACK" + dontFormat();
    }
    else
    {
        return "\u001B[0;30m";
    }
}

std::string Formatter::red()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "RED" + dontFormat();
    }
    else
    {
        return "\u001B[0;31m";
    }
}

std::string Formatter::green()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "GREEN" + dontFormat();
    }
    else
    {
        return "\u001B[0;32m";
    }
}

std::string Formatter::brown()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "BROWN" + dontFormat();
    }
    else
    {
        return "\u001B[0;33m";
    }
}

std::string Formatter::yellow()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "YELLOW" + dontFormat();
    }
    else
    {
        return "\u001B[1;33m";
    }
}

std::string Formatter::blue()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "BLUE" + dontFormat();
    }
    else
    {
        return "\u001B[0;34m";
    }
}

std::string Formatter::magenta()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "MAGENTA" + dontFormat();
    }
    else
    {
        return "\u001B[0;35m";
    }
}

std::string Formatter::cyan()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "CYAN" + dontFormat();
    }
    else
    {
        return "\u001B[0;36m";
    }
}

std::string Formatter::gray()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "GRAY" + dontFormat();
    }
    else
    {
        return "\u001B[0;37m";
    }
}

std::string Formatter::darkGray()
{
    if (getFormat() == Format::CLIENT)
    {
        return doFormat() + "DARK-GRAY" + dontFormat();
    }
    else
    {
        return "\u001B[1;30m";
    }
}

std::string Formatter::doFormat()
{
    std::string output;
    if (getFormat() == Format::CLIENT)
    {
        output += '\0';
        output += static_cast<char>(TelnetChar::IAC);
        output += static_cast<char>(TelnetChar::DO);
        output += static_cast<char>(TelnetChar::FORMAT);
        output += '\0';
    }
    return output;
}

std::string Formatter::dontFormat()
{
    std::string output;
    if (getFormat() == Format::CLIENT)
    {
        output += '\0';
        output += static_cast<char>(TelnetChar::IAC);
        output += static_cast<char>(TelnetChar::DONT);
        output += static_cast<char>(TelnetChar::FORMAT);
        output += '\0';
    }
    return output;
}
