/// @file   player.cpp
/// @brief  Implements player methods.
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

// Basic Include.
#include "player.hpp"

#include <stddef.h>

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <iostream>

// Other Include.
#include "mud.hpp"
#include "race.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "telnet.hpp"
#include "updater.hpp"
#include "faction.hpp"
#include "commands.hpp"
#include "constants.hpp"

using namespace std;

Player::Player(const int & _socket, const int & _port, const std::string & _address) :
        psocket(_socket),
        port(_port),
        address(_address),
        outbuf(),
        inbuf(),
        password(),
        age(),
        experience(),
        prompt(),
        prompt_save(),
        rent_room(),
        skills(),
        remaining_points(),
        connection_state(),
        password_attempts(),
        closing(),
        logged_in()
{
    // Nothing to do.
}

Player::~Player()
{
    Logger::log(LogLevel::Debug, "Deleted: Player (" + this->getNameCapital() + ").");

    // Send the last values still in the outbuffer.
    this->processWrite();

    // Close connection if active.
    if (this->checkConnection())
    {
        if (!Mud::instance().closeSocket(psocket))
        {
            Logger::log(LogLevel::Error, "Something has gone wrong during player socket closure.");
        }
    }
    for (auto iterator : inventory)
    {
        iterator->owner = nullptr;
    }
    for (auto iterator : equipment)
    {
        iterator->owner = nullptr;
    }
    // Remove the player from the room.
    if (room != nullptr)
    {
        room->removeCharacter(this);
    }
}

bool Player::check()
{
    bool safe = Character::check();
    safe &= SafeAssert(psocket > 0);
    safe &= SafeAssert(port > 0);
    safe &= SafeAssert(!address.empty());
    safe &= SafeAssert(!password.empty());
    safe &= SafeAssert(age > 0);
    safe &= SafeAssert(experience >= 0);
    safe &= SafeAssert(!prompt.empty());
    safe &= SafeAssert(!prompt_save.empty());
    safe &= SafeAssert(rent_room >= 0);
    safe &= SafeAssert(connection_state != ConnectionState::NoState);
    //safe &= SafeAssert(!skills.empty());
    //safe &= SafeAssert(skills.size() == Mud::instance().mudSkills.size());
    for (auto iterator : Mud::instance().mudSkills)
    {
        std::map<int, unsigned int>::iterator iterator2 = skills.find(iterator.first);
        if (iterator2 == skills.end())
        {
            skills.insert(std::make_pair(iterator.first, 1));
        }
    }
    safe &= SafeAssert(password_attempts >= 0);
    safe &= SafeAssert(!closing);
    safe &= SafeAssert(!logged_in);
    return safe;
}

bool Player::isPlayer()
{
    return true;
}

///////////////////////////////////////////////////////////
// CONNECTIONI ////////////////////////////////////////////
///////////////////////////////////////////////////////////

int Player::getSocket() const
{
    return psocket;
}

string Player::getAddress() const
{
    return address;
}

bool Player::checkConnection() const
{
    return Mud::instance().checkSocket(psocket);
}

void Player::closeConnection()
{
    closing = true;
}

bool Player::isPlaying() const
{
    return checkConnection() && (connection_state == ConnectionState::Playing) && (!closing) && logged_in;
}

bool Player::hasPendingOutput() const
{
    return !outbuf.empty();
}

///////////////////////////////////////////////////////////
// DATABASE ///////////////////////////////////////////////
///////////////////////////////////////////////////////////

bool Player::createOnDB()
{
    vector<string> arguments;
    // Prepare the arguments of the query.
    arguments.push_back(name);
    arguments.push_back(password);
    arguments.push_back(ToString(race->vnum));
    arguments.push_back(ToString(strength));
    arguments.push_back(ToString(agility));
    arguments.push_back(ToString(perception));
    arguments.push_back(ToString(constitution));
    arguments.push_back(ToString(intelligence));
    arguments.push_back(ToString(sex));
    arguments.push_back(ToString(age));
    arguments.push_back(description);
    arguments.push_back(ToString(weight));
    arguments.push_back(ToString(faction->vnum));
    arguments.push_back(ToString(level));
    arguments.push_back(ToString(experience));
    arguments.push_back(ToString(room->vnum));
    arguments.push_back(prompt);
    arguments.push_back(ToString(flags));
    arguments.push_back(ToString(health));
    arguments.push_back(ToString(stamina));
    arguments.push_back(ToString(hunger));
    arguments.push_back(ToString(thirst));
    arguments.push_back(ToString(rent_room));
    if (!SQLiteDbms::instance().insertInto("Player", arguments))
    {
        Logger::log(LogLevel::Error, "Something gone wrong during player creation on database.");
        return false;
    }

    // Prepare the arguments of the query for skill table.
    for (auto iterator : skills)
    {
        arguments.clear();
        arguments.push_back(name);
        arguments.push_back(ToString(iterator.first));
        arguments.push_back(ToString(iterator.second));
        if (!SQLiteDbms::instance().insertInto("Advancement", arguments))
        {
            Logger::log(LogLevel::Error, "Something gone wrong during player Skill creation on database.");
            return false;
        }
    }
    return true;
}

