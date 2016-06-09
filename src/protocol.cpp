/// @file   protocol.cpp
/// @brief  Implement the protocol negotiation functions.
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

#include "protocol.hpp"
#include "logger.hpp"
#include "mud.hpp"

void LoadProtocolStates()
{
    Mud::instance().addStateAction(ConnectionState::NegotiatingMSDP, ProcessTelnetCommand);
    Mud::instance().addStateAction(ConnectionState::NegotiatingMCCP, ProcessTelnetCommand);
}

bool ExtractCommand(const std::string & source, size_t & index, TelnetChar & command, std::string & buffer)
{
    // Check if the source string is empty.
    if (source.empty())
    {
        return false;
    }
    // Check if we've exceeded the string size.
    if (index >= source.size())
    {
        return false;
    }
    // Check if the character is a valid Telnet Character.
    if (!TelnetCharTest::is_value(source.at(index)))
    {
        command = TelnetChar::None;
    }
    else
    {
        // Retrieve the command.
        command = static_cast<TelnetChar>(static_cast<Byte>(source.at(index)));
        // Add the command to the buffer.
        buffer += GetTelnetCharName(command) + " ";
        // Increment the index.
        index++;
    }
    return true;
}

void NegotiateProtocol(Character * character, const ConnectionState & nextState)
{
    // Cast the character to player.
    Player * player = character->toPlayer();
    // Set its next state.
    player->connection_state = nextState;
    if (nextState == ConnectionState::NegotiatingMSDP)
    {
        Logger::log(LogLevel::Debug, "Sending  : IAC WILL MSDP");
        // IAC WILL MSDP
        std::string command;
        command += static_cast<Byte>(TelnetChar::IAC);
        command += static_cast<Byte>(TelnetChar::WILL);
        command += static_cast<Byte>(TelnetChar::MSDP);
        player->sendMsg(command);
    }
    else if (nextState == ConnectionState::NegotiatingMCCP)
    {
        Logger::log(LogLevel::Debug, "Sending  : IAC WILL MCCP");
        // IAC WILL MCCP
        std::string command;
        command += static_cast<Byte>(TelnetChar::IAC);
        command += static_cast<Byte>(TelnetChar::WILL);
        command += static_cast<Byte>(TelnetChar::MCCP);
        player->sendMsg(command);
    }
}

void ProcessTelnetCommand(Character * character, std::istream & sArgs)
{
    // Cast the character to player.
    Player * player = character->toPlayer();
    // Line which will contain the answer to IAC+WILL+MSDP
    std::string source;
    // Retrieve the answer.
    std::getline(sArgs, source);
    // Get the size of the command.
    size_t sourceSize = source.size();
    // Prepare an index.
    size_t index = 0;
    // Non telnet characters.
    std::string content;
    while (index < sourceSize)
    {
        std::string result;
        TelnetChar command;
        // Keep reading until we find an IAC.
        while (index < sourceSize)
        {
            if (!ExtractCommand(source, index, command, result))
            {
                break;
            }
            if (command == TelnetChar::IAC)
            {
                break;
            }
            else
            {
                result.clear();
                content += source[index];
            }
            index++;
        }
        // Read the next Byte.
        if (!ExtractCommand(source, index, command, result))
        {
            break;
        }
        // If: DO
        if (command == TelnetChar::DO)
        {
            if (!ExtractCommand(source, index, command, result))
            {
                if (player->connection_state == ConnectionState::NegotiatingMSDP)
                {
                    SetFlag(player->connectionFlags, ConnectionFlag::UseMSDP);
                    Logger::log(LogLevel::Debug, "Positive acknowledgments for MSDP (%s).", result);
                }
                else if (player->connection_state == ConnectionState::NegotiatingMCCP)
                {
                    SetFlag(player->connectionFlags, ConnectionFlag::UseMCCP);
                    Logger::log(LogLevel::Debug, "Positive acknowledgments for MCCP (%s).", result);
                }
                break;
            }
            if (command == TelnetChar::MSDP)
            {
                SetFlag(player->connectionFlags, ConnectionFlag::UseMSDP);
                Logger::log(LogLevel::Debug, "Positive acknowledgments for MSDP (%s).", result);
                continue;
            }
            else if (command == TelnetChar::MCCP)
            {
                SetFlag(player->connectionFlags, ConnectionFlag::UseMCCP);
                Logger::log(LogLevel::Debug, "Positive acknowledgments for MCCP (%s).", result);
                continue;
            }
            else if (command == TelnetChar::MSDP_VAR)
            {
                Logger::log(LogLevel::Debug, "Positive acknowledgments for MSDP_VAR (%s).", result);
                continue;
            }
            else
            {
                Logger::log(LogLevel::Warning, "Unknown positive acknowledgments (%s).", result);
            }
        }
        else if (command == TelnetChar::DONT)
        {
            if (!ExtractCommand(source, index, command, result))
            {
                Logger::log(LogLevel::Debug, "Negative acknowledgments (%s).", result);
                break;
            }
            if (command == TelnetChar::MSDP)
            {
                Logger::log(LogLevel::Debug, "Negative acknowledgments for MSDP (%s).", result);
                continue;
            }
            else if (command == TelnetChar::MCCP)
            {
                Logger::log(LogLevel::Debug, "Negative acknowledgments for MCCP (%s).", result);
                continue;
            }
            else
            {
                Logger::log(LogLevel::Warning, "Unknown negative acknowledgments (%s).", result);
            }
        }
        else if (command == TelnetChar::SubnegotiationBegin)
        {
            if (!ExtractCommand(source, index, command, result))
            {
                break;
            }
            if (command == TelnetChar::MSDP)
            {
                if (!ExtractCommand(source, index, command, result))
                {
                    break;
                }
                if (command == TelnetChar::MSDP_VAR)
                {
                    std::string variableName;
                    for (size_t indexVal = index; indexVal < source.size(); ++indexVal)
                    {
                        if (source[indexVal] != static_cast<char>(TelnetChar::MSDP_VAL))
                        {
                            variableName += source[indexVal];
                        }
                        else
                        {
                            break;
                        }
                    }
                    result += "\"" + variableName + "\" ";
                    result += "MSDP_VAL ";
                    // Increment the index.
                    index += variableName.size() + 1;
                    std::string variableValue;
                    for (size_t indexVal = index; indexVal < source.size(); ++indexVal)
                    {
                        if (source[indexVal] != static_cast<char>(TelnetChar::IAC))
                        {
                            variableValue += source[indexVal];
                        }
                        else
                        {
                            break;
                        }
                    }
                    result += "\"" + variableValue + "\" ";
                    result += "IAC";
                    // Increment the index.
                    index += variableValue.size() + 1;
                    player->msdpVariables[variableName] = variableValue;
                    Logger::log(LogLevel::Debug, "Negotiated (%s).", result);
                }
            }
        }
        else if (command == TelnetChar::WILL)
        {
            Logger::log(LogLevel::Debug, "Positive acknowledgments (%s).", result);
        }
        else if (command == TelnetChar::WONT)
        {
            Logger::log(LogLevel::Debug, "Positive acknowledgments (%s).", result);
        }
        index++;
    }
    if (player->connection_state == ConnectionState::NegotiatingMSDP)
    {
        // Activate the procedure of negotiation.
        NegotiateProtocol(character, ConnectionState::NegotiatingMCCP);
    }
    else if (player->connection_state == ConnectionState::NegotiatingMCCP)
    {
        // Activate the procedure of login
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName);
    }
    else
    {
        // Activate the procedure of login
        AdvanceCharacterCreation(character, ConnectionState::AwaitingName);
    }
}
