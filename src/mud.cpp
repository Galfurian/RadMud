/// @file   mud.cpp
/// @brief  Implements mud functions.
/// @author Enrico Fraccaroli
/// @date   Jan 8 2016
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

#include "mud.hpp"

#include <algorithm>
#include <signal.h>
#include <chrono>
#include <errno.h>

#include "states.hpp"

/// Input file descriptor.
static fd_set in_set;
/// Output file descriptor.
static fd_set out_set;
/// Exceptino file descriptor.
static fd_set exc_set;

Mud::Mud() :
        _servSocket(-1),
        _maxDesc(-1),
        _shutdownSignal(),
        _bootTime(time(NULL)),
        _maxVnumRoom(),
        _maxVnumItem(),
        _minVnumCorpses()
{
}

Mud::~Mud()
{
}

bool Mud::savePlayers()
{
    bool result = true;
    for (auto iterator : mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        if (!iterator->updateOnDB())
        {
            LogError("Error saving player :" + iterator->getName());
            result = false;
        }
    }
    return result;
}

bool Mud::saveItems()
{
    bool result = true;
    for (auto iterator : mudItems)
    {
        Item * item = iterator.second;
        if (!item->updateOnDB())
        {
            LogError("Error saving item :" + ToString(item->vnum));
            result = false;
            break;
        }
    }
    return result;
}

bool Mud::saveRooms()
{
    bool result = true;
    for (auto iterator : mudRooms)
    {
        Room * room = iterator.second;
        if (!room->updateOnDB())
        {
            LogError("Error saving room :" + ToString(room->vnum));
            result = false;
            break;
        }
    }
    return result;
}

bool Mud::addPlayer(Player * player)
{
    return mudPlayers.insert(player).second;
}
bool Mud::remPlayer(Player * player)
{
    return (remove_erase(mudPlayers, player) != mudPlayers.end());
}
bool Mud::addMobile(Mobile * mobile)
{
    return mudMobiles.insert(std::make_pair(mobile->id, mobile)).second;
}
bool Mud::remMobile(Mobile * mobile)
{
    return (remove_erase(mudMobiles, mobile->id) != mudMobiles.end());
}
bool Mud::addItem(Item * item)
{
    bool result = mudItems.insert(std::make_pair(item->vnum, item)).second;
    if (result)
    {
        _maxVnumItem = std::max(_maxVnumItem, item->vnum);
    }
    return result;
}
bool Mud::remItem(Item * item)
{
    return (remove_erase(mudItems, item->vnum) != mudItems.end());
}
bool Mud::addRoom(Room * room)
{
    bool result = mudRooms.insert(std::make_pair(room->vnum, room)).second;
    if (result)
    {
        _maxVnumRoom = std::max(_maxVnumRoom, room->vnum);
    }
    return result;
}
bool Mud::remRoom(Room * room)
{
    return (remove_erase(mudRooms, room->vnum) != mudRooms.end());
}
void Mud::addCorpse(Item * corpse)
{
    mudCorpses.push_back(corpse);
    _minVnumCorpses = std::min(_minVnumCorpses, corpse->vnum);
}
bool Mud::remCorpse(Item * corpse)
{
    return (remove_erase(mudCorpses, corpse) != mudCorpses.end());
}
bool Mud::addModel(Model & model)
{
    return mudModels.insert(std::make_pair(model.vnum, model)).second;
}
bool Mud::addArea(Area * area)
{
    return mudAreas.insert(std::make_pair(area->vnum, area)).second;
}
bool Mud::addRace(Race & race)
{
    return mudRaces.insert(std::make_pair(race.vnum, race)).second;
}
bool Mud::addFaction(Faction & faction)
{
    return mudFactions.insert(std::make_pair(faction.vnum, faction)).second;
}
bool Mud::addSkill(Skill & skill)
{
    return mudSkills.insert(std::make_pair(skill.vnum, skill)).second;
}
bool Mud::addWriting(Writing * writing)
{
    return mudWritings.insert(std::make_pair(writing->vnum, writing)).second;
}
bool Mud::addContinent(Continent * continent)
{
    return mudContinents.insert(std::make_pair(continent->vnum, continent)).second;
}
bool Mud::addMaterial(Material & material)
{
    return mudMaterials.insert(std::make_pair(material.vnum, material)).second;
}
bool Mud::addProfession(Profession & profession)
{
    return mudProfessions.insert(std::make_pair(profession.vnum, profession)).second;
}
bool Mud::addProduction(Production & production)
{
    return mudProductions.insert(std::make_pair(production.vnum, production)).second;
}
bool Mud::addLiquid(Liquid & liquid)
{
    return mudLiquids.insert(std::make_pair(liquid.vnum, liquid)).second;
}
bool Mud::addTravelPoint(Room * source, Room * target)
{
    return mudTravelPoints.insert(std::make_pair(source, target)).second;
}
void Mud::addCommand(Command & command)
{
    mudCommands.push_back(command);
}
bool Mud::addDirection(std::string name, Direction direction)
{
    return mudDirections.insert(std::make_pair(name, direction)).second;
}
bool Mud::addStateAction(ConnectionState state, ActionHandler action)
{
    return mudStateActions.insert(std::make_pair(state, action)).second;
}
bool Mud::addBuilding(Building & building)
{
    return mudBuildings.insert(std::make_pair(building.vnum, building)).second;
}

