/// @file   mud.hpp
/// @brief  Define the mud class.
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

#pragma once

/// MUD Major Version.
#define RADMUD_MAJOR_VERSION 1
/// MUD Minor Version.
#define RADMUD_MINOR_VERSION 0
/// MUD Version.
#define RADMUD_VERSION       0

/// Indicator for no socket connected.
#define NO_SOCKET_COMMUNICATION -1

#include "building.hpp"
#include "production.hpp"
#include "profession.hpp"
#include "skill.hpp"
#include "updater.hpp"
#include "writing.hpp"
#include "material.hpp"
#include "area.hpp"
#include "room.hpp"
#include "continent.hpp"
#include "mobile.hpp"
#include "player.hpp"
#include "command.hpp"
#include "sqliteDbms.hpp"
#include "table.hpp"
#include "formatter.hpp"
#include "terrain.hpp"

class Direction;

#ifdef __linux__

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#elif __APPLE__

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#elif __CYGWIN__

#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#elif _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <windows.h>

#endif

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

/// @brief The main class of the entire mud.
/// @details
/// <h3>General</h3>
/// Basically every static and non-static object of the mud is stored
///  inside data structures inside this class.
/// Here you can find the main routine of the mud which handle the
///  advancement of time, execution of characters actions, degradation
///  of items and so on and so forth. The name of the function is
///  <b>mainLoop</b>.
///
/// <h3>Connection</h3>
/// All the connection functionalities are defined here.
/// Basically before starting the main loop the initCommunications function
///  opens the mud's socket. Then, at each iteration of the main loop, the
///  mud awaits for about 0.5 seconds for a connection (using select with
///  timeout).
/// After having managed all new connections, it handles the input/output
///  messages between the mud and the clients.
class Mud
{
private:
    /// The port at which the mud is listening.
    uint16_t mudPort;
    /// Socket for accepting new connections.
    int _servSocket;
    /// The max descriptor, in other terms, the max socket value.
    int _maxDesc;
    /// When set, the MUD shuts down.
    bool _shutdownSignal;
    /// Contains the time when the mud has been booted.
    time_t _bootTime;
    /// Highest value of vnum for rooms.
    int _maxVnumRoom;
    /// Highest value of vnum for items.
    int _maxVnumItem;
    /// Lowest value of vnum for corpses.
    int _minVnumCorpses;

    /// Mud weight measure.
    const std::string _mudMeasure;

    /// Mud database filename.
    const std::string _mudDatabaseName;

    /// Location of system files.
    const std::string _mudSystemDirectory;

    /// @brief Constructor.
    Mud();

    /// @brief Destructor.
    ~Mud();

public:
    /// @brief Disable Copy Construct.
    Mud(Mud const &) = delete;

    /// @brief Disable Move construct.
    Mud(Mud &&) = delete;

    /// @brief Disable Copy assign.
    Mud & operator=(Mud const &) = delete;

    /// @brief Disable Move assign.
    Mud & operator=(Mud &&) = delete;

    /// @brief Get the singleton istance of the Mud.
    /// @return The static and uniquie Mud variable.
    static Mud & instance();

    /// List of all connected players.
    std::list<Player *> mudPlayers;
    /// List all the mobile.
    std::map<std::string, Mobile *> mudMobiles;
    /// List of all items.
    std::map<int, Item *> mudItems;
    /// List of all the rooms.
    std::map<int, Room *> mudRooms;
    /// List all the items model.
    std::map<int, ItemModel *> mudItemModels;
    /// List of all the areas.
    std::map<int, Area *> mudAreas;
    /// List of all the races.
    std::map<int, Race *> mudRaces;
    /// List of all the factions.
    std::map<int, Faction *> mudFactions;
    /// List of all the skills.
    std::map<int, Skill *> mudSkills;
    /// List of all the writings.
    std::map<int, Writing *> mudWritings;
    /// List of all the corpses.
    std::map<int, Item *> mudCorpses;
    /// List of all the continents.
    std::map<int, Continent *> mudContinents;
    /// List of all the materials.
    std::map<int, Material *> mudMaterials;
    /// List of all the professions.
    std::map<unsigned int, Profession *> mudProfessions;
    /// List of all the productions.
    std::map<int, Production *> mudProductions;
    /// List of all the liquids.
    std::map<int, Liquid *> mudLiquids;
    /// List of all the travelling points.
    std::map<Room *, Room *> mudTravelPoints;
    /// Blocked IP addresses.
    std::set<std::string> blockedIPs;
    /// Bad player names.
    std::set<std::string> badNames;
    /// Mud news.
    std::map<std::string, std::string> mudNews;
    /// List of commands (eg. look, quit, north etc.).
    std::vector<Command> mudCommands;
    /// Mud possible directions.
    std::map<std::string, Direction> mudDirections;
    /// Map of buildings schematic.
    std::map<int, Building> mudBuildings;
    /// Map of buildings schematic.
    std::map<unsigned int, std::shared_ptr<Terrain>> mudTerrains;

