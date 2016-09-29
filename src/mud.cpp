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

#include <unistd.h>
#include <signal.h>

#include "protocol.hpp"
#include "utilities/stopwatch.hpp"
#include "utilities/CMacroWrapper.hpp"

/// Input file descriptor.
static fd_set in_set;
/// Output file descriptor.
static fd_set out_set;
/// Exceptino file descriptor.
static fd_set exc_set;

void Bailout(int signal)
{
    std::cout << "\n";
    Logger::log(LogLevel::Global, "Received signal " + ToString(signal) + "!");
    Mud::instance().shutDownSignal();
}

Mud::Mud() :
    mudPort(4000),
    _servSocket(-1),
    _maxDesc(-1),
    _shutdownSignal(),
    _bootTime(time(NULL)),
    _maxVnumRoom(),
    _maxVnumItem(),
    _minVnumCorpses()
{
    // Nothing to do.
}

Mud::~Mud()
{
    Logger::log(LogLevel::Global, "Freeing memory occupied by players...");
    for (auto iterator : Mud::instance().mudPlayers)
    {
        delete (iterator);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by mobiles...");
    for (auto iterator : Mud::instance().mudMobiles)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by items...");
    for (auto iterator : Mud::instance().mudItems)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by rooms...");
    for (auto iterator : Mud::instance().mudRooms)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by areas...");
    for (auto iterator : Mud::instance().mudAreas)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by writings...");
    for (auto iterator : Mud::instance().mudWritings)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by corpses...");
    for (auto iterator : Mud::instance().mudCorpses)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by item models...");
    for (auto iterator : Mud::instance().mudItemModels)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by races...");
    for (auto iterator : Mud::instance().mudRaces)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by factions...");
    for (auto iterator : Mud::instance().mudFactions)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by liquids...");
    for (auto iterator : Mud::instance().mudLiquids)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by professions...");
    for (auto iterator : Mud::instance().mudProfessions)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by productions...");
    for (auto iterator : Mud::instance().mudProductions)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by materials...");
    for (auto iterator : Mud::instance().mudMaterials)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by continents...");
    for (auto iterator : Mud::instance().mudContinents)
    {
        delete (iterator.second);
    }
    Logger::log(LogLevel::Global, "Freeing memory occupied by skills...");
    for (auto iterator : Mud::instance().mudSkills)
    {
        delete (iterator.second);
    }
}

Mud & Mud::instance()
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
    Logger::log(LogLevel::Global, "Saving information on Database for : Players...");
    result &= Mud::instance().savePlayers();
    Logger::log(LogLevel::Global, "Saving information on Database for : Items...");
    result &= Mud::instance().saveItems();
    Logger::log(LogLevel::Global, "Saving information on Database for : Rooms...");
    result &= Mud::instance().saveRooms();
    return result;
}

bool Mud::addPlayer(Player * player)
{
    return mudPlayers.insert(player).second;
}

bool Mud::remPlayer(Player * player)
{
    return (FindErase(mudPlayers, player) != mudPlayers.end());
}

bool Mud::addMobile(Mobile * mobile)
{
    return mudMobiles.insert(std::make_pair(mobile->id, mobile)).second;
}

bool Mud::remMobile(Mobile * mobile)
{
    return (FindErase(mudMobiles, mobile->id) != mudMobiles.end());
}

bool Mud::addItem(Item * item)
{
    if (mudItems.insert(std::make_pair(item->vnum, item)).second)
    {
        _maxVnumItem = std::max(_maxVnumItem, item->vnum);
        return true;
    }
    else
    {
        return false;
    }
}

bool Mud::remItem(Item * item)
{
    for (auto it = mudItems.begin(); it != mudItems.end(); ++it)
    {
        if (it->second->vnum == item->vnum)
        {
            mudItems.erase(it);
            return true;
        }
    }
    return false;
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
    return (FindErase(mudRooms, room->vnum) != mudRooms.end());
}

bool Mud::addCorpse(Item * corpse)
{
    if (mudCorpses.insert(std::make_pair(corpse->vnum, corpse)).second)
    {
        _minVnumCorpses = std::min(_minVnumCorpses, corpse->vnum);
        return true;
    }
    else
    {
        return false;
    }
}

bool Mud::remCorpse(Item * corpse)
{
    for (auto it = mudCorpses.begin(); it != mudCorpses.end(); ++it)
    {
        if (it->second->vnum == corpse->vnum)
        {
            mudCorpses.erase(it);
            return true;
        }
    }
    return false;
}

