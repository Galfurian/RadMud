/// @file   player.cpp
/// @brief  Implements player methods.
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

#include "character/player.hpp"

#include "database/sqliteWriteFunctions.hpp"
#include "utilities/formatter.hpp"
#include "mud.hpp"

Player::Player(const int &_socket, const int &_port,
			   const std::string &_address) :
	psocket(_socket),
	port(_port),
	address(_address),
	inbuf(),
	password(),
	age(),
	experience(),
	prompt(),
	rent_room(),
	remaining_points(),
	connectionState(ConnectionState::LoggingIn),
	password_attempts(),
	closing(),
	logged_in(),
	connectionFlags(),
	msdpVariables(),
	luaVariables()
{
	// Nothing to do.
}

Player::~Player()
{
	// Send the last values still in the outbuffer.
	this->processWrite();

	// Close connection if active.
	if (this->checkConnection()) {
		if (!Mud::instance().closeSocket(psocket)) {
			MudLog(LogLevel::Error, "Error during player socket closure.");
		}
	}
	// Unlink the inventory items.
	for (auto iterator : inventory) {
		iterator->owner = nullptr;
	}
	// Unlink the equipment items.
	for (auto iterator : equipment) {
		iterator->owner = nullptr;
	}
	// Remove the player from the room.
	if (room != nullptr) {
		room->removeCharacter(this);
	}
	//MudLog(LogLevel::Debug, "Deleted player\t\t\t\t(%s)", this->getNameCapital());
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
	safe &= CorrectAssert(password_attempts >= 0);
	safe &= CorrectAssert(!closing);
	safe &= CorrectAssert(!logged_in);
	return safe;
}

bool Player::isPlayer() const
{
	return true;
}

void Player::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	Character::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Age", ToString(this->age) });
	sheet.addRow({ "Experience", ToString(this->experience) });
	sheet.addRow({ "Prompt", this->prompt });
	sheet.addRow({ "Rent Room", ToString(this->rent_room) });
}

std::string Player::getName() const
{
	return ToLower(name);
}

void Player::addInventoryItem(Item *&item)
{
	Character::addInventoryItem(item);
	// Update on database.
	if (item->getType() != ModelType::Corpse) {
		SaveItemPlayer(this, item, 0);
	}
}

void Player::addEquipmentItem(Item *&item)
{
	Character::addEquipmentItem(item);
	// Update on database.
	if (item->getType() != ModelType::Corpse) {
		for (auto bodyPart : item->occupiedBodyParts) {
			SaveItemPlayer(this, item, bodyPart->vnum);
		}
	}
}

bool Player::remInventoryItem(Item *item)
{
	if (Character::remInventoryItem(item)) {
		// Update on database.
		if (item->getType() != ModelType::Corpse) {
			SQLiteDbms::instance().deleteFrom(
				"ItemPlayer", { std::make_pair("item", ToString(item->vnum)) });
		}
		return true;
	}
	return false;
}

bool Player::remEquipmentItem(Item *item)
{
	if (Character::remEquipmentItem(item)) {
		// Update on database.
		if (item->getType() != ModelType::Corpse) {
			SQLiteDbms::instance().deleteFrom(
				"ItemPlayer", { std::make_pair("item", ToString(item->vnum)) });
		}
		return true;
	}
	return false;
}

void Player::initialize()
{
	Character::initialize();
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
	return checkConnection() && (connectionState == ConnectionState::Playing) &&
		   (!closing) && logged_in;
}

bool Player::hasPendingOutput() const
{
	return !outbuffer.empty();
}

bool Player::updateOnDB()
{
	if (!SavePlayer(this)) {
		MudLog(LogLevel::Error, "Creating player on DB.");
		return false;
	}
	if (!SavePlayerSkills(this)) {
		MudLog(LogLevel::Error, "Saving player skills on DB.");
		return false;
	}
	if (!SavePlayerLuaVariables(this)) {
		MudLog(LogLevel::Error, "Saving player lua variables on DB.");
		return false;
	}
	return true;
}

void Player::sendPrompt()
{
	if (this->isPlaying()) {
		// Send to the player his prompt.
		if (this->logged_in) {
			std::string out(prompt + "\n");
			FindAndReplace(&out, "&n", ToLower(name));
			FindAndReplace(&out, "&N", name);
			FindAndReplace(&out, "&h", ToString(health));
			FindAndReplace(&out, "&H", ToString(this->getMaxHealth()));
			FindAndReplace(&out, "&s", ToString(stamina));
			FindAndReplace(&out, "&S", ToString(this->getMaxStamina()));
			FindAndReplace(
				&out, "&T",
				(this->combatHandler.getPredefinedTarget() == nullptr) ?
					"" :
					"[" + this->combatHandler.getPredefinedTarget()->getName() +
						"]");
			FindAndReplace(
				&out, "&A",
				(this->combatHandler.getAimedTarget() == nullptr) ?
					"" :
					"[" + this->combatHandler.getAimedTarget()->getName() +
						"]");
			this->sendMsg(out);
		} else {
			this->sendMsg("\n");
		}
	}
}

