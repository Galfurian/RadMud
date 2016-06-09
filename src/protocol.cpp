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
    Mud::instance().addStateAction(ConnectionState::NegotiatingMSDP, ProcessMSDP);
    Mud::instance().addStateAction(ConnectionState::NegotiatingMCCP, ProcessMCCP);
}

bool ExtractCommand(const std::string & source, size_t & index, TelnetChar & command, std::string & buffer)
{
    if (source.empty())
    {
        //Logger::log(LogLevel::Error, "Source empty!");
        return false;
    }
    if (index >= source.size())
    {
        //Logger::log(LogLevel::Error, "Index exceded!");
        return false;
    }
    if (!TelnetCharTest::is_value(source.at(index)))
    {
        //Logger::log(LogLevel::Error, "Character is not valid!");
        return false;
    }
    // Retrieve the command.
    command = static_cast<TelnetChar>(static_cast<unsigned char>(source.at(index)));
    // Add the command to the buffer.
    buffer += GetTelnetCharName(command) + " ";
    // Increment the index.
    index++;
    return true;
}

void NegotiateProtocol(Character * character, const ConnectionState & nextState)
{
    if (!character->isPlayer())
    {
        Logger::log(LogLevel::Error, "Negotiating for a non-player character");
        return;
    }
    // Cast the character to player.
    Player * player = character->toPlayer();
    // Set its next state.
    player->connection_state = nextState;
    if (nextState == ConnectionState::NegotiatingMSDP)
    {
        Logger::log(LogLevel::Debug, "Sending  : IAC WILL MSDP");
        // IAC WILL MSDP
        std::string command;
        command += static_cast<unsigned char>(TelnetChar::IAC);
        command += static_cast<unsigned char>(TelnetChar::WILL);
        command += static_cast<unsigned char>(TelnetChar::MSDP);
        player->sendMsg(command);
    }
    else if (nextState == ConnectionState::NegotiatingMCCP)
    {
        Logger::log(LogLevel::Debug, "Sending  : IAC WILL MCCP");
        // IAC WILL MCCP
        std::string command;
        command += static_cast<unsigned char>(TelnetChar::IAC);
        command += static_cast<unsigned char>(TelnetChar::WILL);
        command += static_cast<unsigned char>(TelnetChar::MCCP);
        player->sendMsg(command);
    }
}

void ProcessMSDP(Character * character, std::istream & sArgs)
{
    // Line which will contain the answer to IAC+WILL+MSDP
    std::string line;
    // Retrieve the answer.
    std::getline(sArgs, line);
    // Get the size of the command.
    size_t lineSize = line.size();
    // Prepare an index.
    size_t index = 0;
    while (index < lineSize)
    {
        std::string result;
        TelnetChar command;
        if (ExtractCommand(line, index, command, result))
        {
            if (command == TelnetChar::IAC)
            {
                if (ExtractCommand(line, index, command, result))
                {
                    if (command == TelnetChar::DO)
                    {
                        if (ExtractCommand(line, index, command, result))
                        {
                            if (command == TelnetChar::MSDP)
                            {
                                if (character->isPlayer())
                                {
                                    // Cast the character to player.
                                    Player * player = character->toPlayer();
                                    SetFlag(player->connectionFlags, ConnectionFlag::UseMSDP);
                                }
                                Logger::log(LogLevel::Debug, "Received : " + result);
                                continue;
                            }
                        }
                        else
                        {
                            Logger::log(LogLevel::Debug, "Received : " + result);
                            continue;
                        }
                    }
                    else if (command == TelnetChar::DONT)
                    {
                        if (ExtractCommand(line, index, command, result))
                        {
                            if (command == TelnetChar::MSDP)
                            {
                                Logger::log(LogLevel::Debug, "Received : " + result);
                                continue;
                            }
                        }
                        else
                        {
                            Logger::log(LogLevel::Debug, "Received : " + result);
                            continue;
                        }
                    }
                    else if (command == TelnetChar::SubnegotiationBegin)
                    {
                        if (ExtractCommand(line, index, command, result))
                        {
                            if (command == TelnetChar::MSDP)
                            {
                                if (ExtractCommand(line, index, command, result))
                                {
                                    if (command == TelnetChar::MSDP_VAR)
                                    {
                                        std::string variableName;
                                        for (size_t indexVal = index; indexVal < line.size(); ++indexVal)
                                        {
                                            if (line[indexVal] != static_cast<char>(TelnetChar::MSDP_VAL))
                                            {
                                                variableName += line[indexVal];
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
                                        for (size_t indexVal = index; indexVal < line.size(); ++indexVal)
                                        {
                                            if (line[indexVal] != static_cast<char>(TelnetChar::IAC))
                                            {
                                                variableValue += line[indexVal];
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
                                        // Add the variable to the player's list of msdp variables.
                                        if (character->isPlayer())
                                        {
                                            // Cast the character to player.
                                            Player * player = character->toPlayer();
                                            player->msdpVariables[variableName] = variableValue;
                                        }
                                        Logger::log(LogLevel::Debug, "Received : " + result);
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        Logger::log(LogLevel::Warning, "Received an unknown character.");
                    }
                }
            }
        }
        // Increment the index.
        ++index;
    }
    // Activate the procedure of negotiation.
    NegotiateProtocol(character, ConnectionState::NegotiatingMCCP);
}

void ProcessMCCP(Character * character, std::istream & sArgs)
{
    // Line which will contain the answer to IAC+WILL+MSDP
    std::string line;
    // Retrieve the answer.
    std::getline(sArgs, line);
    // Get the size of the command.
    size_t lineSize = line.size();
    // Prepare an index.
    size_t index = 0;
    while (index < lineSize)
    {
        std::string result;
        TelnetChar command;
        if (ExtractCommand(line, index, command, result))
        {
            if (command == TelnetChar::IAC)
            {
                if (ExtractCommand(line, index, command, result))
                {
                    if (command == TelnetChar::DO)
                    {
                        if (ExtractCommand(line, index, command, result))
                        {
                            if (command == TelnetChar::MCCP)
                            {
                                if (character->isPlayer())
                                {
                                    // Cast the character to player.
                                    Player * player = character->toPlayer();
                                    SetFlag(player->connectionFlags, ConnectionFlag::UseMCCP);
                                }
                                Logger::log(LogLevel::Debug, "Received : " + result);
                                continue;
                            }
                        }
                        else
                        {
                            Logger::log(LogLevel::Debug, "Received : " + result);
                            continue;
                        }
                    }
                    else if (command == TelnetChar::DONT)
                    {
                        if (ExtractCommand(line, index, command, result))
                        {
                            if (command == TelnetChar::MCCP)
                            {
                                Logger::log(LogLevel::Debug, "Received : " + result);
                                continue;
                            }
                        }
                        else
                        {
                            Logger::log(LogLevel::Debug, "Received : " + result);
                            continue;
                        }
                    }
                }
            }
        }
        // Increment the index.
        ++index;
    }
    // Activate the procedure of login
    AdvanceCharacterCreation(character, ConnectionState::AwaitingName);
}