bool Mud::addItemModel(ItemModel * model)
{
    if (model == nullptr)
    {
        return false;
    }
    return mudItemModels.insert(std::make_pair(model->vnum, model)).second;
}

bool Mud::addArea(Area * area)
{
    return mudAreas.insert(std::make_pair(area->vnum, area)).second;
}

bool Mud::addRace(Race * race)
{
    if (race == nullptr)
    {
        return false;
    }
    return mudRaces.insert(std::make_pair(race->vnum, race)).second;
}

bool Mud::addFaction(Faction * faction)
{
    if (faction == nullptr)
    {
        return false;
    }
    return mudFactions.insert(std::make_pair(faction->vnum, faction)).second;
}

bool Mud::addSkill(Skill * skill)
{
    if (skill == nullptr)
    {
        return false;
    }
    return mudSkills.insert(std::make_pair(skill->vnum, skill)).second;
}

bool Mud::addWriting(Writing * writing)
{
    return mudWritings.insert(std::make_pair(writing->vnum, writing)).second;
}

bool Mud::addContinent(Continent * continent)
{
    return mudContinents.insert(std::make_pair(continent->vnum, continent)).second;
}

bool Mud::addMaterial(Material * material)
{
    if (material == nullptr)
    {
        return false;
    }
    return mudMaterials.insert(std::make_pair(material->vnum, material)).second;
}

bool Mud::addProfession(Profession * profession)
{
    if (profession == nullptr)
    {
        return false;
    }
    return mudProfessions.insert(std::make_pair(profession->vnum, profession)).second;
}

bool Mud::addProduction(Production * production)
{
    if (production == nullptr)
    {
        return false;
    }
    return mudProductions.insert(std::make_pair(production->vnum, production)).second;
}

bool Mud::addLiquid(Liquid * liquid)
{
    if (liquid == nullptr)
    {
        return false;
    }
    return mudLiquids.insert(std::make_pair(liquid->vnum, liquid)).second;
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
    std::map<std::string, Mobile *>::iterator iterator = mudMobiles.find(id);
    if (iterator != mudMobiles.end())
    {
        return iterator->second;
    }
    return nullptr;
}

ItemModel * Mud::findItemModel(int vnum)
{
    auto iterator = mudItemModels.find(vnum);
    if (iterator != mudItemModels.end())
    {
        return iterator->second;
    }
    else
    {
        return nullptr;
    }
}

