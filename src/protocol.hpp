/// @file   protocol.hpp
/// @brief  Define the negotiation functions for MUD.
/// @author Enrico Fraccaroli
/// @date   Jun 5 2016
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

#pragma once

#include "character.hpp"

/// @brief Map all the connection states which are necessary for the
///         negotiation of protocolls.
void LoadProtocolStates();

/// @brief Allows to estract from a string at the given position, a telnet command.
/// @param source  The source string.
/// @param index   The index from which the command has to be extracted.
/// @param command Where the extracted command has to be stored.
/// @param buffer  The buffer where the string version of the command will be store.
/// @return <b>True</b> if the function extracts a valid command,<br>
///         <b>False</b> otherwise.
bool ExtractCommand(const std::string & source, size_t & index, TelnetChar & command, std::string & buffer);

/// @brief Function for the protocol negotiation with the client.
/// @param character The character with which the protocol is negotiated.
/// @param nextState The state which identifies the stage of protocol negotiation.
void NegotiateProtocol(Character * character, const ConnectionState & nextState);

/// @brief Input process function for handling telnet commands.
/// @param character The character with which the protocol is negotiated.
/// @param sArgs     The input stream containing from the character.
void ProcessTelnetCommand(Character * character, std::istream & sArgs);
