/// @file   msdp.cpp
/// @brief  Implement the negotiation functions for MUD Server Data Protocol.
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

void LoadMSDPStates()
{
    Mud::instance().addStateAction(ConnectionState::NegotiatingMSDP, ProcessMSDP);
}

void PrintCommands(const std::string & source)
{
    for (size_t index = 0; index < source.size(); ++index)
    {
        if (source[index] == static_cast<char>(TelnetChar::Echo))
        {
            Logger::log(LogLevel::Debug, "ECHO");
        }
        else if (source[index] == static_cast<char>(TelnetChar::SuppressGoAhead))
        {
            Logger::log(LogLevel::Debug, "SuppressGoAhead");
        }
        else if (source[index] == static_cast<char>(TelnetChar::Status))
        {
            Logger::log(LogLevel::Debug, "Status");
        }
        else if (source[index] == static_cast<char>(TelnetChar::TimingMark))
        {
            Logger::log(LogLevel::Debug, "TimingMark");
        }
        else if (source[index] == static_cast<char>(TelnetChar::TerminalType))
        {
            Logger::log(LogLevel::Debug, "TerminalType");
        }
        else if (source[index] == static_cast<char>(TelnetChar::NegotiateAboutWindowSize))
        {
            Logger::log(LogLevel::Debug, "NegotiateAboutWindowSize");
        }
        else if (source[index] == static_cast<char>(TelnetChar::TerminalSpeed))
        {
            Logger::log(LogLevel::Debug, "TerminalSpeed");
        }
        else if (source[index] == static_cast<char>(TelnetChar::RemoteFlowControl))
        {
            Logger::log(LogLevel::Debug, "RemoteFlowControl");
        }
        else if (source[index] == static_cast<char>(TelnetChar::LineMode))
        {
            Logger::log(LogLevel::Debug, "LineMode");
        }
        else if (source[index] == static_cast<char>(TelnetChar::EnvironmentVariables))
        {
            Logger::log(LogLevel::Debug, "EnvironmentVariables");
        }
        else if (source[index] == static_cast<char>(TelnetChar::NewEnvironmentOption))
        {
            Logger::log(LogLevel::Debug, "NewEnvironmentOption");
        }
        else if (source[index] == static_cast<char>(TelnetChar::TTYPE))
        {
            Logger::log(LogLevel::Debug, "TTYPE");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP))
        {
            Logger::log(LogLevel::Debug, "MSDP");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP_VAR))
        {
            Logger::log(LogLevel::Debug, "MSDP_VAR");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP_VAL))
        {
            Logger::log(LogLevel::Debug, "MSDP_VAL");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP_TABLE_OPEN))
        {
            Logger::log(LogLevel::Debug, "MSDP_TABLE_OPEN");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP_TABLE_CLOSE))
        {
            Logger::log(LogLevel::Debug, "MSDP_TABLE_CLOSE");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP_ARRAY_OPEN))
        {
            Logger::log(LogLevel::Debug, "MSDP_ARRAY_OPEN");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSDP_ARRAY_CLOSE))
        {
            Logger::log(LogLevel::Debug, "MSDP_ARRAY_CLOSE");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MSSP))
        {
            Logger::log(LogLevel::Debug, "MSSP");
        }
        else if (source[index] == static_cast<char>(TelnetChar::MCCP))
        {
            Logger::log(LogLevel::Debug, "MCCP");
        }
        else if (source[index] == static_cast<char>(TelnetChar::SubNegotiationEnd))
        {
            Logger::log(LogLevel::Debug, "SubNegotiationEnd");
        }
        else if (source[index] == static_cast<char>(TelnetChar::NoOperation))
        {
            Logger::log(LogLevel::Debug, "NoOperation");
        }
        else if (source[index] == static_cast<char>(TelnetChar::DataMark))
        {
            Logger::log(LogLevel::Debug, "DataMark");
        }
        else if (source[index] == static_cast<char>(TelnetChar::Break))
        {
            Logger::log(LogLevel::Debug, "Break");
        }
        else if (source[index] == static_cast<char>(TelnetChar::InterruptProcess))
        {
            Logger::log(LogLevel::Debug, "InterruptProcess");
        }
        else if (source[index] == static_cast<char>(TelnetChar::AbortOutput))
        {
            Logger::log(LogLevel::Debug, "AbortOutput");
        }
        else if (source[index] == static_cast<char>(TelnetChar::AreYouThere))
        {
            Logger::log(LogLevel::Debug, "AreYouThere");
        }
        else if (source[index] == static_cast<char>(TelnetChar::EraseCharacter))
        {
            Logger::log(LogLevel::Debug, "EraseCharacter");
        }
        else if (source[index] == static_cast<char>(TelnetChar::EraseLine))
        {
            Logger::log(LogLevel::Debug, "EraseLine");
        }
        else if (source[index] == static_cast<char>(TelnetChar::GoAhead))
        {
            Logger::log(LogLevel::Debug, "GoAhead");
        }
        else if (source[index] == static_cast<char>(TelnetChar::SubnegotiationBegin))
        {
            Logger::log(LogLevel::Debug, "SubnegotiationBegin");
        }
        else if (source[index] == static_cast<char>(TelnetChar::WILL))
        {
            Logger::log(LogLevel::Debug, "WILL");
        }
        else if (source[index] == static_cast<char>(TelnetChar::WONT))
        {
            Logger::log(LogLevel::Debug, "WONT");
        }
        else if (source[index] == static_cast<char>(TelnetChar::DO))
        {
            Logger::log(LogLevel::Debug, "DO");
        }
        else if (source[index] == static_cast<char>(TelnetChar::DONT))
        {
            Logger::log(LogLevel::Debug, "DONT");
        }
        else if (source[index] == static_cast<char>(TelnetChar::IAC))
        {
            Logger::log(LogLevel::Debug, "IAC");
        }
        else if (source[index] == static_cast<char>(TelnetChar::DRAW_MAP))
        {
            Logger::log(LogLevel::Debug, "MDRAW_MAPSDP");
        }
        else if (source[index] == static_cast<char>(TelnetChar::CLR_MAP))
        {
            Logger::log(LogLevel::Debug, "CLR_MAP");
        }
        else if (source[index] == static_cast<char>(TelnetChar::FORMAT))
        {
            Logger::log(LogLevel::Debug, "FORMAT");
        }
        else
        {
            Logger::log(LogLevel::Debug, ToString(static_cast<int>(source[index])));
        }
    }
}

