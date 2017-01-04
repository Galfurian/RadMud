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

#include "telnetChar.hpp"

TelnetChar::TelnetChar(unsigned int & _telnetChar) :
    telnetChar(None)
{
    if (_telnetChar == 1) telnetChar = MSDP_VAR;
    if (_telnetChar == 2) telnetChar = MSDP_VAL;
    if (_telnetChar == 3) telnetChar = MSDP_TABLE_OPEN;
    if (_telnetChar == 4) telnetChar = MSDP_TABLE_CLOSE;
    if (_telnetChar == 5) telnetChar = MSDP_ARRAY_OPEN;
    if (_telnetChar == 6) telnetChar = MSDP_ARRAY_CLOSE;
    if (_telnetChar == 24) telnetChar = TerminalType;
    if (_telnetChar == 31) telnetChar = NegotiateAboutWindowSize;
    if (_telnetChar == 32) telnetChar = TerminalSpeed;
    if (_telnetChar == 33) telnetChar = RemoteFlowControl;
    if (_telnetChar == 34) telnetChar = LineMode;
    if (_telnetChar == 36) telnetChar = EnvironmentVariables;
    if (_telnetChar == 39) telnetChar = NewEnvironmentOption;
    if (_telnetChar == 69) telnetChar = MSDP;
    if (_telnetChar == 70) telnetChar = MSSP;
    if (_telnetChar == 86) telnetChar = MCCP;
    if (_telnetChar == 91) telnetChar = DRAW_MAP;
    if (_telnetChar == 92) telnetChar = CLR_MAP;
    if (_telnetChar == 100) telnetChar = FORMAT;
    if (_telnetChar == 240) telnetChar = SubNegotiationEnd;
    if (_telnetChar == 241) telnetChar = NoOperation;
    if (_telnetChar == 242) telnetChar = DataMark;
    if (_telnetChar == 243) telnetChar = Break;
    if (_telnetChar == 244) telnetChar = InterruptProcess;
    if (_telnetChar == 245) telnetChar = AbortOutput;
    if (_telnetChar == 246) telnetChar = AreYouThere;
    if (_telnetChar == 247) telnetChar = EraseCharacter;
    if (_telnetChar == 248) telnetChar = EraseLine;
    if (_telnetChar == 249) telnetChar = GoAhead;
    if (_telnetChar == 250) telnetChar = SubnegotiationBegin;
    if (_telnetChar == 251) telnetChar = WILL;
    if (_telnetChar == 252) telnetChar = WONT;
    if (_telnetChar == 253) telnetChar = DO;
    if (_telnetChar == 254) telnetChar = DONT;
    if (_telnetChar == 255) telnetChar = IAC;
}

TelnetChar::TelnetChar(const Enum & _telnetChar) :
    telnetChar(_telnetChar)
{
    // Nothing to do.
}

bool TelnetChar::isValid(const unsigned int & _telnetChar)
{
    if (_telnetChar == 1) return true;
    if (_telnetChar == 2) return true;
    if (_telnetChar == 3) return true;
    if (_telnetChar == 4) return true;
    if (_telnetChar == 5) return true;
    if (_telnetChar == 6) return true;
    if (_telnetChar == 24) return true;
    if (_telnetChar == 31) return true;
    if (_telnetChar == 32) return true;
    if (_telnetChar == 33) return true;
    if (_telnetChar == 34) return true;
    if (_telnetChar == 36) return true;
    if (_telnetChar == 39) return true;
    if (_telnetChar == 69) return true;
    if (_telnetChar == 70) return true;
    if (_telnetChar == 86) return true;
    if (_telnetChar == 91) return true;
    if (_telnetChar == 92) return true;
    if (_telnetChar == 100) return true;
    if (_telnetChar == 240) return true;
    if (_telnetChar == 241) return true;
    if (_telnetChar == 242) return true;
    if (_telnetChar == 243) return true;
    if (_telnetChar == 244) return true;
    if (_telnetChar == 245) return true;
    if (_telnetChar == 246) return true;
    if (_telnetChar == 247) return true;
    if (_telnetChar == 248) return true;
    if (_telnetChar == 249) return true;
    if (_telnetChar == 250) return true;
    if (_telnetChar == 251) return true;
    if (_telnetChar == 252) return true;
    if (_telnetChar == 253) return true;
    if (_telnetChar == 254) return true;
    return (_telnetChar == 255);
}

std::string TelnetChar::toString() const
{
    if (telnetChar == MSDP_VAR) return "MSDP_VAR";
    if (telnetChar == MSDP_VAL) return "MSDP_VAL";
    if (telnetChar == MSDP_TABLE_OPEN) return "MSDP_TABLE_OPEN";
    if (telnetChar == MSDP_TABLE_CLOSE) return "MSDP_TABLE_CLOSE";
    if (telnetChar == MSDP_ARRAY_OPEN) return "MSDP_ARRAY_OPEN";
    if (telnetChar == MSDP_ARRAY_CLOSE) return "MSDP_ARRAY_CLOSE";
    if (telnetChar == TerminalType) return "TerminalType";
    if (telnetChar == NegotiateAboutWindowSize)
        return "NegotiateAboutWindowSize";
    if (telnetChar == TerminalSpeed) return "TerminalSpeed";
    if (telnetChar == RemoteFlowControl) return "RemoteFlowControl";
    if (telnetChar == LineMode) return "LineMode";
    if (telnetChar == EnvironmentVariables) return "EnvironmentVariables";
    if (telnetChar == NewEnvironmentOption) return "NewEnvironmentOption";
    //if (telnetChar == TTYPE) return "TTYPE";
    if (telnetChar == MSDP) return "MSDP";
    if (telnetChar == MSSP) return "MSSP";
    if (telnetChar == MCCP) return "MCCP";
    if (telnetChar == SubNegotiationEnd) return "SubNegotiationEnd";
    if (telnetChar == NoOperation) return "NoOperation";
    if (telnetChar == DataMark) return "DataMark";
    if (telnetChar == Break) return "Break";
    if (telnetChar == InterruptProcess) return "InterruptProcess";
    if (telnetChar == AbortOutput) return "AbortOutput";
    if (telnetChar == AreYouThere) return "AreYouThere";
    if (telnetChar == EraseCharacter) return "EraseCharacter";
    if (telnetChar == EraseLine) return "EraseLine";
    if (telnetChar == GoAhead) return "GoAhead";
    if (telnetChar == SubnegotiationBegin) return "SubnegotiationBegin";
    if (telnetChar == WILL) return "WILL";
    if (telnetChar == WONT) return "WONT";
    if (telnetChar == DO) return "DO";
    if (telnetChar == DONT) return "DONT";
    if (telnetChar == IAC) return "IAC";
    if (telnetChar == DRAW_MAP) return "DRAW_MAP";
    if (telnetChar == DRAW_MAP) return "DRAW_MAP";
    if (telnetChar == FORMAT) return "FORMAT";
    return "NONE";
}

unsigned int TelnetChar::toUInt() const
{
    return static_cast<unsigned int>(telnetChar);
}

char TelnetChar::toChar() const
{
    return static_cast<char>(telnetChar);
}

bool TelnetChar::operator==(const TelnetChar::Enum & rhs) const
{
    return telnetChar == rhs;
}

bool TelnetChar::operator!=(const TelnetChar::Enum & rhs) const
{
    return telnetChar != rhs;
}
