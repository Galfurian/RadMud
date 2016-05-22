/// @file   telnet.cpp
/// @brief  Implements telnet class methods.
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

#include "telnet.hpp"

#include "utils.hpp"

std::string Telnet::doFormat()
{
    std::string output;
    output += '\0';
    output += static_cast<char>(TelnetChar::IAC);
    output += static_cast<char>(TelnetChar::DO);
    output += static_cast<char>(TelnetChar::FORMAT);
    output += '\0';
    return output;
}

std::string Telnet::dontFormat()
{
    std::string output;
    output += '\0';
    output += static_cast<char>(TelnetChar::IAC);
    output += static_cast<char>(TelnetChar::DONT);
    output += static_cast<char>(TelnetChar::FORMAT);
    output += '\0';
    return output;
}

std::string Telnet::doClearMap()
{
    std::string output;
    output += '\0';
    output += static_cast<char>(TelnetChar::IAC);
    output += static_cast<char>(TelnetChar::DO);
    output += static_cast<char>(TelnetChar::CLR_MAP);
    output += '\0';
    return output;
}

std::string Telnet::doDrawMap()
{
    std::string output;
    output += '\0';
    output += static_cast<char>(TelnetChar::IAC);
    output += static_cast<char>(TelnetChar::DO);
    output += static_cast<char>(TelnetChar::DRAW_MAP);
    output += '\0';
    return output;
}
std::string Telnet::dontDrawMap()
{
    std::string output;
    output += '\0';
    output += static_cast<char>(TelnetChar::IAC);
    output += static_cast<char>(TelnetChar::DONT);
    output += static_cast<char>(TelnetChar::DRAW_MAP);
    output += '\0';
    return output;
}

std::string Telnet::reset()
{
    return doFormat() + "RESET" + dontFormat();
}
std::string Telnet::cursorHome()
{
    return doFormat() + "CURSOR-HOME" + dontFormat();
}
std::string Telnet::clearScreen()
{
    return doFormat() + "CLEAR-SCREEN" + dontFormat();
}

std::string Telnet::bold()
{
    return doFormat() + "BOLD" + dontFormat();
}
std::string Telnet::italic()
{
    return doFormat() + "ITALIC" + dontFormat();
}

std::string Telnet::black()
{
    return doFormat() + "BLACK" + dontFormat();
}
std::string Telnet::red()
{
    return doFormat() + "RED" + dontFormat();
}
std::string Telnet::green()
{
    return doFormat() + "GREEN" + dontFormat();
}
std::string Telnet::brown()
{
    return doFormat() + "BROWN" + dontFormat();
}
std::string Telnet::yellow()
{
    return doFormat() + "YELLOW" + dontFormat();
}
std::string Telnet::blue()
{
    return doFormat() + "BLUE" + dontFormat();
}
std::string Telnet::magenta()
{
    return doFormat() + "MAGENTA" + dontFormat();
}
std::string Telnet::cyan()
{
    return doFormat() + "CYAN" + dontFormat();
}
std::string Telnet::gray()
{
    return doFormat() + "GRAY" + dontFormat();
}
std::string Telnet::darkGray()
{
    return doFormat() + "DARK-GRAY" + dontFormat();
}