void Player::kill()
{
	// Call the method of the father class.
	Character::kill();
	// Move the player to the default room.
	if ((room = Mud::instance().findRoom(rent_room)) != nullptr) {
		room->addCharacter(this);
	}
	// Set values of health and stamina to 1.
	this->setHealth(1, true);
	this->setStamina(1, true);
	Character::sendMsg("%sYou have%s died%s... "
					   "but don't worry your fight is not over...%s\n\n\n",
					   Formatter::gray(), Formatter::red(), Formatter::gray(),
					   Formatter::reset());
}

void Player::enterGame()
{
	// -------------------------------------------------------------------------
	// Phase 1: Clear the screen and show the welcome message.
	this->sendMsg(Formatter::clearScreen());
	// Greet them.
	Character::sendMsg("%sWelcome, %s!%s\n", Formatter::bold(), name,
					   Formatter::reset());
	// -------------------------------------------------------------------------
	// Phase 2: Show the news.
	this->sendMsg("#---------------- Global News ----------------#\n");
	for (auto it = Mud::instance().mudNews.rbegin();
		 it != Mud::instance().mudNews.rend(); ++it) {
		this->sendMsg("Date :" + it->first + "\n");
		this->sendMsg(it->second + "\n");
	}
	this->sendMsg("#---------------------------------------------#\n\n");
	// -------------------------------------------------------------------------
	// Phase 3: Place the player.
	this->sendMsg("You walked through the mist and came into the world...\n\n");
	// Notice all the players in the same room.
	if (room != nullptr) {
		room->addCharacter(this);
		// Send the message inside the room.
		room->sendToAll("%s appears.\n", { this }, name);
	} else {
		this->closeConnection();
	}
	// Set the player as logged in.
	logged_in = true;
	// -------------------------------------------------------------------------
	// Phase 4: Initialize the player.
	this->initialize();
	// -------------------------------------------------------------------------
	// Phase 5: Look around.
	this->doCommand("look");
}

/// Size of buffers used for communications.
#define BUFSIZE 512

void Player::processRead()
{
	char buffer[BUFSIZE];
	// Once closed, don't handle any pending input.
	if (closing) {
		return;
	}
	ssize_t nRead = recv(psocket, &buffer, BUFSIZE - 1, MSG_DONTWAIT);
	if (nRead <= 0) {
		MudLog(LogLevel::Error, "Socket recv failed: %d", errno);
		// Close the socket.
		if (Mud::instance().closeSocket(psocket)) {
			MudLog(LogLevel::Error,
				   "Something has gone wrong during socket closure.");
		}
		// Log the error.
		MudLog(LogLevel::Error, "Connection %d closed.", psocket);
		// Clear the socket.
		this->psocket = NO_SOCKET_COMMUNICATION;
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
	// Execute the received command.
	this->doCommand(Trim(inbuf));
	// Null-terminate the buffer.
	inbuf[uRead] = 0;
}

void Player::processWrite()
{
	std::vector<unsigned char> uncompressed;
	std::vector<unsigned char> compressed;
	//std::vector<unsigned char> check;
	// We will loop attempting to write all in buffer, until write blocks.
	while ((psocket != NO_SOCKET_COMMUNICATION) && !outbuffer.empty()) {
		// We attach to the message the player prompt.
		this->sendPrompt();

		// For portability I replace \n with \r\n.
		FindAndReplace(&outbuffer, "\n", "\r\n");

		// Send a maximum of 4096 at a time.
		size_t iLength = std::min<size_t>(outbuffer.size(), BUFSIZE);

#if 0
        // Compress the data.
        for (unsigned int i = 0; i < outbuffer.size(); i++)
        uncompressed.push_back(outbuffer[i]);

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
		auto nWrite =
			send(psocket, outbuffer.c_str(), outbuffer.size(), MSG_NOSIGNAL);
		auto uWritten = static_cast<std::size_t>(nWrite);
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
		if (nWrite <= 0) {
			if (errno == EPIPE) {
				MudLog(LogLevel::Error, "Sending on a closed connection...");
			} else {
				MudLog(LogLevel::Error, "Unknown error during Send...");
			}
			return;
		}
		// Update the output band.
		MudUpdater::instance().updateBandOut(uWritten);
		// Remove what we successfully sent from the buffer.
		outbuffer.erase(0, uWritten);
		// If partial write, exit
		if (uWritten < iLength) {
			break;
		}
	}
}

void Player::processException()
{
	// Signals can cause exceptions, don't get too excited. :)
	std::cerr << "Exception on socket " << psocket << std::endl << std::endl;
}

void Player::updateTicImpl()
{
	// Check if the player is playing.
	if (this->isPlaying()) {
		Character::updateTicImpl();
	}
}

void Player::updateHourImpl()
{
	// Nothing to do.
}
