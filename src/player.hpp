/// @file   player.hpp
/// @brief  Define player variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include <ctime>
#include <list>
#include <map>
#include <set>
#include <string>

#include "character.hpp"
#include "skill.hpp"

/// @brief Holds details about each connected player.
class Player: public Character
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
        /// A backup of the player prompt.
        std::string prompt_save;
        /// The place where the player has slept last time.
        int rent_room;
        /// The player's list of skills.
        std::map<int, unsigned int> skills;
        /// Points that could be spent during character creation.
        int remaining_points;
        /// Connection state.
        ConnectionState connection_state;
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

        /// @brief Constructor.
        /// @param socket    Player socket.
        /// @param port      Player port.
        /// @param address   Player address.
        Player(const int & socket, const int & port, const std::string & address);

        /// @brief Destructor.
        ~Player();

        /// Disable copy constructor.
        Player(const Player&) = delete;

        /// Disable assign operator.
        Player& operator=(const Player&) = delete;

        virtual bool check() const;

        virtual bool isPlayer() const;

        /// @brief Return player socket.
        /// @return Player sockec.
        int getSocket() const;

        /// @brief Return player IP address.
        /// @return Player IP Address.
        std::string getAddress() const;

        /// @brief Check if player is connected.
        /// @return <b>True</b> if player is connected,<br><b>False</b> otherwise.
        bool checkConnection() const;

        /// @brief Close this player's connection.
        void closeConnection();

        /// @brief Check if this player actively playing.
        /// @return <b>True</b> if player is playing,<br><b>False</b> otherwise.
        bool isPlaying() const;

        /// @brief Check if player has pending output.
        /// @return <b>True</b> if we have something to send them,<br><b>False</b> otherwise.
        bool hasPendingOutput() const;

        /// @brief Create an entry for the player in the database.
        bool createOnDB();

        /// @brief Load player from database.
        bool loadFromDB();

        /// @brief Save player to database.
        /// @return <b>True</b> if the update goes well,<br><b>False</b> otherwise.
        bool updateOnDB();

        /// @brief Send the prompt to player.
        void sendPrompt();

        /// @brief Remove the passed item from the player's inventory and update the information on database.
        /// @param item The item to remove from inventory.
        bool remInventoryItem(Item * item);

        /// @brief Remove from current equipment the item, and update the information on database.
        /// @param item The item to remove.
        bool remEquipmentItem(Item * item);

        /// @brief Handle what happend when this player die.
        void kill();

        /// @brief Handle player has entered the game.
        void enterGame();

        /// @brief Process player input - check connection state, and act accordingly.
        /// @param player  Player that submitted input.
        /// @param command Player's input command.
        void processInput(Player* player, const std::string & command);

        /// @brief Get player input.
        void processRead();

        /// @brief Output text to player.
        /// @details
        /// Here when we can send stuff to the player. We are allowing for large
        /// volumes of output that might not be sent all at once, so whatever cannot
        /// go this time gets put into the list of outstanding strings for this player.
        void processWrite();

        /// @brief Handle player exception on socket.
        void processException();

        /// @brief Output to player (any type).
        /// @param msg String to sent.
        void sendMsg(const std::string & msg);
};

/// Player map.
typedef std::set<Player *> PlayerList;
