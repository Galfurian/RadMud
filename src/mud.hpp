/// @file   mud.hpp
/// @brief  Define the mud class.
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

#ifndef MUD_HPP
#define MUD_HPP

#include "area.hpp"
#include "building.hpp"
#include "commands.hpp"
#include "constants.hpp"
#include "continent.hpp"
#include "mobile.hpp"
#include "player.hpp"
#include "production.hpp"
#include "profession.hpp"
#include "room.hpp"
#include "skill.hpp"
#include "sqlite/SQLiteDbms.hpp"
#include "telnet.hpp"
#include "updater.hpp"
#include "utilities/table.hpp"
#include "writing.hpp"

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

        /// List of all connected players.
        PlayerList mudPlayers;
        /// List all the mobile.
        MobileMap mudMobiles;
        /// List of all items.
        ItemMap mudItems;
        /// List of all the rooms.
        RoomMap mudRooms;
        /// List all the items model.
        ModelMap mudModels;
        /// List of all the areas.
        AreaMap mudAreas;
        /// List of all the races.
        RaceMap mudRaces;
        /// List of all the factions.
        FactionMap mudFactions;
        /// List of all the skills.
        SkillMap mudSkills;
        /// List of all the writings.
        WritingMap mudWritings;
        /// List of all the corpses.
        ItemList mudCorpses;
        /// List of all the continents.
        ContinentMap mudContinents;
        /// List of all the materials.
        MaterialMap mudMaterials;
        /// List of all the professions.
        ProfessionMap mudProfessions;
        /// List of all the productions.
        ProductionMap mudProductions;
        /// List of all the liquids.
        LiquidMap mudLiquids;
        /// List of all the travelling points.
        TravelPointMap mudTravelPoints;
        /// Blocked IP addresses.
        std::set<std::string> blockedIPs;
        /// Bad player names.
        std::set<std::string> badNames;
        /// Mud news.
        std::map<std::string, std::string> mudNews;
        /// List of commands (eg. look, quit, north etc.).
        std::vector<Command> mudCommands;
        /// Mud possible directions.
        DirectionMap mudDirections;
        /// Map of things to do for various connection states.
        StateActionMap mudStateActions;
        /// Map of buildings schematic.
        BuildingMap mudBuildings;

        /// @brief Update all the player on the database.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool savePlayers();

        /// @brief Update all the items on the database.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool saveItems();

        /// @brief Update all the rooms on the database.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool saveRooms();

        /// @defgroup GlobalAddRemove Global Add and Remove Functions
        /// @brief All the functions necessary to add and remove objects from their correspondent global list.
        /// @{

        /// Add a player to the list of connected players.
        bool addPlayer(Player * player);
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
        void addCorpse(Item * corpse);
        /// Remove the given corpse from the mud.
        bool remCorpse(Item * corpse);
        /// Add the given model to the mud.
        bool addModel(Model & model);
        /// Add the given area to the mud.
        bool addArea(Area * area);
        /// Add the given race to the mud.
        bool addRace(Race & race);
        /// Add the given faction to the mud.
        bool addFaction(Faction & faction);
        /// Add the given skill to the mud.
        bool addSkill(Skill & skill);
        /// Add the given writing to the mud.
        bool addWriting(Writing * writing);
        /// Add the given continent to the mud.
        bool addContinent(Continent * continent);
        /// Add the given material to the mud.
        bool addMaterial(Material & material);
        /// Add the given profession to the mud.
        bool addProfession(Profession & profession);
        /// Add the given production to the mud.
        bool addProduction(Production & production);
        /// Add the given liquid to the mud.
        bool addLiquid(Liquid & liquid);
        /// Add the given travel point to the mud.
        bool addTravelPoint(Room * source, Room * target);
        /// Add a command to the mud.
        void addCommand(Command & command);
        /// Add a direction to the mud.
        bool addDirection(std::string name, Direction direction);
        /// Add a state action to the mud.
        bool addStateAction(ConnectionState state, ActionHandler action);
        /// Add a building to the mud.
        bool addBuilding(Building & building);
        ///@}

        /// @defgroup GlobalFind Global Find Functions
        /// @brief All the functions necessary to find objects from their correspondent global list.
        /// @{

        /// Find an item given its vnum.
        Item * findItem(int vnum);
        /// Find a model given its vnum.
        Model * findModel(int vnum);
        /// Find a mobile given his id.
        Mobile * findMobile(std::string id);
        /// Find a player given his name.
        Player * findPlayer(std::string name);
        /// Find an area given its vnum.
        Area * findArea(int vnum);
        /// Find a room given its vnum.
        Room * findRoom(int vnum);
        /// Find a room given its coordinates, separately.
        Room * findRoom(int x, int y, int z);
        /// Find a room given its coordinates, as a coordinates structure.
        Room * findRoom(Coordinates coord);
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
        /// Find the action handler given a connection state.
        ActionHandler & findStateAction(ConnectionState state);
        /// Find a building given its name.
        Building * findBuilding(std::string name);
        /// Find a building given the vnum of the model to build.
        Building * findBuilding(int vnum);
        /// Find the direction.
        Direction findDirection(std::string direction, bool exact);
        ///@}

        /// @brief Get the singleton istance of the Mud.
        /// @return The static and uniquie Mud variable.
        static Mud & getInstance();

        /// @brief Get the singleton istance of the DMBS.
        /// @return The static and uniquie DBMS variable.
        static SQLiteDbms & getDbms();

        /// @brief Get the singleton istance of the Updater.
        /// @return The static and uniquie Updater variable.
        static MudUpdater & getUpdater();

        /// @brief Return the list of all the players plus all the mobiles.
        /// @return The joined list of players and mobiles.
        CharacterList getCharacterList();

        /// @brief Get the time value of when the mud has been booted.
        /// @return The time value.
        time_t getBootTime();

        /// @brief Returns the current maximum vnum used for rooms.
        /// @return The maximum rooms vnum.
        int getMaxVnumRoom();

        /// @brief Returns the current maximum vnum used for items.
        /// @return The maximum items vnum.
        int getMaxVnumItem();

        /// @brief Returns the current minimum vnum used for corpses.
        /// @return The minimum corpses vnum.
        int getMinVnumCorpse();

        /// @brief Function which checks if the provided socket is closed.
        /// @param socket The socket that has to be checked.
        /// @return <b>True</b> if the socket is closed,<br>
        ///         <b>False</b> Otherwise.
        bool isSocketClosed(int socket);

        /// @brief Send message to all connected players.
        /// @param level   The level of the player: 0 normal, 1 admin.
        /// @param message Message to send.
        void broadcastMsg(int level, std::string message);

        /// @brief Remove players that are disconnected or about to leave the MUD.
        void removeInactivePlayers();

        /// @brief New player has connected.
        void processNewConnection();

        /// @brief Set up communications and get ready to listen for connection.
        /// @return <b>true</b> if there is no error, <b>false</b> otherwise.
        bool initComunications();

        /// @brief Close listening port.
        void closeComunications();

        /// @brief Handle all the comunication descriptor, it's the socket value.
        void setupDescriptor(Player * player);

        /// @brief Handle communications with player using descriptor previusly set.
        void processDescriptor(Player * player);

        /// @brief Close a socket port.
        /// @param socket The port to close.
        void closeSocket(int socket);

        /// @brief Shut down the mud.
        void shutDown();

        /// @brief Main processing loop.
        void mainLoop();

    private:
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
};

/// @brief Here when a signal is raised.
/// @param signal Signal received from player.
void Bailout(int signal);

#endif