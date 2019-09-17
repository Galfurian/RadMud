/// @file   mud.cpp
/// @brief  Implements mud functions.
/// @author Enrico Fraccaroli
/// @date   Jan 8 2016
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

#include "mud.hpp"

#include "processPlayerName.hpp"
#include "CMacroWrapper.hpp"
#include "mobileModel.hpp"
#include "stopwatch.hpp"
#include <unistd.h>
#include <signal.h>

/// Input file descriptor.
static fd_set in_set;
/// Output file descriptor.
static fd_set out_set;
/// Exception file descriptor.
static fd_set exc_set;

void Bailout(int signal)
{
	std::cout << "\n";
	MudLog(LogLevel::Global, "Received signal " + ToString(signal) + "!");
	Mud::instance().shutDownSignal();
}

Mud::Mud() :
	mudPort(4000),
	_servSocket(-1),
	_maxDesc(-1),
	_shutdownSignal(),
	_bootTime(time(NULL)),
	_maxVnumRoom(1),
	_maxVnumItem(1),
	_maxVnumMobile(1),
	_mudMeasure("stones"),
	_mudDatabaseName("radmud.db"),
	_mudSystemDirectory("../system/"),
	mudPlayers(),
	mudMobiles(),
	mudMobileModels(),
	mudItems(),
	mudRooms(),
	mudItemModels(),
	mudAreas(),
	mudRaces(),
	mudFactions(),
	mudSkills(),
	mudWritings(),
	mudCorpses(),
	mudMaterials(),
	mudProfessions(),
	mudProductions(),
	mudLiquids(),
	mudTravelPoints(),
	blockedIPs(),
	badNames(),
	mudNews(),
	mudCommands(),
	mudBuildings(),
	mudTerrains(),
	mudBodyParts()
{
	// Nothing to do.
}