bool Player::loadFromDB()
{
    return SQLiteDbms::instance().loadPlayer(this);
}

bool Player::updateOnDB()
{
    if (connection_state == ConnectionState::Playing)
    {
        QueryList value, where;
        value.push_back(std::make_pair("name", name));
        value.push_back(std::make_pair("password", password));
        value.push_back(std::make_pair("race", ToString(race->vnum)));
        value.push_back(std::make_pair("str", ToString(strength)));
        value.push_back(std::make_pair("agi", ToString(agility)));
        value.push_back(std::make_pair("per", ToString(perception)));
        value.push_back(std::make_pair("con", ToString(constitution)));
        value.push_back(std::make_pair("int", ToString(intelligence)));
        value.push_back(std::make_pair("sex", ToString(sex)));
        value.push_back(std::make_pair("age", ToString(age)));
        value.push_back(std::make_pair("description", description));
        value.push_back(std::make_pair("weight", ToString(weight)));
        value.push_back(std::make_pair("faction", ToString(faction->vnum)));
        value.push_back(std::make_pair("level", ToString(level)));
        value.push_back(std::make_pair("experience", ToString(experience)));
        value.push_back(std::make_pair("position", ToString(room->vnum)));
        value.push_back(std::make_pair("prompt", prompt));
        value.push_back(std::make_pair("flag", ToString(flags)));
        value.push_back(std::make_pair("health", ToString(health)));
        value.push_back(std::make_pair("stamina", ToString(stamina)));
        value.push_back(std::make_pair("rent_room", ToString(rent_room)));
        value.push_back(std::make_pair("hunger", ToString(hunger)));
        value.push_back(std::make_pair("thirst", ToString(thirst)));
        where.push_back(std::make_pair("name", name));

        if (!SQLiteDbms::instance().updateInto("Player", value, where))
        {
            Logger::log(LogLevel::Error, "Error during Player's Information save.");
            return false;
        }
        for (auto iterator : inventory)
        {
            if (!iterator->updateOnDB())
            {
                Logger::log(LogLevel::Error, "Error during Player's Inventory save.");
                return false;
            }
        }
        for (auto iterator : equipment)
        {
            if (!iterator->updateOnDB())
            {
                Logger::log(LogLevel::Error, "Error during Player's Equipment save.");
                return false;
            }
        }
        for (auto iterator : skills)
        {
            vector<string> arguments;
            arguments.push_back(name);
            arguments.push_back(ToString(iterator.first));
            arguments.push_back(ToString(iterator.second));

            if (!SQLiteDbms::instance().insertInto("Advancement", arguments, false, true))
            {
                Logger::log(LogLevel::Error, "Error during Player's Skills save.");
                return false;
            }
        }
    }
    return true;
}

///////////////////////////////////////////////////////////
// INTERFACE //////////////////////////////////////////////
///////////////////////////////////////////////////////////

void Player::sendPrompt()
{
    string readyPrompt = prompt + "\n";
    FindAndReplace(readyPrompt, "&n", ToLower(name));
    FindAndReplace(readyPrompt, "&N", name);
    FindAndReplace(readyPrompt, "&h", ToString(health));
    FindAndReplace(readyPrompt, "&H", ToString(getMaxHealth()));
    FindAndReplace(readyPrompt, "&s", ToString(stamina));
    FindAndReplace(readyPrompt, "&S", ToString(getMaxStamina()));
    // Send to the player his prompt.
    if (!closing)
    {
        this->sendMsg(readyPrompt);
    }
}

///////////////////////////////////////////////////////////
// INVENTORY //////////////////////////////////////////////
///////////////////////////////////////////////////////////

