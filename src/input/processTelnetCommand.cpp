/// @file   processTelnetCommand.cpp
/// @brief  Implement the protocol negotiation functions.
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

#include "processTelnetCommand.hpp"
#include "processPlayerName.hpp"
#include "mud.hpp"

bool ExtractCommand(
    const std::string & source,
    size_t & index,
    TelnetChar & command,
    std::string & buffer)
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
    if (TelnetCharTest::is_value(source.at(index)))
    {
        // Retrieve the command.
        command = static_cast<TelnetChar>(static_cast<unsigned char>(source.at(index)));
        // Add the command to the buffer.
        buffer += GetTelnetCharName(command) + " ";
        // Increment the index.
        index++;
    }
    else
    {
        command = TelnetChar::None;
    }
    return true;
}

void NegotiateProtocol(Character * character, const ConnectionState & nextState)
{
    // Cast the character to player.
    auto player = character->toPlayer();
    // Set its next state.
    player->connectionState = nextState;
    if (nextState == ConnectionState::NegotiatingMSDP)
    {
        Logger::log(LogLevel::Debug, "Sending  : IAC WONT MSDP");
        // IAC WILL MSDP
        std::string command;
        command += static_cast<char>(TelnetChar::IAC);
        command += static_cast<char>(TelnetChar::WONT);
        command += static_cast<char>(TelnetChar::MSDP);
        player->sendMsg(command);
    }
    else if (nextState == ConnectionState::NegotiatingMCCP)
    {
        Logger::log(LogLevel::Debug, "Sending  : IAC WONT MCCP");
        // IAC WILL MCCP
        std::string command;
        command += static_cast<char>(TelnetChar::IAC);
        command += static_cast<char>(TelnetChar::WONT);
        command += static_cast<char>(TelnetChar::MCCP);
        player->sendMsg(command);
    }
}

void ProcessTelnetCommand::process(Character * character, ArgumentHandler & args)
{
    Logger::log(LogLevel::Debug, "ProcessTelnetCommand");
    // Cast the character to player.
    auto player = character->toPlayer();
    // Line which will contain the answer to IAC+WILL+MSDP
    std::string source = args.getOriginal();
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
                Logger::log(
                    LogLevel::Debug,
                    "Content '%s'.",
                    ToString(static_cast<int>(source[index])));
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
                if (player->connectionState == ConnectionState::NegotiatingMSDP)
                {
                    SetFlag(&player->connectionFlags, ConnectionFlag::UseMSDP);
                    Logger::log(LogLevel::Debug, "Positive acknowledgments for MSDP (%s).", result);
                }
                else if (player->connectionState == ConnectionState::NegotiatingMCCP)
                {
                    SetFlag(&player->connectionFlags, ConnectionFlag::UseMCCP);
                    Logger::log(LogLevel::Debug, "Positive acknowledgments for MCCP (%s).", result);
                }
                break;
            }
            if (command == TelnetChar::MSDP)
            {
                SetFlag(&player->connectionFlags, ConnectionFlag::UseMSDP);
                Logger::log(LogLevel::Debug, "Positive acknowledgments for MSDP (%s).", result);
                continue;
            }
            else if (command == TelnetChar::MCCP)
            {
                SetFlag(&player->connectionFlags, ConnectionFlag::UseMCCP);
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
                        if (source[indexVal] == static_cast<char>(TelnetChar::MSDP_VAL))
                        {
                            break;
                        }
                        variableName += source[indexVal];
                    }
                    result += "\"" + variableName + "\" ";
                    result += "MSDP_VAL ";
                    // Increment the index.
                    index += variableName.size() + 1;
                    std::string variableValue;
                    for (size_t indexVal = index; indexVal < source.size(); ++indexVal)
                    {
                        if (source[indexVal] == static_cast<char>(TelnetChar::IAC))
                        {
                            break;
                        }
                        variableValue += source[indexVal];
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
    if (player->connectionState == ConnectionState::NegotiatingMSDP)
    {
        // Activate the procedure of negotiation.
        NegotiateProtocol(character, ConnectionState::NegotiatingMCCP);
    }
    // Create a shared pointer to the next step.
    std::shared_ptr<ProcessPlayerName> newStep = std::make_shared<ProcessPlayerName>();
    // Set the handler.
    player->inputProcessor = newStep;
    // Advance to the next step.
    newStep->advance(character);
}
