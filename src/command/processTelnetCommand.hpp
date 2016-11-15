/// @file   processTelnetCommand.hpp
/// @brief  Define the negotiation functions for MUD.
/// @author Enrico Fraccaroli
/// @date   Jun 5 2016
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

#include "inputHandler.hpp"
#include "defines.hpp"

/// @brief Allows to estract from a string at the given position, a telnet command.
/// @param source  The source string.
/// @param index   The index from which the command has to be extracted.
/// @param command Where the extracted command has to be stored.
/// @param buffer  The buffer where the string version of the command will be store.
/// @return <b>True</b> if the function extracts a valid command,<br>
///         <b>False</b> otherwise.
bool ExtractCommand(
    const std::string & source,
    size_t & index,
    TelnetChar & command,
    std::string & buffer);

/// @brief Function for the protocol negotiation with the client.
/// @param character The character with which the protocol is negotiated.
/// @param nextState The state which identifies the stage of protocol negotiation.
void NegotiateProtocol(Character * character, const ConnectionState & nextState);

/// @brief Process for handling telnet commands.
class ProcessTelnetCommand :
    public InputHandler
{
public:
    void process(Character * character, ArgumentHandler & args);
};
