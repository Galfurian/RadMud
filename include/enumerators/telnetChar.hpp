/// @file   telnetChar.hpp
/// @brief  Define the telnet character class.
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

#pragma once

#include "baseEnumerator.hpp"

/// List of telnet commands.
class TelnetChar : public BaseEnumerator {
public:
	// OPTIONS
	// Echo = 1,
	// SuppressGoAhead = 3,
	// Status = 5,
	// TimingMark = 6,
	//TTYPE = 24,       ///< Mud Terminal Type Standard

	/// List of possible telnet chars.
	enum Enum {
		None = 0,
		MSDP_VAR = 1,
		MSDP_VAL = 2,
		MSDP_TABLE_OPEN = 3,
		MSDP_TABLE_CLOSE = 4,
		MSDP_ARRAY_OPEN = 5,
		MSDP_ARRAY_CLOSE = 6,
		TerminalType = 24,
		NegotiateAboutWindowSize = 31,
		TerminalSpeed = 32,
		RemoteFlowControl = 33,
		LineMode = 34,
		EnvironmentVariables = 36,
		NewEnvironmentOption = 39,
		MSDP = 69, ///< Mud Server Data Protocol
		MSSP = 70, ///< Mud Server Status Protocol
		MCCP = 86, ///< Mud Client Compression Protocol
		DRAW_MAP = 91, ///< MUD SPECIFIC: I will send the map.
		CLR_MAP = 92, ///< MUD SPECIFIC: Please, clear the already drawn map.
		FORMAT = 100, ///< MUD SPECIFIC: I will send a format string.
		SubNegotiationEnd = 240,
		NoOperation = 241,
		DataMark = 242,
		Break = 243,
		InterruptProcess = 244,
		AbortOutput = 245,
		AreYouThere = 246,
		EraseCharacter = 247,
		EraseLine = 248,
		GoAhead = 249,
		SubnegotiationBegin = 250,
		WILL = 251, ///< I will use option.
		WONT = 252, ///< I wont use option.
		DO = 253, ///< Please, you use option
		DONT = 254, ///< Don't use option.
		IAC = 255 ///< Interpret As Command
	};

	/// @brief Constructor.
	TelnetChar() : BaseEnumerator()
	{
		// Nothing to do.
	}

	/// @brief Constructor from unsigned int.
	explicit TelnetChar(const unsigned int &_value) : BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Constructor from enum.
	TelnetChar(const Enum &_value) : BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Returns the telnet character as string.
	std::string toString() const override;
};
