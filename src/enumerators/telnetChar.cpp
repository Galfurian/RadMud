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


#include <unordered_map>

using enum TelnetChar;

std::string to_string(TelnetChar value) {
    switch (value) {
        case MSDP_VAR: return "MSDP_VAR";
        case MSDP_VAL: return "MSDP_VAL";
        case MSDP_TABLE_OPEN: return "MSDP_TABLE_OPEN";
        case MSDP_TABLE_CLOSE: return "MSDP_TABLE_CLOSE";
        case MSDP_ARRAY_OPEN: return "MSDP_ARRAY_OPEN";
        case MSDP_ARRAY_CLOSE: return "MSDP_ARRAY_CLOSE";
        case TerminalType: return "TerminalType";
        case NegotiateAboutWindowSize: return "NegotiateAboutWindowSize";
        case TerminalSpeed: return "TerminalSpeed";
        case RemoteFlowControl: return "RemoteFlowControl";
        case LineMode: return "LineMode";
        case EnvironmentVariables: return "EnvironmentVariables";
        case NewEnvironmentOption: return "NewEnvironmentOption";
        case MSDP: return "MSDP";
        case MSSP: return "MSSP";
        case MCCP: return "MCCP";
        case DRAW_MAP: return "DRAW_MAP";
        case CLR_MAP: return "CLR_MAP";
        case FORMAT: return "FORMAT";
        case SubNegotiationEnd: return "SubNegotiationEnd";
        case NoOperation: return "NoOperation";
        case DataMark: return "DataMark";
        case Break: return "Break";
        case InterruptProcess: return "InterruptProcess";
        case AbortOutput: return "AbortOutput";
        case AreYouThere: return "AreYouThere";
        case EraseCharacter: return "EraseCharacter";
        case EraseLine: return "EraseLine";
        case GoAhead: return "GoAhead";
        case SubnegotiationBegin: return "SubnegotiationBegin";
        case WILL: return "WILL";
        case WONT: return "WONT";
        case DO: return "DO";
        case DONT: return "DONT";
        case IAC: return "IAC";
        case None: return "NONE";
        default: return "NONE";
    }
}

TelnetChar telnet_char_from_string(const std::string &str) {
    static const std::unordered_map<std::string, TelnetChar> map = {
        {"MSDP_VAR", MSDP_VAR},
        {"MSDP_VAL", MSDP_VAL},
        {"MSDP_TABLE_OPEN", MSDP_TABLE_OPEN},
        {"MSDP_TABLE_CLOSE", MSDP_TABLE_CLOSE},
        {"MSDP_ARRAY_OPEN", MSDP_ARRAY_OPEN},
        {"MSDP_ARRAY_CLOSE", MSDP_ARRAY_CLOSE},
        {"TerminalType", TerminalType},
        {"NegotiateAboutWindowSize", NegotiateAboutWindowSize},
        {"TerminalSpeed", TerminalSpeed},
        {"RemoteFlowControl", RemoteFlowControl},
        {"LineMode", LineMode},
        {"EnvironmentVariables", EnvironmentVariables},
        {"NewEnvironmentOption", NewEnvironmentOption},
        {"MSDP", MSDP},
        {"MSSP", MSSP},
        {"MCCP", MCCP},
        {"DRAW_MAP", DRAW_MAP},
        {"CLR_MAP", CLR_MAP},
        {"FORMAT", FORMAT},
        {"SubNegotiationEnd", SubNegotiationEnd},
        {"NoOperation", NoOperation},
        {"DataMark", DataMark},
        {"Break", Break},
        {"InterruptProcess", InterruptProcess},
        {"AbortOutput", AbortOutput},
        {"AreYouThere", AreYouThere},
        {"EraseCharacter", EraseCharacter},
        {"EraseLine", EraseLine},
        {"GoAhead", GoAhead},
        {"SubnegotiationBegin", SubnegotiationBegin},
        {"WILL", WILL},
        {"WONT", WONT},
        {"DO", DO},
        {"DONT", DONT},
        {"IAC", IAC},
        {"NONE", None}
    };
    auto it = map.find(str);
    if (it != map.end()) return it->second;
    return None;
}