    /// @brief Update all the player on the database.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool savePlayers();

    /// @brief Update all the items on the database.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool saveItems();

    /// @brief Update all the rooms on the database.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool saveRooms();

    /// @brief Update the mud dynimic information like: Players and Items.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool saveMud();

    /// @defgroup GlobalAddRemove Global Add and Remove Functions
    /// @brief All the functions necessary to add and remove objects from their correspondent global list.
    /// @{

    /// Add a player to the list of connected players.
    void addPlayer(Player * player);

    /// Remove a player from the list of connected players.
    bool remPlayer(Player * player);

    /// Add the given mobile to the mud.
    bool addMobile(Mobile * mobile);

    /// Remove the given mobile from the mud.
    bool remMobile(Mobile * mobile);

    /// Add the given item to the mud.
    bool addItem(Item * item);

    /// Remove the given item from the mud.
    bool remItem(Item * item);

    /// Add the given room to the mud.
    bool addRoom(Room * room);

    /// Remove the given room from the mud.
    bool remRoom(Room * room);

    /// Add the given corpse to the mud.
    bool addCorpse(Item * corpse);

    /// Remove the given corpse from the mud.
    bool remCorpse(Item * corpse);

    /// Add the given item model to the mud.
    bool addItemModel(ItemModel * model);

    /// Add the given area to the mud.
    bool addArea(Area * area);

    /// Add the given race to the mud.
    bool addRace(Race * race);

    /// Add the given faction to the mud.
    bool addFaction(Faction * faction);

    /// Add the given skill to the mud.
    bool addSkill(Skill * skill);

    /// Add the given writing to the mud.
    bool addWriting(Writing * writing);

    /// Add the given continent to the mud.
    bool addContinent(Continent * continent);

    /// Add the given material to the mud.
    bool addMaterial(Material * material);

    /// Add the given profession to the mud.
    bool addProfession(Profession * profession);

    /// Add the given production to the mud.
    bool addProduction(Production * production);

    /// Add the given liquid to the mud.
    bool addLiquid(Liquid * liquid);

    /// Add the given travel point to the mud.
    bool addTravelPoint(Room * source, Room * target);

    /// Add a command to the mud.
    void addCommand(const Command & command);

    /// Add a direction to the mud.
    bool addDirection(std::string name, const Direction & direction);

    /// Add a building to the mud.
    bool addBuilding(Building & building);

    /// Add a terrain to the mud.
    bool addTerrain(std::shared_ptr<Terrain> terrain);
    ///@}

    /// @defgroup GlobalFind Global Find Functions
    /// @brief All the functions necessary to find objects from their correspondent global list.
    /// @{

    /// Find an item given its vnum.
    Item * findItem(int vnum);

    /// Find an item model given its vnum.
    ItemModel * findItemModel(int vnum);

    /// Find a mobile given his id.
    Mobile * findMobile(std::string id);

    /// Find a player given his name.
    Player * findPlayer(const std::string & name);

    /// Find an area given its vnum.
    Area * findArea(int vnum);

    /// Find a room given its vnum.
    Room * findRoom(int vnum);

