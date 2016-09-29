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

// Other Include.
#include "../mud.hpp"

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
        connection_state(ConnectionState::NoState),
        password_attempts(),
        closing(),
        logged_in(),
        connectionFlags(),
        msdpVariables()
{
    // Nothing to do.
}

Player::~Player()
{
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
    // Unlink the inventory items.
    for (auto iterator : inventory)
    {
        iterator->owner = nullptr;
    }
    // Unlink the equipment items.
    for (auto iterator : equipment)
    {
        iterator->owner = nullptr;
    }
    // Remove the player from the room.
    if (room != nullptr)
    {
        room->removeCharacter(this);
    }
    Logger::log(LogLevel::Debug, "Deleted player\t\t\t\t(%s)", this->getNameCapital());
}

bool Player::check() const
{
    bool safe = Character::check();
    safe &= CorrectAssert(psocket > 0);
    safe &= CorrectAssert(port > 0);
    safe &= CorrectAssert(!address.empty());
    safe &= CorrectAssert(!password.empty());
    safe &= CorrectAssert(age > 0);
    safe &= CorrectAssert(experience >= 0);
    safe &= CorrectAssert(!prompt.empty());
    safe &= CorrectAssert(!prompt_save.empty());
    safe &= CorrectAssert(rent_room >= 0);
    safe &= CorrectAssert(connection_state != ConnectionState::NoState);
    //safe &= CorrectAssert(!skills.empty());
    //safe &= CorrectAssert(skills.size() == Mud::instance().mudSkills.size());
    for (auto iterator : Mud::instance().mudSkills)
    {
        std::map<int, unsigned int>::const_iterator iterator2 = skills.find(iterator.first);
        if (iterator2 == skills.end())
        {
            Logger::log(LogLevel::Error, "The skill %s has been set.", iterator.second->name);
            safe &= false;
        }
    }
    safe &= CorrectAssert(password_attempts >= 0);
    safe &= CorrectAssert(!closing);
    safe &= CorrectAssert(!logged_in);
    return safe;
}

bool Player::isPlayer() const
{
    return true;
}

void Player::getSheet(Table & sheet) const
{
    // Call the function of the father class.
    Character::getSheet(sheet);
    // Add a divider.
    sheet.addDivider();
    // Set the values.
    sheet.addRow( { "Age", ToString(this->age) });
    sheet.addRow( { "Experience", ToString(this->experience) });
    sheet.addRow( { "Prompt", this->prompt });
    sheet.addRow( { "Rent Room", ToString(this->rent_room) });
    sheet.addDivider();
    sheet.addRow( { "## Skill", "## Points" });
    for (auto it : Mud::instance().mudSkills)
    {
        if (this->skills.find(it.first) != this->skills.end())
        {
            sheet.addRow( { it.second->name, ToString(this->skills.at(it.first)) });
        }
        else
        {
            sheet.addRow( { it.second->name, "0" });
        }
    }
}

void Player::addInventoryItem(Item * & item)
{
    Character::addInventoryItem(item);
    // Update on database.
    SQLiteDbms::instance().insertInto(
        "ItemPlayer",
        { name, ToString(item->vnum), EnumToString(EquipmentSlot::None) },
        false,
        true);
}

void Player::addEquipmentItem(Item * & item)
{
    Character::addEquipmentItem(item);
    // Update on database.
    SQLiteDbms::instance().insertInto(
        "ItemPlayer",
        { name, ToString(item->vnum), EnumToString(item->getCurrentSlot()) },
        false,
        true);
}

bool Player::remInventoryItem(Item * item)
{
    if (Character::remInventoryItem(item))
    {
        // Update on database.
        SQLiteDbms::instance().deleteFrom(
            "ItemPlayer",
            { std::make_pair("item", ToString(item->vnum)) });
        return true;
    }
    return false;
}

bool Player::remEquipmentItem(Item * item)
{
    if (Character::remEquipmentItem(item))
    {
        // Update on database.
        SQLiteDbms::instance().deleteFrom(
            "ItemPlayer",
            { std::make_pair("item", ToString(item->vnum)) });
        return true;
    }
    return false;
}

int Player::getSocket() const
{
    return psocket;
}

std::string Player::getAddress() const
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
    return checkConnection() && (connection_state == ConnectionState::Playing) && (!closing)
        && logged_in;
}

bool Player::hasPendingOutput() const
{
    return !outbuf.empty();
}