Item * Mud::findItem(int vnum)
{
    std::map<int, Item *>::iterator iterator = mudItems.find(vnum);
    if (iterator != mudItems.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Area * Mud::findArea(int vnum)
{
    auto iterator = mudAreas.find(vnum);
    if (iterator != mudAreas.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Room * Mud::findRoom(int vnum)
{
    std::map<int, Room *>::iterator iterator = mudRooms.find(vnum);
    if (iterator != mudRooms.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Race * Mud::findRace(int vnum)
{
    std::map<int, Race *>::iterator iterator = mudRaces.find(vnum);
    if (iterator != mudRaces.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Race * Mud::findRace(std::string name)
{
    for (std::map<int, Race *>::iterator iterator = mudRaces.begin(); iterator != mudRaces.end();
         ++iterator)
    {
        if (ToLower(iterator->second->name) == ToLower(name))
        {
            return iterator->second;
        }
    }
    return nullptr;
}

Faction * Mud::findFaction(int vnum)
{
    std::map<int, Faction *>::iterator iterator = mudFactions.find(vnum);
    if (iterator != mudFactions.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Faction * Mud::findFaction(std::string name)
{
    for (std::map<int, Faction *>::iterator iterator = mudFactions.begin();
         iterator != mudFactions.end(); ++iterator)
    {
        if (ToLower(iterator->second->name) == ToLower(name))
        {
            return iterator->second;
        }
    }
    return nullptr;
}

Skill * Mud::findSkill(int vnum)
{
    std::map<int, Skill *>::iterator iterator = mudSkills.find(vnum);
    if (iterator != mudSkills.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Writing * Mud::findWriting(int vnum)
{
    std::map<int, Writing *>::iterator iterator = mudWritings.find(vnum);
    if (iterator != mudWritings.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Item * Mud::findCorpse(int vnum)
{
    std::map<int, Item *>::iterator iterator = mudCorpses.find(vnum);
    if (iterator != mudCorpses.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Continent * Mud::findContinent(int vnum)
{
    std::map<int, Continent *>::iterator iterator = mudContinents.find(vnum);
    if (iterator != mudContinents.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Material * Mud::findMaterial(int vnum)
{
    std::map<int, Material *>::iterator iterator = mudMaterials.find(vnum);
    if (iterator != mudMaterials.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Profession * Mud::findProfession(unsigned int vnum)
{
    std::map<unsigned int, Profession *>::iterator iterator = mudProfessions.find(vnum);
    if (iterator != mudProfessions.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Profession * Mud::findProfession(std::string command)
{
    for (std::map<unsigned int, Profession *>::iterator iterator = mudProfessions.begin();
         iterator != mudProfessions.end(); ++iterator)
    {
        if (ToLower(iterator->second->command) == ToLower(command))
        {
            return iterator->second;
        }
    }
    return nullptr;
}

Production * Mud::findProduction(int vnum)
{
    std::map<int, Production *>::iterator iterator = mudProductions.find(vnum);
    if (iterator != mudProductions.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Production * Mud::findProduction(std::string name)
{
    for (std::map<int, Production *>::iterator iterator = mudProductions.begin();
         iterator != mudProductions.end(); ++iterator)
    {
        if (ToLower(iterator->second->name) == ToLower(name))
        {
            return iterator->second;
        }
    }
    return nullptr;
}

Liquid * Mud::findLiquid(int vnum)
{
    std::map<int, Liquid *>::iterator iterator = mudLiquids.find(vnum);
    if (iterator != mudLiquids.end())
    {
        return iterator->second;
    }
    return nullptr;
}

Room * Mud::findTravelPoint(Room * room)
{
    std::map<Room *, Room *>::iterator iterator = mudTravelPoints.find(room);
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
    for (std::map<int, Building>::iterator iterator = mudBuildings.begin();
         iterator != mudBuildings.end(); ++iterator)
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
    for (std::map<int, Building>::iterator iterator = mudBuildings.begin();
         iterator != mudBuildings.end(); ++iterator)
    {
        if (iterator->second.buildingModel->vnum == vnum)
        {
            return &(iterator->second);
        }
    }
    return nullptr;
}

Direction Mud::findDirection(const std::string & direction, bool exact)
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

bool Mud::runMud()
{
    // Open logging file.
    if (!Logger::instance().openLog(kSystemDir + GetDate() + ".log"))
    {
        std::cerr << "Can't create the logging file." << std::endl;
        return false;
    }

    if (!this->startMud())
    {
        Logger::log(LogLevel::Error, "Something gone wrong during the boot.");
        return false;
    }
    // Set up timeout interval.
    struct timeval timeoutVal;
    timeoutVal.tv_sec = 0; // seconds
    timeoutVal.tv_usec = 5e05;  // microseconds
    Logger::log(LogLevel::Global, "Waiting for Connections...");
    // Loop processing input, output, events.
    // We will go through this loop roughly every timeout seconds.
    do
    {
        // Check if an hour has passed.
        if (MudUpdater::instance().hasHourPassed())
        {
            // Update mud time.
            MudUpdater::instance().updateTime();
            // Update mud mobile.
            MudUpdater::instance().updateMobilesHour();
            // Update items.
            MudUpdater::instance().updateItems();
        }
        if (MudUpdater::instance().hasTicPassed())
        {
            // Update players.
            MudUpdater::instance().updatePlayers();
            // Update mobiles.
            MudUpdater::instance().updateMobiles();
        }

        // Perform characters pending actions.
        MudUpdater::instance().performActions();

        // Delete players who have closed their comms.
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
        if (CMacroWrapper::FdIsSet(_servSocket, &in_set))
        {
            this->processNewConnection();
        }

        // Handle all player input/output.
        for (auto iterator : mudPlayers)
        {
            this->processDescriptor(iterator);
        }
    } while (!_shutdownSignal);

    if (!this->stopMud())
    {
        Logger::log(LogLevel::Error, "Something gone wrong during the shutdown.");
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

bool Mud::checkSocket(const int & socket) const
{
    int error_code;
    socklen_t error_code_size = sizeof(error_code);
    return getsockopt(socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size) == 0;
}

bool Mud::closeSocket(const int & socket) const
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

int Mud::getMaxVnumRoom() const
{
    return _maxVnumRoom;
}

int Mud::getMaxVnumItem() const
{
    return _maxVnumItem;
}

int Mud::getMinVnumCorpse() const
{
    return _minVnumCorpses;
}

void Mud::broadcastMsg(const int & level, const std::string & message) const
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
    std::set<Player *> toRemove;
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
        Logger::log(LogLevel::Global, "Removing inactive player : " + player->getName());
        // Only if the player has successfully logged in, save its state on DB.
        if (player->logged_in)
        {
            SQLiteDbms::instance().beginTransaction();
            player->updateOnDB();
            SQLiteDbms::instance().endTransaction();
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
            socketFileDescriptor = accept(
                _servSocket,
                (struct sockaddr *) &socketAddress,
                &socketAddressSize);

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
                Logger::log(LogLevel::Global, "Rejected connection from " + address + "!");
                closeSocket(socketFileDescriptor);
                continue;
            }

            Player * player = new Player(socketFileDescriptor, port, address);

            // Insert the player in the list of players.
            addPlayer(player);

            Logger::log(LogLevel::Global, "#--------- New Connection ---------#");
            Logger::log(LogLevel::Global, " Socket  : " + ToString(socketFileDescriptor));
            Logger::log(LogLevel::Global, " Address : " + address);
            Logger::log(LogLevel::Global, " Port    : " + ToString(port));
            Logger::log(LogLevel::Global, "#----------------------------------#");

            // Activate the procedure of negotiation.
            NegotiateProtocol(player, ConnectionState::NegotiatingMSDP);
        }
        catch (std::exception & e)
        {
            Logger::log(LogLevel::Error, "Error during processing a new connection.");
            Logger::log(LogLevel::Error, "Error : " + std::string(e.what()));
            break;
        }
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
            CMacroWrapper::FdSet(player->getSocket(), &in_set);
            CMacroWrapper::FdSet(player->getSocket(), &exc_set);
        }
        // We are only interested in writing to sockets we have something for.
        if (player->hasPendingOutput())
        {
            CMacroWrapper::FdSet(player->getSocket(), &out_set);
        }
    }
}

void Mud::processDescriptor(Player * player)
{
    // Handle exceptions.
    if (player->checkConnection())
    {
        if (CMacroWrapper::FdIsSet(player->getSocket(), &exc_set))
        {
            player->processException();
        }
    }
    // Look for ones we can read from, provided they aren't closed.
    if (player->checkConnection())
    {
        if (CMacroWrapper::FdIsSet(player->getSocket(), &in_set))
        {
            player->processRead();
        }
    }
    // Look for ones we can write to, provided they aren't closed.
    if (player->checkConnection())
    {
        if (CMacroWrapper::FdIsSet(player->getSocket(), &out_set))
        {
            player->processWrite();
        }
    }
}

bool Mud::initVariables()
{
    // First map all the mud directions.
    Mud::instance().addDirection("north", Direction::North);
    Mud::instance().addDirection("south", Direction::South);
    Mud::instance().addDirection("west", Direction::West);
    Mud::instance().addDirection("east", Direction::East);
    Mud::instance().addDirection("up", Direction::Up);
    Mud::instance().addDirection("down", Direction::Down);

    // Init the updater timers.
    MudUpdater::instance().initTimers();

    // Set the boot time.
    time(&_bootTime);

    return true;
}

bool Mud::initDatabase()
{
    if (!SQLiteDbms::instance().openDatabase())
    {
        Logger::log(LogLevel::Error, "Error opening database!");
        return false;
    }
    if (!SQLiteDbms::instance().loadTables())
    {
        Logger::log(LogLevel::Error, "Error loading tables!");
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
    socketAddress.sin_port = htons(mudPort);
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

bool Mud::closeComunications()
{
    if (_servSocket != kNoSocketIndicator)
    {
        return this->closeSocket(_servSocket);
    }
    return false;
}

bool Mud::startMud()
{
    // Create a stopwatch for general timing information.
    Stopwatch<std::chrono::milliseconds> stopwatch("Boot");
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
    Logger::log(LogLevel::Global, "             XXXXXXXXXXXXX                ");
    Logger::log(LogLevel::Global, "  /'--_###XXXXXXXXXXXXXXXXXXX###_--'\\    ");
    Logger::log(LogLevel::Global, "  \\##/#/#XXXXXXXXXXXXXXXXXXXXX#\\#\\##/  ");
    Logger::log(LogLevel::Global, "   \\/#/#XXXXXXXXXXXXXXXXXXXXXXX#\\#\\/   ");
    Logger::log(LogLevel::Global, "    \\/##XXXXXXXXXXXXXXXXXXXXXXX##\\/     ");
    Logger::log(LogLevel::Global, "     ###XXXX  ''-.XXX.-''  XXXX###        ");
    Logger::log(LogLevel::Global, "       \\XXXX               XXXX/         ");
    Logger::log(LogLevel::Global, "         XXXXXXXXXXXXXXXXXXXXX            ");
    Logger::log(LogLevel::Global, "         XXXX XXXX X XXXX XXXX            ");
    Logger::log(LogLevel::Global, "         XXX # XXX X XXX # XXX            ");
    Logger::log(LogLevel::Global, "        /XXXX XXX XXX XXX XXXX\\          ");
    Logger::log(LogLevel::Global, "       ##XXXXXXX X   X XXXXXXX##          ");
    Logger::log(LogLevel::Global, "      ##   XOXXX X   X XXXOX   ##         ");
    Logger::log(LogLevel::Global, "      ##    #XXXX XXX XXX #    ##         ");
    Logger::log(LogLevel::Global, "       ##..##  XXXXXXXXX  ##..##          ");
    Logger::log(LogLevel::Global, "        ###      XXXXX     ####           ");
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
    Logger::log(LogLevel::Global, "|                   RadMud                   |");
    Logger::log(LogLevel::Global, "| Created by : Enrico Fraccaroli.            |");
    Logger::log(LogLevel::Global, "| Date       : 29 September 2014             |");
    Logger::log(LogLevel::Global, "#--------------------------------------------#");
    Logger::log(LogLevel::Global, "Booting...");
    Logger::log(LogLevel::Global, "Initializing Mud Variables...");
    if (!this->initVariables())
    {
        Logger::log(LogLevel::Error, "Something gone wrong during variables initialization.");
        return false;
    }

    Logger::log(LogLevel::Global, "Initializing Commands...");
    LoadCommands();

    Logger::log(LogLevel::Global, "Initializing States...");
    LoadStates();

    Logger::log(LogLevel::Global, "Initializing MSDP States...");
    LoadProtocolStates();

    Logger::log(LogLevel::Global, "Initializing Database...");
    if (!this->initDatabase())
    {
        Logger::log(LogLevel::Error, "Something gone wrong during database initialization.");
        return false;
    }

    Logger::log(LogLevel::Global, "Initializing Communications...");
    if (!this->initComunications())
    {
        Logger::log(LogLevel::Error, "Something gone wrong during initialization of comunication.");
        return false;
    }

    Logger::log(LogLevel::Global, "Booting Done (" + ToString(stopwatch.elapsed()) + ").");
    return true;
}

bool Mud::stopMud()
{
    Stopwatch<std::chrono::milliseconds> stopwatch("Shutdown");

    Logger::log(LogLevel::Global, "Shutting down RadMud...");
    Logger::log(LogLevel::Global, "Closing Communications...");
    if (!Mud::instance().closeComunications())
    {
        Logger::log(LogLevel::Error, "The communication has not been closed correctly.");
    }

    Logger::log(LogLevel::Global, "Saving Mud Information...");
    if (!Mud::instance().saveMud())
    {
        Logger::log(LogLevel::Error, "Somwthing has gone wrong during data saving.");
    }

    Logger::log(LogLevel::Global, "Closing Database...");
    if (!SQLiteDbms::instance().closeDatabase())
    {
        Logger::log(LogLevel::Error, "The database has not been closed correctly.");
    }
    Logger::log(LogLevel::Global, "Shutdown Completed (" + ToString(stopwatch.elapsed()) + ").");

    ///////////////////////////////////////////////////////////////////////////
    size_t bIn = MudUpdater::instance().getBandIn();
    size_t bOut = MudUpdater::instance().getBandOut();
    size_t bUnc = MudUpdater::instance().getBandUncompressed();

    // Print some statistics.
    Logger::log(LogLevel::Info, "");
    Logger::log(LogLevel::Info, "Statistics");
    Logger::log(LogLevel::Info, "    In            = " + ToString(bIn) + " Bytes.");
    Logger::log(LogLevel::Info, "    Output        = " + ToString(bOut) + " Bytes.");
    Logger::log(LogLevel::Info, "    Uncompressed  = " + ToString(bUnc) + " Bytes.");
    Logger::log(LogLevel::Info, "    Band. Saved   = " + ToString(bUnc - bOut) + " Bytes.");
    Logger::log(LogLevel::Info, "");
    return true;
}
