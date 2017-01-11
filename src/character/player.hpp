/// @file   player.hpp
/// @brief  Define player variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include <ctime>
#include <list>
#include <map>
#include <set>
#include <string>

#include "character.hpp"
#include "skill.hpp"

/// Handle all the player's phases during login.
using ConnectionState = enum class ConnectionState_t
{
    LoggingIn, ///< The player is logging in.
    Playing    ///< The player is playing.
};

/// @brief Holds details about each connected player.
class Player :
    public Character
{
private:
    /// Socket they connected on.
    int psocket;
    /// Port they connected on.
    int port;
    /// Address player is from.
    std::string address;
    /// Pending output.
    std::string outbuf;
    /// Pending input.
    std::string inbuf;

public:
    /// Player password.
    std::string password;
    /// Player age.
    int age;
    /// Player experience points.
    int experience;
    /// The current prompt.
    std::string prompt;
    /// The place where the player has slept last time.
    int rent_room;
    /// The player's list of skills.
    std::vector<std::pair<std::shared_ptr<Skill>, int>> skills;
    /// Points that could be spent during character creation.
    int remaining_points;
    /// Connection state.
    ConnectionState connectionState;
    /// Password guessing attempts.
    int password_attempts;
    /// True if they are about to leave us.
    bool closing;
    /// Determine if the player has successfully logged in.
    bool logged_in;
    /// Connection flags.
    unsigned int connectionFlags;
    /// MSDP Variables.
    std::map<std::string, std::string> msdpVariables;
    /// Lua variables.
    std::map<std::string, std::string> luaVariables;

    /// @brief Constructor.
    /// @param socket    Player socket.
    /// @param port      Player port.
    /// @param address   Player address.
    Player(const int & socket, const int & port, const std::string & address);

    /// @brief Destructor.
    ~Player();

    /// Disable copy constructor.
    Player(const Player &) = delete;

    /// Disable assign operator.
    Player & operator=(const Player &) = delete;

    bool check() const override;

    bool isPlayer() const override;

    void getSheet(Table & sheet) const override;

    std::string getName() const override;

    void addInventoryItem(Item *& item) override;

    void addEquipmentItem(Item *& item) override;

    bool remInventoryItem(Item * item) override;

    bool remEquipmentItem(Item * item) override;

    /// @brief Return player socket.
    /// @return Player sockec.
    int getSocket() const;

    /// @brief Return player IP address.
    /// @return Player IP Address.
    std::string getAddress() const;

    /// @brief Check if player is connected.
    /// @return <b>True</b> if player is connected,<br>
    ///         <b>False</b> otherwise.
    bool checkConnection() const;

    /// @brief Close this player's connection.
    void closeConnection();

    /// @brief Check if this player actively playing.
    /// @return <b>True</b> if player is playing,<br>
    ///         <b>False</b> otherwise.
    bool isPlaying() const;

    /// @brief Check if player has pending output.
    /// @return <b>True</b> if we have something to send them,<br>
    ///         <b>False</b> otherwise.
    bool hasPendingOutput() const;

    /// @brief Create an updated entry for the player inside the database.
    /// @return <b>True</b> if the update goes well,<br>
    ///         <b>False</b> otherwise.
    bool updateOnDB();

    /// @brief Send the prompt to player.
    void sendPrompt();

    /// @brief Handle what happend when this player die.
    void kill() override;

    /// @brief Handle player has entered the game.
    void enterGame();

    /// @brief Get player input.
    void processRead();

    /// @brief Output text to player.
    void processWrite();

    /// @brief Handle player exception on socket.
    void processException();

    /// @brief Output to player (any type).
    /// @param msg String to sent.
    void sendMsg(const std::string & msg) override;

    /// @brief Adds a variable to the list of LUA-Visible variables.
    /// @param variableName  The name of the variable.
    /// @param variableValue The value of the variable.
    void setLuaVariable(std::string variableName, std::string variableValue);

    /// @brief Provides the value of the given LUA-Visible variable.
    std::string getLuaVariable(std::string variableName);

    /// @brief Removes the variable from the list of LUA-Visible variables.
    bool removeLuaVariable(std::string variableName);

protected:
    void updateTicImpl() override;

    void updateHourImpl() override;

};