Player * Mud::findPlayer(std::string name)
{
    for (auto iterator : mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        if (ToLower(iterator->getName()) == ToLower(name))
        {
            return iterator;
        }
    }
    return nullptr;
}

Mobile * Mud::findMobile(std::string id)
{
    MobileMap::iterator iterator = mudMobiles.find(id);
    if (iterator != mudMobiles.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Model * Mud::findModel(int vnum)
{
    ModelMap::iterator iterator = mudModels.find(vnum);
    if (iterator != mudModels.end())
    {
        return &(iterator->second);
    }
    else return nullptr;
}

Item * Mud::findItem(int vnum)
{
    ItemMap::iterator iterator = mudItems.find(vnum);
    if (iterator != mudItems.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Area * Mud::findArea(int vnum)
{
    AreaMap::iterator iterator = mudAreas.find(vnum);
    if (iterator != mudAreas.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Room * Mud::findRoom(int vnum)
{
    RoomMap::iterator iterator = mudRooms.find(vnum);
    if (iterator != mudRooms.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Room * Mud::findRoom(int x, int y, int z)
{
    for (auto iterator : mudRooms)
    {
        Room * room = iterator.second;
        if (room->coord.x == x && room->coord.y == y && room->coord.z == z)
        {
            return room;
        }
    }
    return nullptr;
}

Room * Mud::findRoom(Coordinates coord)
{
    return this->findRoom(coord.x, coord.y, coord.z);
}

Race * Mud::findRace(int vnum)
{
    RaceMap::iterator iterator = mudRaces.find(vnum);
    if (iterator != mudRaces.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Race * Mud::findRace(std::string name)
{
    for (RaceMap::iterator iterator = mudRaces.begin(); iterator != mudRaces.end(); ++iterator)
    {
        if (ToLower(iterator->second.name) == ToLower(name))
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Faction * Mud::findFaction(int vnum)
{
    FactionMap::iterator iterator = mudFactions.find(vnum);
    if (iterator != mudFactions.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Faction * Mud::findFaction(std::string name)
{
    for (FactionMap::iterator iterator = mudFactions.begin(); iterator != mudFactions.end(); ++iterator)
    {
        if (ToLower(iterator->second.name) == ToLower(name))
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Skill * Mud::findSkill(int vnum)
{
    SkillMap::iterator iterator = mudSkills.find(vnum);
    if (iterator != mudSkills.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Writing * Mud::findWriting(int vnum)
{
    WritingMap::iterator iterator = mudWritings.find(vnum);
    if (iterator != mudWritings.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Item * Mud::findCorpse(int vnum)
{
    for (ItemList::iterator it = mudCorpses.begin(); it != mudCorpses.end(); ++it)
    {
        Item * corpse = (*it);
        if (corpse->vnum == vnum)
        {
            return corpse;
        }
    }
    return nullptr;
}

Continent * Mud::findContinent(int vnum)
{
    ContinentMap::iterator iterator = mudContinents.find(vnum);
    if (iterator != mudContinents.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Material * Mud::findMaterial(int vnum)
{
    MaterialMap::iterator iterator = mudMaterials.find(vnum);
    if (iterator != mudMaterials.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Profession * Mud::findProfession(unsigned int vnum)
{
    ProfessionMap::iterator iterator = mudProfessions.find(vnum);
    if (iterator != mudProfessions.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Profession * Mud::findProfession(std::string command)
{
    for (ProfessionMap::iterator iterator = mudProfessions.begin(); iterator != mudProfessions.end(); ++iterator)
    {
        if (ToLower(iterator->second.command) == ToLower(command))
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Production * Mud::findProduction(int vnum)
{
    ProductionMap::iterator iterator = mudProductions.find(vnum);
    if (iterator != mudProductions.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Production * Mud::findProduction(std::string name)
{
    for (ProductionMap::iterator iterator = mudProductions.begin(); iterator != mudProductions.end(); ++iterator)
    {
        if (ToLower(iterator->second.name) == ToLower(name))
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Liquid * Mud::findLiquid(int vnum)
{
    LiquidMap::iterator iterator = mudLiquids.find(vnum);
    if (iterator != mudLiquids.end())
    {
        return &(iterator->second);
    }
    return nullptr;
}

Room * Mud::findTravelPoint(Room * room)
{
    TravelPointMap::iterator iterator = mudTravelPoints.find(room);
    if (iterator != mudTravelPoints.end())
    {
        return iterator->second;
    }
    return nullptr;
}

ActionHandler & Mud::findStateAction(ConnectionState state)
{
    return mudStateActions.find(state)->second;
}

Building * Mud::findBuilding(std::string name)
{
    for (BuildingMap::iterator iterator = mudBuildings.begin(); iterator != mudBuildings.end(); ++iterator)
    {
        if (ToLower(iterator->second.name) == ToLower(name))
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Building * Mud::findBuilding(int vnum)
{
    for (BuildingMap::iterator iterator = mudBuildings.begin(); iterator != mudBuildings.end(); ++iterator)
    {
        if (iterator->second.buildingModel->vnum == vnum)
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Direction Mud::findDirection(std::string direction, bool exact)
{
    for (auto iterator : mudDirections)
    {
        if (exact)
        {
            if (iterator.first == ToLower(direction))
            {
                return iterator.second;
            }
        }
        else
        {
            if (BeginWith(iterator.first, ToLower(direction)))
            {
                return iterator.second;
            }
        }
    }
    return Direction::None;
}

Mud & Mud::getInstance()
{
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static Mud instance;
    // Return a reference to our instance.
    return instance;
}

SQLiteDbms & Mud::getDbms()
{
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static SQLiteDbms instance;
    // Return a reference to our instance.
    return instance;
}

MudUpdater & Mud::getUpdater()
{
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static MudUpdater instance;
    // Return a reference to our instance.
    return instance;
}

CharacterList Mud::getCharacterList()
{
    // Initialize the variables.
    CharacterList characters;
    for (auto iterator : mudMobiles)
    {
        characters.push_back(iterator.second);
    }

    for (auto iterator : mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        characters.push_back(iterator);
    }

    return characters;
}

time_t Mud::getBootTime()
{
    return _bootTime;
}

int Mud::getMaxVnumRoom()
{
    return _maxVnumRoom;
}

int Mud::getMaxVnumItem()
{
    return _maxVnumItem;
}

int Mud::getMinVnumCorpse()
{
    return _minVnumCorpses;
}

bool Mud::isSocketClosed(int socket)
{
    int error_code;
    socklen_t error_code_size = sizeof(error_code);
    return getsockopt(socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size) != 0;
}

void Mud::broadcastMsg(int level, std::string message)
{
    for (auto iterator : mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        if (level == 1 && HasFlag(iterator->flags, CharacterFlag::IsGod))
        {
            iterator->sendMsg("\n" + message + "\n");
        }
        else if (level == 0)
        {
            iterator->sendMsg("\n" + message + "\n");
        }
    }
}

void Mud::removeInactivePlayers()
{
    PlayerList toRemove = PlayerList();
    for (auto iterator : mudPlayers)
    {
        // Proceed only if the player is not connected and is closing.
        if (!iterator->checkConnection() || iterator->closing)
        {
            // Add the player to the list of players that have to be removed.
            toRemove.insert(iterator);
        }
    }
    for (auto iterator = toRemove.begin(); iterator != toRemove.end(); ++iterator)
    {
        // Get the player at the given position.
        Player * player = *iterator;
        // Log the action of removing.
        LogMessage(kMSys, "Removing inactive player : " + player->getName());
        // Only if the player has successfully logged in, save its state on DB.
        if (player->logged_in)
        {
            getDbms().beginTransaction();
            player->updateOnDB();
            getDbms().endTransaction();
        }
        // Remove the player from the list of players.
        remPlayer(player);
        // Delete the player.
        delete (player);
    }
}

void Mud::processNewConnection()
{
    // The file descriptor for the new socket.
    int socketFileDescriptor;
    struct sockaddr_in socketAddress;
    socklen_t socketAddressSize = sizeof(socketAddress);

    // Loop until all outstanding connections are accepted.
    while (true)
    {
        try
        {
            socketFileDescriptor = accept(_servSocket, (struct sockaddr *) &socketAddress, &socketAddressSize);

            // A bad socket probably means no more connections are outstanding.
            if (socketFileDescriptor == kNoSocketIndicator)
            {
                // blocking is OK - we have accepted all outstanding connections.
                if (errno == EWOULDBLOCK)
                {
                    return;
                }
                throw std::runtime_error("ACCEPT");
            }

            // Here on successful accept - make sure socket doesn't block.
#ifdef __linux__
            if (fcntl(socketFileDescriptor, F_SETFL, FNDELAY) == -1)
            {
                throw std::runtime_error("FCNTL on player socket");
            }
#elif __APPLE__
            if (fcntl(socketFileDescriptor, F_SETFL, FNDELAY) == -1)
            {
                throw std::runtime_error("FCNTL on player socket");
            }
#elif __CYGWIN__

            int flags = fcntl(_servSocket, F_GETFL, 0);
            //if (fcntl(_servSocket, F_SETFL, flags | O_NONBLOCK | O_RDWR | O_NOCTTY | O_NDELAY) == -1)
            if (fcntl(_servSocket, F_SETFL, flags | O_NDELAY | O_NONBLOCK) == -1)
            {
                throw std::runtime_error("FCNTL on player socket");
            }
#elif _WIN32
            u_long imode = 1;
            if (ioctlsocket(_servSocket, FIONBIO, &imode) == -1)
            {
                throw std::runtime_error("FCNTL on Control Socket");
            }
#endif

            std::string address = inet_ntoa(socketAddress.sin_addr);
            int port = ntohs(socketAddress.sin_port);

            // Immediately close connections from blocked IP addresses.
            if (blockedIPs.find(address) != blockedIPs.end())
            {
                LogMessage(kMSys, "Rejected connection from " + address + "!");
                closeSocket(socketFileDescriptor);
                continue;
            }

            Player * player = new Player(socketFileDescriptor, port, address);

            // Insert the player in the list of players.
            addPlayer(player);

            LogMessage(kMSys, "#--------- New Connection ---------#");
            LogMessage(kMSys, "     Socket  : " + ToString(socketFileDescriptor));
            LogMessage(kMSys, "     Address : " + address);
            LogMessage(kMSys, "     Port    : " + ToString(port));
            LogMessage(kMSys, "#----------------------------------#");

            // Prepare to receive the player name.
            AdvanceCharacterCreation(player, ConnectionState::AwaitingName);
        }
        catch (std::exception & e)
        {
            LogError("Error during processing a new connection.");
            LogError("Error : " + std::string(e.what()));
            break;
        }
    }
}

bool Mud::initComunications()
{
    struct sockaddr_in socketAddress;
#ifdef _WIN32
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(0x0202, &wsaData);
    if (iResult != 0)
    {
        throw std::runtime_error("WSAStartup failed: "+ ToString(iResult));
    }
#endif

    // Create the control socket.
    if ((_servSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Creating Control Socket");
        return false;
    }

    // Make sure socket doesn't block.
#ifdef __linux__
    if (fcntl(_servSocket, F_SETFL, FNDELAY) == -1)
    {
        throw std::runtime_error("FCNTL on Control Socket");
    }
#elif __APPLE__
    if (fcntl(_servSocket, F_SETFL, FNDELAY) == -1)
    {
        throw std::runtime_error("FCNTL on Control Socket");
    }
#elif __CYGWIN__

    int flags = fcntl(_servSocket, F_GETFL, 0);
    //if (fcntl(_servSocket, F_SETFL, flags | O_NONBLOCK | O_RDWR | O_NOCTTY | O_NDELAY) == -1)
    if (fcntl(_servSocket, F_SETFL, flags | O_NDELAY | O_NONBLOCK) == -1)
    {
        perror("FCNTL on Control Socket");
        return false;
    }
#elif _WIN32
    u_long imode = 1;
    if (ioctlsocket(_servSocket, FIONBIO, &imode) == -1)
    {
        throw std::runtime_error("FCNTL on Control Socket");
    }
#endif

    // Zero it.
    struct linger ld = linger();

    // Don't allow closed sockets to linger.
    if (setsockopt(_servSocket, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof ld) < 0)
    {
        perror("Setsockopt (SO_LINGER)");
        return false;
    }

    int x = 1;

    // Allow address reuse.
    if (setsockopt(_servSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof x) == -1)
    {
        perror("Setsockopt (SO_REUSEADDR)");
        return false;
    }

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(kPort);
    socketAddress.sin_addr.s_addr = INADDR_ANY; // Change to listen on a specific adapter.

    // Bind the socket to our connection port.
    if (::bind(_servSocket, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0)
    {
        perror("BIND");
        return false;
    }

    // Listen for connections
    // Note: SOMAXCONN is the backlog count.
    if (listen(_servSocket, SOMAXCONN) == -1)
    {
        perror("LISTEN");
        return false;
    }

    // Standard termination signals.
    signal(SIGINT, Bailout);
    signal(SIGTERM, Bailout);
    signal(SIGHUP, Bailout);

    return true;
}

void Mud::closeComunications()
{
    if (_servSocket != kNoSocketIndicator)
    {
        closeSocket(_servSocket);
    }
}

void Mud::setupDescriptor(Player * player)
{
    // Don't bother if connection is closed.
    if (player->checkConnection())
    {
        _maxDesc = std::max(_maxDesc, player->getSocket());
        // Don't take input if they are closing down.
        if (!player->closing)
        {
            FD_SET(player->getSocket(), &in_set);
            FD_SET(player->getSocket(), &exc_set);
        }
        // We are only interested in writing to sockets we have something for.
        if (player->hasPendingOutput())
        {
            FD_SET(player->getSocket(), &out_set);
        }
    }
}

void Mud::processDescriptor(Player * player)
{
    // Handle exceptions.
    if (player->checkConnection())
    {
        if (FD_ISSET(player->getSocket(), &exc_set))
        {
            player->processException();
        }
    }

    // Look for ones we can read from, provided they aren't closed.
    if (player->checkConnection())
    {
        if (FD_ISSET(player->getSocket(), &in_set))
        {
            player->processRead();
        }
    }

    // Look for ones we can write to, provided they aren't closed.
    if (player->checkConnection())
    {
        if (FD_ISSET(player->getSocket(), &out_set))
        {
            player->processWrite();
        }
    }
}

void Mud::closeSocket(int socket)
{
#ifdef __linux__
    close(socket);
#elif __APPLE__
    close(socket);
#elif __CYGWIN__
    close(socket);
#elif _WIN32
    closesocket(socket);
#endif
}

void Mud::shutDown()
{
    _shutdownSignal = true;
}

void Mud::mainLoop()
{
    time(&_bootTime);
    // Set up timeout interval.
    struct timeval timeoutVal;
    timeoutVal.tv_sec = 0; // seconds
    timeoutVal.tv_usec = 5e05;  // microseconds
    LogMessage(kMSys, "Waiting for Connections...");
    // Loop processing input, output, events.
    // We will go through this loop roughly every timeout seconds.
    do
    {
        // Check if an hour has passed.
        if (this->getUpdater().hasHourPassed())
        {
            // Update mud time.
            this->getUpdater().updateTime();
            // Update mud mobile.
            this->getUpdater().updateMobilesHour();
            // Update items.
            this->getUpdater().updateItems();
        }
        if (this->getUpdater().hasTicPassed())
        {
            // Update players.
            this->getUpdater().updatePlayers();
            // Update mobiles.
            this->getUpdater().updateMobiles();
        }

        // Perform characters pending actions.
        this->getUpdater().performActions();

        // Delete players who have closed their comms.
        this->removeInactivePlayers();

        // Get ready for "select" function.
        FD_ZERO(&in_set);
        FD_ZERO(&out_set);
        FD_ZERO(&exc_set);

        // Add our control socket, needed for new connections.
        FD_SET(_servSocket, &in_set);

        // Set the max file descriptor to the server socket.
        _maxDesc = _servSocket;

        // Set bits in in_set, out_set etc. for each connected player.
        for (auto iterator : mudPlayers)
        {
            this->setupDescriptor(iterator);
        }

        // Check for activity, timeout after 'timeout' seconds.
        int activity = select((_maxDesc + 1), &in_set, &out_set, &exc_set, &timeoutVal);
        if ((activity < 0) && (errno != EINTR))
        {
            perror("Select");
        }

        // Check if there are new connections on control port.
        if (FD_ISSET(_servSocket, &in_set))
        {
            this->processNewConnection();
        }

        // Handle all player input/output.
        for (auto iterator : mudPlayers)
        {
            this->processDescriptor(iterator);
        }
    }
    while (!_shutdownSignal);

    // Game over - Tell them all.
    broadcastMsg(0, "\nGame shut down!\n");
}

void Bailout(int signal)
{
    std::cout << "\n";
    LogMessage(kMSys, "Received signal " + ToString(signal) + "!");
    Mud::getInstance().shutDown();
}
