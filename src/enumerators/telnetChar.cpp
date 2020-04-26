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

std::string TelnetChar::toString() const
{
	if (value == MSDP_VAR)
		return "MSDP_VAR";
	if (value == MSDP_VAL)
		return "MSDP_VAL";
	if (value == MSDP_TABLE_OPEN)
		return "MSDP_TABLE_OPEN";
	if (value == MSDP_TABLE_CLOSE)
		return "MSDP_TABLE_CLOSE";
	if (value == MSDP_ARRAY_OPEN)
		return "MSDP_ARRAY_OPEN";
	if (value == MSDP_ARRAY_CLOSE)
		return "MSDP_ARRAY_CLOSE";
	if (value == TerminalType)
		return "TerminalType";
	if (value == NegotiateAboutWindowSize)
		return "NegotiateAboutWindowSize";
	if (value == TerminalSpeed)
		return "TerminalSpeed";
	if (value == RemoteFlowControl)
		return "RemoteFlowControl";
	if (value == LineMode)
		return "LineMode";
	if (value == EnvironmentVariables)
		return "EnvironmentVariables";
	if (value == NewEnvironmentOption)
		return "NewEnvironmentOption";
	//if (value == TTYPE) return "TTYPE";
	if (value == MSDP)
		return "MSDP";
	if (value == MSSP)
		return "MSSP";
	if (value == MCCP)
		return "MCCP";
	if (value == SubNegotiationEnd)
		return "SubNegotiationEnd";
	if (value == NoOperation)
		return "NoOperation";
	if (value == DataMark)
		return "DataMark";
	if (value == Break)
		return "Break";
	if (value == InterruptProcess)
		return "InterruptProcess";
	if (value == AbortOutput)
		return "AbortOutput";
	if (value == AreYouThere)
		return "AreYouThere";
	if (value == EraseCharacter)
		return "EraseCharacter";
	if (value == EraseLine)
		return "EraseLine";
	if (value == GoAhead)
		return "GoAhead";
	if (value == SubnegotiationBegin)
		return "SubnegotiationBegin";
	if (value == WILL)
		return "WILL";
	if (value == WONT)
		return "WONT";
	if (value == DO)
		return "DO";
	if (value == DONT)
		return "DONT";
	if (value == IAC)
		return "IAC";
	if (value == DRAW_MAP)
		return "DRAW_MAP";
	if (value == DRAW_MAP)
		return "DRAW_MAP";
	if (value == FORMAT)
		return "FORMAT";
	return "NONE";
}
