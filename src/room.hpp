/// @file   room.hpp
/// @brief  Define the room class.
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

#ifndef ROOM_HPP
#define ROOM_HPP

#include <map>

#include "character.hpp"
#include "defines.hpp"
#include "exit.hpp"
#include "item.hpp"
#include "lua/lua_script.hpp"
#include "mobile.hpp"
#include "player.hpp"
#include "utilities/coordinates.hpp"
#include "utils.hpp"

class Item;
class Area;
class Continent;

/// @brief Holds details about room.
class Room
{
    public:
        /// The current room vnum.
        int vnum;
        /// The area where is located the room.
        Area * area;
        /// The continent where is located the room.
        Continent * continent;
        /// The current room coordinates.
        Coordinates<unsigned int> coord;
        /// The terrain where the room is situated.
        std::string terrain;
        /// The name of the room.
        std::string name;
        /// A long description of the room.
        std::string description;
        /// List of exits.
        ExitList exits;
        /// List of items in the room.
        ItemVector items;
        /// List of characters in the room.
        CharacterList characters;
        /// Integer that describe the flags of the room.
        unsigned int flags;

        /// @brief Constructor.
        Room();

        /// @brief Copy Constructor.
        ~Room();

        /// @brief Destructor.
        bool check(bool complete = false);

        /// @brief In addition to adding the the passed item to room's items list.
        /// @param item The item to add to the list.
        void addItem(Item * item);

        /// @brief Add the passed item to room's buildings list.
        /// @param item The item to add to the list.
        void addBuilding(Item * item);

        /// @brief Add the passed character to room's character list.
        /// @param character The character to add to the list.
        void addCharacter(Character * character);

        /// @brief Remove the passed item from room's item list.
        /// @param item The item to remove from the list.
        void removeItem(Item * item);

        /// @brief Remove the passed item from room's building list.
        /// @param item The building to remove from the list.
        void removeBuilding(Item * item);

        /// @brief Remove the passed character from room's character list.
        /// @param character The character to remove from the list.
        void removeCharacter(Character * character);

        /// @brief Select from the room all the players and return them.
        /// @param exception The exception from the list.
        /// @return The list of all the players in the room.
        PlayerList getAllPlayer(Character * exception);

        /// @brief Select from the room all the mobiles and return them.
        /// @param exception The exception from the list.
        /// @return The list of all the mobiles in the room.
        MobileList getAllMobile(Character * exception);

        /// @brief Save the room on database.
        /// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
        bool updateOnDB();

        /// @brief Remove the room from the database.
        /// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
        bool removeOnDB();

        /// @brief Search for the item in the room.
        /// @param search_parameter The item to search.
        /// @param number           Position of the item we want to look for.
        /// @return The item, if it's in the room.
        Item * findItem(std::string search_parameter, int & number);

        /// @brief Search for the building in the room.
        /// @param target The building to search.
        /// @param number Position of the building we want to look for.
        /// @return The building, if it's in the room.
        Item * findBuilding(std::string target, int & number);

        /// @brief Search for the building in the room.
        /// @param buildingVnum The vnum of the building to search.
        /// @return The building, if it's in the room.
        Item * findBuilding(int buildingVnum);

        /// @brief Search for the buildings of the given type inside the room.
        /// @param type The type of the buildings.
        /// @return The buildings list.
        ItemVector findBuildings(ModelType type);

        /// @brief Search for the character in the room.
        /// @param target    The character to search.
        /// @param number    Number of the character we are looking for.
        /// @param exception The character to skip in the search.
        /// @return The character, if it's in the room.
        Character * findCharacter(std::string target, int & number, Character * exception = nullptr);

        /// @brief Search for the player in the room.
        /// @param target    The player to search.
        /// @param exception The player to skip in the search.
        /// @param number    Number of the player we are looking for.
        /// @return The player, if it's in the room.
        Player * findPlayer(std::string target, int & number, Player * exception = nullptr);

        /// @brief Search for the mobile in the room.
        /// @param target    The mobile to search.
        /// @param exception The mobile to skip in the search.
        /// @param number    Number of the mobile we are looking for.
        /// @return The mobile, if it's in the room.
        Mobile * findMobile(std::string target, int & number, Mobile * exception = nullptr);

        /// @brief Remove from the list of exits the one on the given direction.
        /// @param direction The direction to removed.
        /// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
        bool removeExit(Direction direction);

        /// @brief Search for the desired exit in a direction, provided as an enumerator.
        /// @param direction The direction to search.
        /// @return The desired exit.
        Exit * findExit(Direction direction);

        /// @brief Search for the desired exit in a direction, provided as a string.
        /// @param direction The direction to search.
        /// @return The desired exit.
        Exit * findExit(std::string direction);

        /// @brief Search for the desired exit of this room.
        /// @param destination The destination room.
        /// @return The desired exit.
        Exit * findExit(Room * destination);

        /// @brief Search for a built door inside the room.
        /// @return The contained door if there is one.
        Item * findDoor();

        /// @brief Provide a detailed description of the room.
        /// @param exception The one who are looking.
        /// @return A detailed description of the room.
        std::string getLook(Character * exception);

        /// @brief Send a message to all the player in the room, can specify exceptions.
        /// @param message   The message to send.
        /// @param source    The source of the message.
        /// @param exception Another exception to the message.
        void sendToAll(std::string message, Character * source = nullptr, Character * exception = nullptr);

        /// @brief Connect the current room with all the nearby rooms.
        void connectExits();

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);

        /// @brief Define operator lesser than.
        /// @param right The comparison room.
        /// @return <b>True</b> if the room vnum is lesser than the other,<br>
        ///         <b>False</b> otherwise.
        bool operator<(const Room & right) const
        {
            return vnum < right.vnum;
        }

        /// @brief Define operator eual.
        /// @param right The comparison room.
        /// @return <b>True</b> if this and the other room have the same vnum,<br>
        ///         <b>False</b> otherwise.
        bool operator==(const Room & right) const
        {
            return vnum == right.vnum;
        }
};

/// @brief Create a room in the desired position.
/// @param coord       The coordinates where create the room.
/// @param source_room During mining, the emthod uses source_room to connect rooms.
/// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
bool CreateRoom(Coordinates<unsigned int> coord, Room * source_room = NULL);

/// @brief Connect the room with the near rooms.
/// @param room The room to be connected.
void ConnectRoom(Room * room);

/// Room list handler.
typedef std::vector<Room *> RoomList;

/// RoomList iterator.
typedef RoomList::iterator RoomListIterator;

/// Room map handler.
typedef std::map<int, Room *> RoomMap;

/// RoomMap iterator.
typedef RoomMap::iterator RoomMapIterator;

/// Map of travelling points.
typedef std::map<Room *, Room *> TravelPointMap;

#endif