    /// Find a race given its vnum.
    Race * findRace(int vnum);

    /// Find a race given its name.
    Race * findRace(std::string name);

    /// Find a faction given its vnum.
    Faction * findFaction(int vnum);

    /// Find a faction given its name.
    Faction * findFaction(std::string name);

    /// Find a skill given its vnum.
    Skill * findSkill(int vnum);

    /// Find a writing given its vnum.
    Writing * findWriting(int vnum);

    /// Find a corpse given its vnum.
    Item * findCorpse(int vnum);

    /// Find a continent given its vnum.
    Continent * findContinent(int vnum);

    /// Find a material given its vnum.
    Material * findMaterial(int vnum);

    /// Find a profession given its vnum.
    Profession * findProfession(unsigned int vnum);

    /// Find a profession given its command.
    Profession * findProfession(std::string command);

    /// Find a production given its vnum.
    Production * findProduction(int vnum);

    /// Find a production given its name.
    Production * findProduction(std::string name);

    /// Find a liquid given its vnum.
    Liquid * findLiquid(int vnum);

    /// Find the destination room of a travel point, given its starting room.
    Room * findTravelPoint(Room * room);

    /// Find a building given its name.
    Building * findBuilding(std::string name);

    /// Find a building given the vnum of the model to build.
    Building * findBuilding(int vnum);

    /// Find a terrain given its vnum.
    std::shared_ptr<Terrain> findTerrain(unsigned int vnum);

    /// Find the direction.
    Direction findDirection(const std::string & direction, bool exact);
    ///@}

    /// @brief Main processing loop.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool runMud();

    /// @brief Activate the signal for shutting down the mud.
    void shutDownSignal();

    /// @brief Function which checks if the provided socket is still open.
    /// @param socket The socket that has to be checked.
    /// @return <b>True</b> if the socket is still open,<br>
    ///         <b>False</b> Otherwise.
    bool checkSocket(const int & socket) const;

    /// @brief Close a socket port.
    /// @param socket The port to close.
    /// @return <b>True</b> if the socket has been closed,<br>
    ///         <b>False</b> Otherwise.
    bool closeSocket(const int & socket) const;

    /// @brief Get the totale uptime.
    /// @return The uptime.
    double getUpTime() const;

    /// @brief Returns the current maximum vnum used for rooms.
    /// @return The maximum rooms vnum.
    int getMaxVnumRoom() const;

    /// @brief Returns the current maximum vnum used for items.
    /// @return The maximum items vnum.
    int getMaxVnumItem() const;

    /// @brief Returns the current minimum vnum used for corpses.
    /// @return The minimum corpses vnum.
    int getMinVnumCorpse() const;

    /// @brief Send message to all connected players.
    /// @param level   The level of the player: 0 normal, 1 admin.
    /// @param message Message to send.
    void broadcastMsg(const int & level, const std::string & message) const;

    /// @brief Provides the name of the measure for weight.
    std::string getWeightMeasure() const;

    /// @brief Provides the name of the database.
    std::string getMudDatabaseName() const;

    /// @brief Provides the path to the system directory.
    std::string getMudSystemDirectory() const;

private:
    /// @brief Remove players that are disconnected or about to leave the MUD.
    void removeInactivePlayers();

    /// @brief New player has connected.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool processNewConnection();

    /// @brief Handle all the comunication descriptor, it's the socket value.
    void setupDescriptor(Player * player);

    /// @brief Handle communications with player using descriptor previusly set.
    void processDescriptor(Player * player);

    /// @brief Set up the mud variables.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool initVariables();

    /// @brief Load data from the database.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool initDatabase();

    /// @brief Set up communications and get ready to listen for connection.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool initComunications();

    /// @brief Close listening port.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool closeComunications();

    /// @brief Boot up the mud.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool startMud();

    /// @brief Shut down the mud.
    /// @return <b>True</b> if there are no errors,<br>
    ///         <b>False</b> otherwise.
    bool stopMud();
};

/// @brief Here when a signal is raised.
/// @param signal Signal received from player.
void Bailout(int signal);