Mud::~Mud()
{
	MudLog(LogLevel::Global, "Freeing memory occupied by players...");
	for (auto iterator : mudPlayers) {
		delete (iterator);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by mobiles...");
	for (auto iterator : mudMobiles) {
		delete (iterator);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by items...");
	for (auto iterator : mudItems) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by rooms...");
	for (auto iterator : mudRooms) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by areas...");
	for (auto iterator : mudAreas) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by writings...");
	for (auto iterator : mudWritings) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by corpses...");
	for (auto iterator : mudCorpses) {
		delete (iterator);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by races...");
	for (auto iterator : mudRaces) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by factions...");
	for (auto iterator : mudFactions) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by liquids...");
	for (auto iterator : mudLiquids) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by professions...");
	for (auto iterator : mudProfessions) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by productions...");
	for (auto iterator : mudProductions) {
		delete (iterator.second);
	}
	MudLog(LogLevel::Global, "Freeing memory occupied by materials...");
	for (auto iterator : mudMaterials) {
		delete (iterator.second);
	}
}

Mud &Mud::instance()
{
	// Since it's a static variable, if the class has already been created,
	// It won't be created again. And it **is** thread-safe in C++11.
	static Mud instance;
	// Return a reference to our instance.
	return instance;
}

bool Mud::savePlayers()
{
	return SQLiteDbms::instance().updatePlayers();
}

bool Mud::saveItems()
{
	return SQLiteDbms::instance().updateItems();
}

bool Mud::saveRooms()
{
	return SQLiteDbms::instance().updateRooms();
}

bool Mud::saveMud()
{
	bool result = true;
	MudLog(LogLevel::Global, "Saving information on Database for : Players...");
	result &= this->savePlayers();
	MudLog(LogLevel::Global, "Saving information on Database for : Items...");
	result &= this->saveItems();
	MudLog(LogLevel::Global, "Saving information on Database for : Rooms...");
	result &= this->saveRooms();
	// Save, if necessary the database.
	result &= SQLiteDbms::instance().updateInMemoryDatabase();
	return result;
}

void Mud::addPlayer(Player *player)
{
	mudPlayers.insert(mudPlayers.end(), player);
}

bool Mud::remPlayer(Player *player)
{
	for (auto it = mudPlayers.begin(); it != mudPlayers.end(); ++it) {
		if ((*it)->name == player->name) {
			mudPlayers.erase(it);
			return true;
		}
	}
	return false;
}

bool Mud::addMobile(Mobile *mobile)
{
	for (auto it : mudMobiles) {
		if (it->vnum == mobile->vnum) {
			return false;
		}
	}
	_maxVnumMobile = std::max(mobile->vnum, _maxVnumMobile);
	mudMobiles.emplace_back(mobile);
	return true;
}

bool Mud::remMobile(unsigned int vnum)
{
	for (auto it = mudMobiles.begin(); it != mudMobiles.end(); ++it) {
		if ((*it)->vnum == vnum) {
			mudMobiles.erase(it);
			return true;
		}
	}
	return false;
}

bool Mud::addItem(Item *item)
{
	if (mudItems.insert(std::make_pair(item->vnum, item)).second) {
		_maxVnumItem = std::max(_maxVnumItem, item->vnum);
		return true;
	}
	return false;
}

bool Mud::remItem(Item *item)
{
	for (auto it = mudItems.begin(); it != mudItems.end(); ++it) {
		if (it->second->vnum == item->vnum) {
			mudItems.erase(it);
			return true;
		}
	}
	return false;
}

bool Mud::addRoom(Room *room)
{
	bool result = mudRooms.insert(std::make_pair(room->vnum, room)).second;
	if (result) {
		_maxVnumRoom = std::max(_maxVnumRoom, room->vnum);
	}
	return result;
}

bool Mud::remRoom(Room *room)
{
	for (auto it = mudRooms.begin(); it != mudRooms.end(); ++it) {
		if (it->second->vnum == room->vnum) {
			mudRooms.erase(it);
			return true;
		}
	}
	return false;
}

bool Mud::addCorpse(Item *corpse)
{
	mudCorpses.emplace_back(corpse);
	return true;
}

bool Mud::remCorpse(Item *corpse)
{
	for (auto it = mudCorpses.begin(); it != mudCorpses.end(); ++it) {
		if ((*it)->vnum == corpse->vnum) {
			mudCorpses.erase(it);
			return true;
		}
	}
	return false;
}

bool Mud::addItemModel(std::shared_ptr<ItemModel> model)
{
	return mudItemModels.insert(std::make_pair(model->vnum, model)).second;
}

bool Mud::addArea(Area *area)
{
	return mudAreas.insert(std::make_pair(area->vnum, area)).second;
}

bool Mud::addRace(Race *race)
{
	return (race == nullptr) ?
			   false :
			   mudRaces.insert(std::make_pair(race->vnum, race)).second;
}

bool Mud::addFaction(Faction *faction)
{
	return (faction == nullptr) ?
			   false :
			   mudFactions.insert(std::make_pair(faction->vnum, faction)).second;
}

bool Mud::addSkill(std::shared_ptr<Skill> skill)
{
	if (this->findSkill(skill->vnum))
		return false;
	mudSkills.emplace_back(skill);
	return true;
}

bool Mud::addWriting(Writing *writing)
{
	return mudWritings.insert(std::make_pair(writing->vnum, writing)).second;
}

bool Mud::addMaterial(Material *material)
{
	return (material == nullptr) ?
			   false :
			   mudMaterials.insert(std::make_pair(material->vnum, material))
				   .second;
}

bool Mud::addProfession(Profession *profession)
{
	return (profession == nullptr) ?
			   false :
			   mudProfessions
				   .insert(std::make_pair(profession->vnum, profession))
				   .second;
}

bool Mud::addProduction(Production *production)
{
	return (production == nullptr) ?
			   false :
			   mudProductions
				   .insert(std::make_pair(production->vnum, production))
				   .second;
}

bool Mud::addLiquid(Liquid *liquid)
{
	return mudLiquids.insert(std::make_pair(liquid->vnum, liquid)).second;
}

bool Mud::addTravelPoint(Room *source, Room *target)
{
	return mudTravelPoints.insert(std::make_pair(source, target)).second;
}

void Mud::addCommand(const std::shared_ptr<Command> &command)
{
	mudCommands.emplace_back(command);
}

bool Mud::addBuilding(const std::shared_ptr<Building> &building)
{
	return mudBuildings.insert(std::make_pair(building->vnum, building)).second;
}

bool Mud::addTerrain(const std::shared_ptr<Terrain> &terrain)
{
	return mudTerrains.insert(std::make_pair(terrain->vnum, terrain)).second;
}

bool Mud::addBodyPart(const std::shared_ptr<BodyPart> &bodyPart)
{
	return mudBodyParts.insert(std::make_pair(bodyPart->vnum, bodyPart)).second;
}

Player *Mud::findPlayer(const std::string &name)
{
	for (auto it : mudPlayers) {
		// If the player is not playing, continue.
		if (!it->isPlaying())
			continue;
		if (ToLower(it->getName()) == ToLower(name))
			return it;
	}
	return nullptr;
}

Mobile *Mud::findMobile(unsigned int vnum)
{
	for (auto it : mudMobiles) {
		if (it->vnum == vnum)
			return it;
	}
	return nullptr;
}

std::shared_ptr<MobileModel> Mud::findMobileModel(unsigned int vnum)
{
	auto it = mudMobileModels.find(vnum);
	if (it == mudMobileModels.end())
		return nullptr;
	return it->second;
}

std::shared_ptr<ItemModel> Mud::findItemModel(unsigned int vnum)
{
	auto it = mudItemModels.find(vnum);
	return (it == mudItemModels.end()) ? nullptr : it->second;
}

Item *Mud::findItem(unsigned int vnum)
{
	auto it = mudItems.find(vnum);
	return (it == mudItems.end()) ? nullptr : it->second;
}

Area *Mud::findArea(unsigned int vnum)
{
	auto it = mudAreas.find(vnum);
	return (it == mudAreas.end()) ? nullptr : it->second;
}

Room *Mud::findRoom(unsigned int vnum)
{
	auto it = mudRooms.find(vnum);
	return (it == mudRooms.end()) ? nullptr : it->second;
}

Race *Mud::findRace(unsigned int vnum)
{
	auto it = mudRaces.find(vnum);
	return (it == mudRaces.end()) ? nullptr : it->second;
}

Race *Mud::findRace(std::string name)
{
	for (auto &it : mudRaces) {
		if (ToLower(it.second->name) == ToLower(name))
			return it.second;
	}
	return nullptr;
}

Faction *Mud::findFaction(unsigned int vnum)
{
	auto it = mudFactions.find(vnum);
	return (it == mudFactions.end()) ? nullptr : it->second;
}

Faction *Mud::findFaction(std::string name)
{
	for (auto &it : mudFactions) {
		if (ToLower(it.second->name) == ToLower(name))
			return it.second;
	}
	return nullptr;
}

std::shared_ptr<Skill> Mud::findSkill(const unsigned int &vnum)
{
	for (const auto &skill : mudSkills) {
		if (skill->vnum == vnum)
			return skill;
	}
	return nullptr;
}

Writing *Mud::findWriting(unsigned int vnum)
{
	auto it = mudWritings.find(vnum);
	return (it == mudWritings.end()) ? nullptr : it->second;
}

Item *Mud::findCorpse(unsigned int vnum)
{
	for (const auto &corpse : mudCorpses) {
		if (corpse->vnum == vnum)
			return corpse;
	}
	return nullptr;
}

Material *Mud::findMaterial(unsigned int vnum)
{
	auto it = mudMaterials.find(vnum);
	return (it == mudMaterials.end()) ? nullptr : it->second;
}

Profession *Mud::findProfession(unsigned int vnum)
{
	auto it = mudProfessions.find(vnum);
	return (it == mudProfessions.end()) ? nullptr : it->second;
}

Profession *Mud::findProfession(std::string command)
{
	for (auto &it : mudProfessions) {
		if (ToLower(it.second->command) == ToLower(command))
			return it.second;
	}
	return nullptr;
}

Production *Mud::findProduction(unsigned int vnum)
{
	auto it = mudProductions.find(vnum);
	return (it == mudProductions.end()) ? nullptr : it->second;
}

Production *Mud::findProduction(std::string name)
{
	for (auto &it : mudProductions) {
		if (ToLower(it.second->name) == ToLower(name))
			return it.second;
	}
	return nullptr;
}

Liquid *Mud::findLiquid(const unsigned int &vnum)
{
	auto it = mudLiquids.find(vnum);
	return (it == mudLiquids.end()) ? nullptr : it->second;
}

Room *Mud::findTravelPoint(Room *room)
{
	auto it = mudTravelPoints.find(room);
	return (it == mudTravelPoints.end()) ? nullptr : it->second;
}

std::shared_ptr<Building> Mud::findBuilding(std::string name)
{
	for (auto &it : mudBuildings) {
		if (ToLower(it.second->name) == ToLower(name))
			return it.second;
	}
	return nullptr;
}

std::shared_ptr<Building> Mud::findBuilding(unsigned int vnum)
{
	auto it = mudBuildings.find(vnum);
	return (it == mudBuildings.end()) ? nullptr : it->second;
}

std::shared_ptr<Terrain> Mud::findTerrain(unsigned int vnum)
{
	auto it = mudTerrains.find(vnum);
	return (it == mudTerrains.end()) ? nullptr : it->second;
}

std::shared_ptr<BodyPart> Mud::findBodyPart(unsigned int vnum)
{
	auto it = mudBodyParts.find(vnum);
	return (it == mudBodyParts.end()) ? nullptr : it->second;
}

std::shared_ptr<HeightMap> Mud::findHeightMap(const unsigned int &vnum)
{
	auto it = mudHeightMaps.find(vnum);
	return (it == mudHeightMaps.end()) ? nullptr : it->second;
}

bool Mud::runMud()
{
	// Open logging file.
	if (!OpenLog(this->getMudSystemDirectory() + GetDate() + ".log")) {
		std::cerr << "Can't create the logging file." << std::endl;
		return false;
	}

	if (!this->startMud()) {
		MudLog(LogLevel::Error, "Something gone wrong during the boot.");
		return false;
	}
	// Set up timeout interval.
	struct timeval timeoutVal;
	timeoutVal.tv_sec = 0; // seconds
	timeoutVal.tv_usec = 500000; // microseconds
	MudLog(LogLevel::Global, "Waiting for Connections...");
	// Loop processing input, output, events.
	// We will go through this loop roughly every timeout seconds.
	do {
		// Let the time advance.
		MudUpdater::instance().advanceTime();
		// Delete the inactive players.
		this->removeInactivePlayers();
		// Get ready for "select" function.
		FD_ZERO(&in_set);
		FD_ZERO(&out_set);
		FD_ZERO(&exc_set);
		// Add our control socket, needed for new connections.
		CMacroWrapper::FdSet(_servSocket, &in_set);
		// Set the max file descriptor to the server socket.
		_maxDesc = _servSocket;
		// Set bits in in_set, out_set etc. for each connected player.
		for (auto iterator : mudPlayers) {
			this->setupDescriptor(iterator);
		}
		// Check for activity, timeout after 'timeout' seconds.
		int activity =
			select((_maxDesc + 1), &in_set, &out_set, &exc_set, &timeoutVal);
		if ((activity < 0) && (errno != EINTR)) {
			perror("Select");
		}
		// Check if there are new connections on control port.
		if (CMacroWrapper::FdIsSet(_servSocket, &in_set)) {
			if (!this->processNewConnection()) {
				MudLog(LogLevel::Error,
					   "Error during processing a new connection.");
			}
		}
		// Handle all player input/output.
		for (auto iterator : mudPlayers) {
			this->processDescriptor(iterator);
		}
	} while (!_shutdownSignal);
	if (!this->stopMud()) {
		MudLog(LogLevel::Error, "Something gone wrong during the shutdown.");
		return false;
	}
	return true;
}

void Mud::shutDownSignal()
{
	// Game over - Tell them all.
	this->broadcastMsg(0, "\nGame is shutting down!\n");
	_shutdownSignal = true;
}

bool Mud::checkSocket(const int &socket) const
{
	int error_code;
	socklen_t error_code_size = sizeof(error_code);
	return getsockopt(socket, SOL_SOCKET, SO_ERROR, &error_code,
					  &error_code_size) == 0;
}

bool Mud::closeSocket(const int &socket) const
{
#ifdef __linux__
	return close(socket) == 0;
#elif __APPLE__
	return close(socket) == 0;
#elif __CYGWIN__
	return close(socket) == 0;
#elif _WIN32
	return closesocket(socket) == 0;
#endif
}

double Mud::getUpTime() const
{
	return difftime(time(NULL), _bootTime);
}

unsigned int Mud::getMaxVnumRoom() const
{
	return _maxVnumRoom;
}

unsigned int Mud::getMaxVnumItem() const
{
	return _maxVnumItem;
}

unsigned int Mud::getUniqueAreaVnum() const
{
	unsigned int vnum;
	do {
		vnum = TRand<unsigned int>(0, INT8_MAX);
	} while (mudAreas.find(vnum) != mudAreas.end());
	return vnum;
}

void Mud::broadcastMsg(const int &level, const std::string &message) const
{
	for (auto iterator : mudPlayers) {
		// If the player is not playing, continue.
		if (!iterator->isPlaying()) {
			continue;
		}
		if (level == 1 && HasFlag(iterator->flags, CharacterFlag::IsGod)) {
			iterator->sendMsg("\n" + message + "\n");
		} else if (level == 0) {
			iterator->sendMsg("\n" + message + "\n");
		}
	}
}

std::string Mud::getWeightMeasure() const
{
	return _mudMeasure;
}

std::string Mud::getMudDatabaseName() const
{
	return _mudDatabaseName;
}

std::string Mud::getMudSystemDirectory() const
{
	return _mudSystemDirectory;
}

void Mud::removeInactivePlayers()
{
	std::set<Player *> toRemove;
	for (auto iterator : mudPlayers) {
		// Proceed only if the player is not connected and is closing.
		if (!iterator->checkConnection() || iterator->closing) {
			// Add the player to the list of players that have to be removed.
			toRemove.insert(iterator);
		}
	}
	for (auto iterator = toRemove.begin(); iterator != toRemove.end();
		 ++iterator) {
		// Get the player at the given position.
		auto player = *iterator;
		// Log the action of removing.
		MudLog(LogLevel::Global,
			   "Removing inactive player : " + player->getName());
		// Only if the player has successfully logged in, save its state on DB.
		if (player->logged_in) {
			SQLiteDbms::instance().beginTransaction();
			if (!player->updateOnDB()) {
				SQLiteDbms::instance().rollbackTransection();
			}
			SQLiteDbms::instance().endTransaction();
		}
		// Remove the player from the list of players.
		remPlayer(player);
		// Delete the player.
		delete (player);
	}
}

bool Mud::processNewConnection()
{
	// The file descriptor for the new socket.
	int socketFileDescriptor;
	struct sockaddr_in socketAddress;
	socklen_t socketAddressSize = sizeof(socketAddress);
	// Loop until all outstanding connections are accepted.
	while (true) {
		socketFileDescriptor =
			accept(_servSocket,
				   reinterpret_cast<struct sockaddr *>(&socketAddress),
				   &socketAddressSize);
		// A bad socket probably means no more connections are outstanding.
		if (socketFileDescriptor == NO_SOCKET_COMMUNICATION) {
			// blocking is OK - we have accepted all outstanding connections.
			if (errno == EWOULDBLOCK) {
				break;
			}
			perror("ACCEPT");
			return false;
		}
		// Here on successful accept - make sure socket doesn't block.
#ifdef __linux__
		if (fcntl(socketFileDescriptor, F_SETFL, FNDELAY) == -1) {
			perror("FCNTL on player socket");
			return false;
		}
#elif __APPLE__
		if (fcntl(socketFileDescriptor, F_SETFL, FNDELAY) == -1) {
			perror("FCNTL on player socket");
			return false;
		}
#elif __CYGWIN__
		int flags = fcntl(_servSocket, F_GETFL, 0);
		// O_NONBLOCK | O_RDWR | O_NOCTTY | O_NDELAY
		if (fcntl(_servSocket, F_SETFL, flags | O_NDELAY | O_NONBLOCK) == -1) {
			perror("FCNTL on player socket");
			return false;
		}
#elif _WIN32
		u_long imode = 1;
		if (ioctlsocket(_servSocket, FIONBIO, &imode) == -1) {
			perror("FCNTL on player socket");
			return false;
		}
#endif
		std::string address = inet_ntoa(socketAddress.sin_addr);
		int port = ntohs(socketAddress.sin_port);
		// Immediately close connections from blocked IP addresses.
		if (blockedIPs.find(address) != blockedIPs.end()) {
			MudLog(LogLevel::Global,
				   "Rejected connection from " + address + "!");
			closeSocket(socketFileDescriptor);
			continue;
		}
		auto player = new Player(socketFileDescriptor, port, address);
		// Insert the player in the list of players.
		this->addPlayer(player);
		MudLog(LogLevel::Global, "#--------- New Connection ---------#");
		MudLog(LogLevel::Global,
			   " Socket  : " + ToString(socketFileDescriptor));
		MudLog(LogLevel::Global, " Address : " + address);
		MudLog(LogLevel::Global, " Port    : " + ToString(port));
		MudLog(LogLevel::Global, "#----------------------------------#");
		// Create a shared pointer to the next step.
		auto newStep = std::make_shared<ProcessPlayerName>(player);
		// Set the handler.
		player->inputProcessor = newStep;
		// Advance to the next step.
		newStep->advance();
		//        // Activate the procedure of negotiation.
		//        NegotiateProtocol(player, ConnectionState::NegotiatingMSDP);
		//        // Create a shared pointer to the next step.
		//        auto newStep = std::make_shared<ProcessTelnetCommand>();
		//        // Set the handler.
		//        player->inputProcessor = newStep;
	}
	return true;
}

void Mud::setupDescriptor(Player *player)
{
	// Don't bother if connection is closed.
	if (player->checkConnection()) {
		_maxDesc = std::max(_maxDesc, player->getSocket());
		// Don't take input if they are closing down.
		if (!player->closing) {
			CMacroWrapper::FdSet(player->getSocket(), &in_set);
			CMacroWrapper::FdSet(player->getSocket(), &exc_set);
		}
		// We are only interested in writing to sockets we have something for.
		if (player->hasPendingOutput()) {
			CMacroWrapper::FdSet(player->getSocket(), &out_set);
		}
	}
}

void Mud::processDescriptor(Player *player)
{
	// Handle exceptions.
	if (player->checkConnection()) {
		if (CMacroWrapper::FdIsSet(player->getSocket(), &exc_set)) {
			player->processException();
		}
	}
	// Look for ones we can read from, provided they aren't closed.
	if (player->checkConnection()) {
		if (CMacroWrapper::FdIsSet(player->getSocket(), &in_set)) {
			player->processRead();
		}
	}
	// Look for ones we can write to, provided they aren't closed.
	if (player->checkConnection()) {
		if (CMacroWrapper::FdIsSet(player->getSocket(), &out_set)) {
			player->processWrite();
		}
	}
}

bool Mud::initDatabase()
{
	if (!SQLiteDbms::instance().openDatabase()) {
		MudLog(LogLevel::Error, "Error opening database!");
		return false;
	}
	if (!SQLiteDbms::instance().loadTables()) {
		MudLog(LogLevel::Error, "Error loading tables!");
		return false;
	}
	return true;
}

bool Mud::initComunications()
{
	struct sockaddr_in socketAddress;
#ifdef _WIN32
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(0x0202, &wsaData);
	if (iResult != 0) {
		perror("WSAStartup failed: " + ToString(iResult));
		return false;
	}
#endif

	// Create the control socket.
	if ((_servSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Creating Control Socket");
		return false;
	}

	// Make sure socket doesn't block.
#ifdef __linux__
	if (fcntl(_servSocket, F_SETFL, FNDELAY) == -1) {
		perror("FCNTL on Control Socket");
		return false;
	}
#elif __APPLE__
	if (fcntl(_servSocket, F_SETFL, FNDELAY) == -1) {
		perror("FCNTL on Control Socket");
		return false;
	}
#elif __CYGWIN__

	int flags = fcntl(_servSocket, F_GETFL, 0);
	// O_NONBLOCK | O_RDWR | O_NOCTTY | O_NDELAY
	if (fcntl(_servSocket, F_SETFL, flags | O_NDELAY | O_NONBLOCK) == -1) {
		perror("FCNTL on Control Socket");
		return false;
	}
#elif _WIN32
	u_long imode = 1;
	if (ioctlsocket(_servSocket, FIONBIO, &imode) == -1) {
		perror("FCNTL on Control Socket");
		return false;
	}
#endif

	// Zero it.
	struct linger ld = linger();

	// Don't allow closed sockets to linger.
	if (setsockopt(_servSocket, SOL_SOCKET, SO_LINGER,
				   reinterpret_cast<char *>(&ld), sizeof ld) < 0) {
		perror("Setsockopt (SO_LINGER)");
		return false;
	}

	int x = 1;

	// Allow address reuse.
	if (setsockopt(_servSocket, SOL_SOCKET, SO_REUSEADDR,
				   reinterpret_cast<char *>(&x), sizeof x) == -1) {
		perror("Setsockopt (SO_REUSEADDR)");
		return false;
	}

	// Change to listen on a specific adapter.
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(mudPort);
	socketAddress.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to our connection port.
	if (::bind(_servSocket, reinterpret_cast<struct sockaddr *>(&socketAddress),
			   sizeof(socketAddress)) < 0) {
		perror("BIND");
		return false;
	}

	// Listen for connections
	// Note: SOMAXCONN is the backlog count.
	if (listen(_servSocket, SOMAXCONN) == -1) {
		perror("LISTEN");
		return false;
	}

	// Standard termination signals.
	signal(SIGINT, Bailout);
	signal(SIGTERM, Bailout);
	signal(SIGHUP, Bailout);

	return true;
}

bool Mud::closeComunications()
{
	return (_servSocket == NO_SOCKET_COMMUNICATION) ?
			   false :
			   this->closeSocket(_servSocket);
}

bool Mud::startMud()
{
	// Create a stopwatch for general timing information.
	Stopwatch<std::chrono::milliseconds> stopwatch("Boot");
	MudLog(LogLevel::Global, "#--------------------------------------------#");
	MudLog(LogLevel::Global, "             XXXXXXXXXXXXX                ");
	MudLog(LogLevel::Global, "  /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    ");
	MudLog(LogLevel::Global, "  \\##/#/#XXXXXXXXXXXXXXXXXXXXX#\\#\\##/  ");
	MudLog(LogLevel::Global, "   \\/#/#XXXXXXXXXXXXXXXXXXXXXXX#\\#\\/   ");
	MudLog(LogLevel::Global, "    \\/##XXXXXXXXXXXXXXXXXXXXXXX##\\/     ");
	MudLog(LogLevel::Global, "     ###XXXX  ''-.XXX.-''  XXXX###        ");
	MudLog(LogLevel::Global, "       \\XXXX               XXXX/         ");
	MudLog(LogLevel::Global, "         XXXXXXXXXXXXXXXXXXXXX            ");
	MudLog(LogLevel::Global, "         XXXX XXXX X XXXX XXXX            ");
	MudLog(LogLevel::Global, "         XXX # XXX X XXX # XXX            ");
	MudLog(LogLevel::Global, "        /XXXX XXX XXX XXX XXXX\\          ");
	MudLog(LogLevel::Global, "       ##XXXXXXX X   X XXXXXXX##          ");
	MudLog(LogLevel::Global, "      ##   XOXXX X   X XXXOX   ##         ");
	MudLog(LogLevel::Global, "      ##    #XXXX XXX XXX #    ##         ");
	MudLog(LogLevel::Global, "       ##..##  XXXXXXXXX  ##..##          ");
	MudLog(LogLevel::Global, "        ###      XXXXX     ####           ");
	MudLog(LogLevel::Global, "#--------------------------------------------#");
	MudLog(LogLevel::Global, "|                   RadMud                   |");
	MudLog(LogLevel::Global, "| Created by : Enrico Fraccaroli.            |");
	MudLog(LogLevel::Global, "| Date       : 29 September 2014             |");
	MudLog(LogLevel::Global, "#--------------------------------------------#");
	MudLog(LogLevel::Global, "Booting...");
	// Set the boot time.
	time(&_bootTime);
	MudLog(LogLevel::Global, "Initializing Commands...");
	LoadCommands();
	MudLog(LogLevel::Global, "Initializing Database...");
	if (!this->initDatabase()) {
		MudLog(LogLevel::Error,
			   "Something gone wrong during database initialization.");
		return false;
	}

	MudLog(LogLevel::Global, "Initializing Communications...");
	if (!this->initComunications()) {
		MudLog(LogLevel::Error,
			   "Something gone wrong during initialization of comunication.");
		return false;
	}
	MudLog(LogLevel::Global,
		   "Booting Done (" + ToString(stopwatch.elapsed()) + ").");
	if (!this->afterBootActions()) {
		MudLog(LogLevel::Error, "Afterboot actions went wrong.");
		return false;
	}
	return true;
}

bool Mud::stopMud()
{
	Stopwatch<std::chrono::milliseconds> stopwatch("Shutdown");

	MudLog(LogLevel::Global, "Shutting down RadMud...");
	MudLog(LogLevel::Global, "Closing Communications...");
	if (!this->closeComunications()) {
		MudLog(LogLevel::Error,
			   "The communication has not been closed correctly.");
	}

	MudLog(LogLevel::Global, "Saving Mud Information...");
	if (!this->saveMud()) {
		MudLog(LogLevel::Error, "Somwthing has gone wrong during data saving.");
	}

	MudLog(LogLevel::Global, "Closing Database...");
	if (!SQLiteDbms::instance().closeDatabase()) {
		MudLog(LogLevel::Error, "The database has not been closed correctly.");
	}
	MudLog(LogLevel::Global,
		   "Shutdown Completed (" + ToString(stopwatch.elapsed()) + ").");

	///////////////////////////////////////////////////////////////////////////
	size_t bIn = MudUpdater::instance().getBandIn();
	size_t bOut = MudUpdater::instance().getBandOut();
	size_t bUnc = MudUpdater::instance().getBandUncompressed();
	// Print some statistics.
	MudLog(LogLevel::Info, "");
	MudLog(LogLevel::Info, "Statistics");
	MudLog(LogLevel::Info, "    In            = " + ToString(bIn) + " Bytes.");
	MudLog(LogLevel::Info, "    Output        = " + ToString(bOut) + " Bytes.");
	MudLog(LogLevel::Info, "    Uncompressed  = " + ToString(bUnc) + " Bytes.");
	MudLog(LogLevel::Info, "");
	return true;
}

bool Mud::afterBootActions()
{
#if 0
    auto heightMap = this->findHeightMap(1);
    if (heightMap == nullptr)
    {
        return false;
    }
    MapGen::MapConfiguration configuration(60, 30, 40);
    // Instantiate the map generator.
    MapGen::MapGenerator mapGenerator(configuration, heightMap);
    // Generate the map.
    if (!mapGenerator.generateMap())
    {
        return false;
    }
#endif
	return true;
}