bool Player::remInventoryItem(Item * item)
{
    if (Character::remInventoryItem(item))
    {
        SQLiteDbms::instance().deleteFrom("ItemPlayer",
        { std::make_pair("owner", name), std::make_pair("item", ToString(item->vnum)) });
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////
// EQUIPMENT //////////////////////////////////////////////
///////////////////////////////////////////////////////////

bool Player::remEquipmentItem(Item * item)
{
    if (Character::remEquipmentItem(item))
    {
        SQLiteDbms::instance().deleteFrom("ItemPlayer",
        { std::make_pair("owner", name), std::make_pair("item", ToString(item->vnum)) });
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////
// DEATH //////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void Player::triggerDeath()
{
    // Call the method of the father class.
    Character::triggerDeath();

    // Move the player to the default room.
    if ((room = Mud::instance().findRoom(rent_room)) != nullptr)
    {
        room->addCharacter(this);
    }

    // Set values of health and stamina to 1.
    health = 1;
    stamina = 1;
}

///////////////////////////////////////////////////////////
// COMMUNICATION //////////////////////////////////////////
///////////////////////////////////////////////////////////

void Player::enterGame()
{
    this->sendMsg(Telnet::clearScreen());
    map<string, string>::reverse_iterator it;

    // Greet them.
    this->sendMsg(Telnet::bold() + "Welcome, " + name + "!\n" + Telnet::reset());

    // Load the news.
    this->sendMsg("#---------------- Global News ----------------#\n");
    for (it = Mud::instance().mudNews.rbegin(); it != Mud::instance().mudNews.rend(); ++it)
    {
        this->sendMsg("Date :" + it->first + "\n");
        this->sendMsg(it->second + "\n");
    }
    this->sendMsg("#---------------------------------------------#\n\n");
    this->sendMsg("You walked through the mist and came into the world...\n\n");

    // Notice all the players in the same room.
    if (room != nullptr)
    {
        room->addCharacter(this);
        room->sendToAll(name + " appears.\n", this);
    }
    else
    {
        closeConnection();
    }

    // Set the player as logged in.
    logged_in = true;

    // New player looks around.
    doCommand("look");
}

void Player::processInput(Player * player, const string & command)
{
    ActionHandler action = Mud::instance().findStateAction(player->connection_state);
    std::istringstream is(command);

    try
    {
        action(player, is);
    }
    catch (std::runtime_error & e)
    {
        sendMsg(std::string(e.what()) + "\n");
    }
}

void Player::processRead()
{
    vector<char> buffer = vector<char>(8192);

    // Once closed, don't handle any pending input.
    if (closing)
    {
        return;
    }

    ssize_t nRead = recv(psocket, &buffer[0], sizeof(buffer), MSG_DONTWAIT);
    if (nRead <= 0)
    {
        // Print error.
        perror("Error during socket receive");
        // Close the socket.
        if (Mud::instance().closeSocket(psocket))
        {
            Logger::log(LogLevel::Error, "Something has gone wrong during player socket closure.");
        }
        // Log the error.
        Logger::log(LogLevel::Error, "Connection " + ToString(psocket) + " closed.");
        // Clear the socket.
        this->psocket = kNoSocketIndicator;
        // Close the connection.
        this->closeConnection();
        // Skip the rest of the function.
        return;
    }

    // Add to input buffer.
    inbuf += string(&buffer[0], nRead);

    // Update received data.
    MudUpdater::instance().updateBandIn(nRead);

    size_t iStart = inbuf.size();
    // Try to extract lines from the input buffer.
    while (true)
    {
        string::size_type i = inbuf.find('\n');
        if (i == string::npos)
        {
            // No more at present.
            break;
        }

        // Extract first line.
        std::string sLine = inbuf.substr(0, i);

        // Get rest of string.
        inbuf = inbuf.substr(i + 1, string::npos);

        iStart += i + 1;

        // Now, process it.
        this->processInput(this, Trim(sLine));
    }
    inbuf[iStart] = '\0';
}

void Player::processWrite()
{
    std::vector<unsigned char> uncompressed;
    std::vector<unsigned char> compressed;
    //std::vector<unsigned char> check;
    // We will loop attempting to write all in buffer, until write blocks.
    while (psocket != kNoSocketIndicator && !outbuf.empty())
    {
        // We attach to the message the player prompt.
        this->sendPrompt();

        // For portability I replace \n with \r\n.
        FindAndReplace(outbuf, "\n", "\r\n");

        // Send a maximum of 4096 at a time.
        size_t iLength = std::min<size_t>(outbuf.size(), kMaxStream);

#if 0
        // Compress the data.
        for (unsigned int i = 0; i < outbuf.size(); i++)
        uncompressed.push_back(outbuf[i]);

        // Create compressed Buffer from input data.
        DeflateStream(uncompressed, compressed);

        // Send to player.
        int nWrite = send(psocket, (const char *) compressed.data(), compressed.size(), 0);

        // Update sent data.
        Mud::instance().getUpdater().updateBandWidth(1, compressed.size());

        // Update uncompressed data.
        Mud::instance().getUpdater().updateBandWidth(2, uncompressed.size());
#else
        // Send to player.
        size_t nWrite = send(psocket, outbuf.c_str(), outbuf.size(), MSG_NOSIGNAL);
#endif
#if 0
        // Decompress the sent data for checking.
        InflateStream(compressed, check);
        // Check the sent data.
        if (uncompressed != check)
        {
            LogWarning("Uncrompressed message is different from compressed one.");
        }
#endif

        MudUpdater::instance().updateBandOut(nWrite);

        // Check for bad write.
        if (nWrite <= 0)
        {
            switch (errno)
            {
                case EPIPE:
                    perror("Sending on a closed connection...");
                default:
                    perror("Unknown error during Send...");
            }
            return;
        }

        // Remove what we successfully sent from the buffer.
        outbuf.erase(0, outbuf.size());

        // If partial write, exit
        if (nWrite < iLength)
        {
            break;
        }
    }
}

void Player::processException()
{
    // Signals can cause exceptions, don't get too excited. :)
    std::cerr << "Exception on socket " << psocket << std::endl << std::endl;
}

void Player::sendMsg(const std::string & message)
{
    outbuf += message;
}