bool Player::updateOnDB()
{
    std::vector<std::string> arguments;
    // Prepare the arguments of the query.
    arguments.push_back(name);
    arguments.push_back(password);
    arguments.push_back(ToString(race->vnum));
    arguments.push_back(ToString(this->getAbility(Ability::Strength, false)));
    arguments.push_back(ToString(this->getAbility(Ability::Agility, false)));
    arguments.push_back(ToString(this->getAbility(Ability::Perception, false)));
    arguments.push_back(ToString(this->getAbility(Ability::Constitution, false)));
    arguments.push_back(ToString(this->getAbility(Ability::Intelligence, false)));
    arguments.push_back(ToString(static_cast<int>(gender)));
    arguments.push_back(ToString(age));
    arguments.push_back(description);
    arguments.push_back(ToString(weight));
    arguments.push_back(ToString(faction->vnum));
    arguments.push_back(ToString(level));
    arguments.push_back(ToString(experience));
    arguments.push_back(ToString(room->vnum));
    arguments.push_back(prompt);
    arguments.push_back(ToString(flags));
    arguments.push_back(ToString(this->getHealth()));
    arguments.push_back(ToString(this->getStamina()));
    arguments.push_back(ToString(this->getHunger()));
    arguments.push_back(ToString(this->getThirst()));
    arguments.push_back(ToString(rent_room));
    if (!SQLiteDbms::instance().insertInto("Player", arguments, false, true))
    {
        Logger::log(LogLevel::Error, "Error during player creation on database.");
        return false;
    }
    // Prepare the arguments of the query for skill table.
    for (auto iterator : skills)
    {
        arguments.clear();
        arguments.push_back(name);
        arguments.push_back(ToString(iterator.first));
        arguments.push_back(ToString(iterator.second));
        if (!SQLiteDbms::instance().insertInto("Advancement", arguments, false, true))
        {
            Logger::log(LogLevel::Error, "Error during player Skill creation on database.");
            return false;
        }
    }
    return true;
}

void Player::sendPrompt()
{
    if (this->isPlaying())
    {
        // Send to the player his prompt.
        if (this->logged_in)
        {
            std::string readyPrompt = prompt + "\n";
            FindAndReplace(readyPrompt, "&n", ToLower(name));
            FindAndReplace(readyPrompt, "&N", name);
            FindAndReplace(readyPrompt, "&h", ToString(this->getHealth()));
            FindAndReplace(readyPrompt, "&H", ToString(this->getMaxHealth()));
            FindAndReplace(readyPrompt, "&s", ToString(this->getStamina()));
            FindAndReplace(readyPrompt, "&S", ToString(this->getMaxStamina()));
            this->sendMsg(readyPrompt);
        }
        else
        {
            this->sendMsg("\n");
        }
    }
}

void Player::kill()
{
    // Call the method of the father class.
    Character::kill();
    // Move the player to the default room.
    if ((room = Mud::instance().findRoom(rent_room)) != nullptr)
    {
        room->addCharacter(this);
    }
    // Set values of health and stamina to 1.
    this->setHealth(1, true);
    this->setStamina(1, true);
}

void Player::enterGame()
{
    this->sendMsg(Formatter::clearScreen());
    // Greet them.
    this->sendMsg(Formatter::bold() + "Welcome, " + name + "!\n" + Formatter::reset());
    // Load the news.
    this->sendMsg("#---------------- Global News ----------------#\n");
    for (auto it = Mud::instance().mudNews.rbegin(); it != Mud::instance().mudNews.rend(); ++it)
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
        // Set the list of exceptions.
        std::vector<Character *> exceptions;
        exceptions.push_back(this);
        // Send the message inside the room.
        room->sendToAll("%s appears.\n", exceptions, name);
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

void Player::processInput(Player * player, const std::string & command)
{
    ActionHandler _action = Mud::instance().findStateAction(player->connection_state);
    try
    {
        ArgumentHandler argumentHandler(command);
        _action(player, argumentHandler);
    }
    catch (std::runtime_error & e)
    {
        sendMsg(std::string(e.what()) + "\n");
    }
}

/// Buffer used for the reading process.
#define BUFSIZE 512

void Player::processRead()
{
    char buffer[BUFSIZE];
    // Once closed, don't handle any pending input.
    if (closing)
    {
        return;
    }
    ssize_t nRead = recv(psocket, &buffer, BUFSIZE - 1, MSG_DONTWAIT);
    if (nRead <= 0)
    {
        Logger::log(LogLevel::Error, "Socket recv failed: %s", ToString(errno));
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
    std::size_t uRead = static_cast<std::size_t>(nRead);
    // Move the received data into the input buffer.
    inbuf = std::string(buffer, uRead);
    // Update received data.
    MudUpdater::instance().updateBandIn(uRead);
    // Process the input.
    this->processInput(this, Trim(inbuf));
    // Null-terminate the buffer.
    inbuf[uRead] = 0;
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
        ssize_t nWrite = send(psocket, outbuf.c_str(), outbuf.size(), MSG_NOSIGNAL);
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

        // Check for bad write.
        if (nWrite <= 0)
        {
            if (errno == EPIPE)
            {
                Logger::log(LogLevel::Error, "Sending on a closed connection...");
            }
            else
            {
                Logger::log(LogLevel::Error, "Unknown error during Send...");
            }
            return;
        }
        std::size_t uWritten = static_cast<std::size_t>(nWrite);
        MudUpdater::instance().updateBandOut(uWritten);

        // Remove what we successfully sent from the buffer.
        outbuf.erase(0, outbuf.size());

        // If partial write, exit
        if (uWritten < iLength)
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

void Player::sendMsg(const std::string & msg)
{
    outbuf += msg;
}
