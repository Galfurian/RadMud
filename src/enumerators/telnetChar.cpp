/// @file   telnetChar.cpp
/// @brief  Implements the telnet character class methods.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
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

#include "enumerators/telnetChar.hpp"

#include <unordered_map>

std::string telnet_char_to_string(TelnetChar value)
{
    switch (value) {
    case TelnetChar::MSDP_VAR:
        return "MSDP_VAR";
    case TelnetChar::MSDP_VAL:
        return "MSDP_VAL";
    case TelnetChar::MSDP_TABLE_OPEN:
        return "MSDP_TABLE_OPEN";
    case TelnetChar::MSDP_TABLE_CLOSE:
        return "MSDP_TABLE_CLOSE";
    case TelnetChar::MSDP_ARRAY_OPEN:
        return "MSDP_ARRAY_OPEN";
    case TelnetChar::MSDP_ARRAY_CLOSE:
        return "MSDP_ARRAY_CLOSE";
    case TelnetChar::TerminalType:
        return "TerminalType";
    case TelnetChar::NegotiateAboutWindowSize:
        return "NegotiateAboutWindowSize";
    case TelnetChar::TerminalSpeed:
        return "TerminalSpeed";
    case TelnetChar::RemoteFlowControl:
        return "RemoteFlowControl";
    case TelnetChar::LineMode:
        return "LineMode";
    case TelnetChar::EnvironmentVariables:
        return "EnvironmentVariables";
    case TelnetChar::NewEnvironmentOption:
        return "NewEnvironmentOption";
    case TelnetChar::MSDP:
        return "MSDP";
    case TelnetChar::MSSP:
        return "MSSP";
    case TelnetChar::MCCP:
        return "MCCP";
    case TelnetChar::DRAW_MAP:
        return "DRAW_MAP";
    case TelnetChar::CLR_MAP:
        return "CLR_MAP";
    case TelnetChar::FORMAT:
        return "FORMAT";
    case TelnetChar::SubNegotiationEnd:
        return "SubNegotiationEnd";
    case TelnetChar::NoOperation:
        return "NoOperation";
    case TelnetChar::DataMark:
        return "DataMark";
    case TelnetChar::Break:
        return "Break";
    case TelnetChar::InterruptProcess:
        return "InterruptProcess";
    case TelnetChar::AbortOutput:
        return "AbortOutput";
    case TelnetChar::AreYouThere:
        return "AreYouThere";
    case TelnetChar::EraseCharacter:
        return "EraseCharacter";
    case TelnetChar::EraseLine:
        return "EraseLine";
    case TelnetChar::GoAhead:
        return "GoAhead";
    case TelnetChar::SubnegotiationBegin:
        return "SubnegotiationBegin";
    case TelnetChar::WILL:
        return "WILL";
    case TelnetChar::WONT:
        return "WONT";
    case TelnetChar::DO:
        return "DO";
    case TelnetChar::DONT:
        return "DONT";
    case TelnetChar::IAC:
        return "IAC";
    case TelnetChar::None:
        return "NONE";
    default:
        return "NONE";
    }
}

TelnetChar telnet_char_from_string(const std::string &str)
{
    static const std::unordered_map<std::string, TelnetChar> map = {
        {"MSDP_VAR", TelnetChar::MSDP_VAR},
        {"MSDP_VAL", TelnetChar::MSDP_VAL},
        {"MSDP_TABLE_OPEN", TelnetChar::MSDP_TABLE_OPEN},
        {"MSDP_TABLE_CLOSE", TelnetChar::MSDP_TABLE_CLOSE},
        {"MSDP_ARRAY_OPEN", TelnetChar::MSDP_ARRAY_OPEN},
        {"MSDP_ARRAY_CLOSE", TelnetChar::MSDP_ARRAY_CLOSE},
        {"TerminalType", TelnetChar::TerminalType},
        {"NegotiateAboutWindowSize", TelnetChar::NegotiateAboutWindowSize},
        {"TerminalSpeed", TelnetChar::TerminalSpeed},
        {"RemoteFlowControl", TelnetChar::RemoteFlowControl},
        {"LineMode", TelnetChar::LineMode},
        {"EnvironmentVariables", TelnetChar::EnvironmentVariables},
        {"NewEnvironmentOption", TelnetChar::NewEnvironmentOption},
        {"MSDP", TelnetChar::MSDP},
        {"MSSP", TelnetChar::MSSP},
        {"MCCP", TelnetChar::MCCP},
        {"DRAW_MAP", TelnetChar::DRAW_MAP},
        {"CLR_MAP", TelnetChar::CLR_MAP},
        {"FORMAT", TelnetChar::FORMAT},
        {"SubNegotiationEnd", TelnetChar::SubNegotiationEnd},
        {"NoOperation", TelnetChar::NoOperation},
        {"DataMark", TelnetChar::DataMark},
        {"Break", TelnetChar::Break},
        {"InterruptProcess", TelnetChar::InterruptProcess},
        {"AbortOutput", TelnetChar::AbortOutput},
        {"AreYouThere", TelnetChar::AreYouThere},
        {"EraseCharacter", TelnetChar::EraseCharacter},
        {"EraseLine", TelnetChar::EraseLine},
        {"GoAhead", TelnetChar::GoAhead},
        {"SubnegotiationBegin", TelnetChar::SubnegotiationBegin},
        {"WILL", TelnetChar::WILL},
        {"WONT", TelnetChar::WONT},
        {"DO", TelnetChar::DO},
        {"DONT", TelnetChar::DONT},
        {"IAC", TelnetChar::IAC},
        {"NONE", TelnetChar::None}};
    auto it = map.find(str);
    if (it != map.end())
        return it->second;
    return TelnetChar::None;
}