void NegotiateMSDP(Character * character)
{
    Player * player = character->toPlayer();

    if (player->connection_state == ConnectionState::NoState)
    {
        Logger::log(LogLevel::Debug, "Sending : IAC + WILL + MSDP;");
        // IAC WILL MSDP
        std::string command;
        command += static_cast<char>(TelnetChar::IAC);
        command += static_cast<char>(TelnetChar::WILL);
        command += static_cast<char>(TelnetChar::MSDP);
        player->sendMsg(command);
        player->connection_state = ConnectionState::NegotiatingMSDP;
    }
}

void ProcessMSDP(Character * character, std::istream & sArgs)
{
    (void) character;
    (void) sArgs;
    std::string line;
    std::getline(sArgs, line);
    PrintCommands(line.substr(0, line.size()));
    for (size_t index = 0; index < line.size(); ++index)
    {
        std::string cmd;
        if (line[index] == static_cast<char>(TelnetChar::IAC))
        {
            // Set what we've read.
            cmd += "IAC ";
            // Increment the index.
            index += 1;
            // Check the character.
            if (line[index] == static_cast<char>(TelnetChar::DO))
            {
                // Set what we've read.
                cmd += "DO ";
                // Increment the index.
                index += 1;
                // Check the character.
                if (line[index] == static_cast<char>(TelnetChar::MSDP))
                {
                    // Set what we've read.
                    cmd += "MSDP";
                }
            }
            else if (line[index] == static_cast<char>(TelnetChar::DONT))
            {
                // Set what we've read.
                cmd += "DONT ";
                // Increment the index.
                index += 1;
                // Check the character.
                if (line[index] == static_cast<char>(TelnetChar::MSDP))
                {
                    // Set what we've read.
                    cmd += "MSDP";
                }
            }
            else if (line[index] == static_cast<char>(TelnetChar::SubnegotiationBegin))
            {
                // Set what we've read.
                cmd += "SB ";
                // Increment the index.
                index += 1;
                // Check the character.
                if (line[index] == static_cast<char>(TelnetChar::MSDP))
                {
                    // Set what we've read.
                    cmd += "MSDP ";
                    // Increment the index.
                    index += 1;
                    // Check the character.
                    if (line[index] == static_cast<char>(TelnetChar::MSDP_VAR))
                    {
                        // Set what we've read.
                        cmd += "MSDP_VAR ";
                        // Increment the index.
                        index += 1;
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
                        cmd += "\"" + variableName + "\" ";
                        cmd += "MSDP_VAL ";
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
                        cmd += "\"" + variableValue + "\" ";
                        cmd += "IAC ";
                        // Increment the index.
                        index += variableValue.size();
                    }
                }
            }
            Logger::log(LogLevel::Debug, "Received: " + cmd + ";");
        }
    }
    AdvanceCharacterCreation(character, ConnectionState::AwaitingName);
}
