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

#pragma once

#include <map>

#include "character.hpp"
#include "defines.hpp"
#include "exit.hpp"
#include "mobile.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "item/item.hpp"
#include "lua/lua_script.hpp"
#include "utilities/coordinates.hpp"

#include "item/itemContainer.hpp"

class Item;
class Area;
class Continent;

/// Used to determine the flag of the room.
typedef enum class RoomFlags
{
    /// A player can rent and disconnect.
    Rent = 1,
    /// Everyone here can't be harmful.
    Peaceful = 2,
    /// From here a player can travel to another location.
    TravelPoint = 4,
    /// It is a spawn room for players.
    SpawnPoint = 8
} RoomFlag;

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
        Coordinates coord;
        /// The terrain where the room is situated.
        std::string terrain;
        /// The name of the room.
        std::string name;
        /// A long description of the room.
        std::string description;
        /// List of exits.
        ExitVector exits;
        /// List of items in the room.
        ItemContainer items;
        /// List of characters in the room.
        std::vector<Character *> characters;
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
        void addItem(Item * & item);

        /// @brief Add the passed item to room's buildings list.
        /// @param item The item to add to the list.
        void addBuilding(Item * item);

        /// @brief Add the passed character to room's character list.
        /// @param character The character to add to the list.
        void addCharacter(Character * character);

        /// @brief Remove the passed item from room's item list.
        /// @param item The item to remove from the list.
        /// @return <b>True</b> if the operation goes well,<br>
        ///         <b>False</b> otherwise.
        bool removeItem(Item * item);

        /// @brief Remove the passed item from room's building list.
        /// @param item The building to remove from the list.
        /// @return <b>True</b> if the operation goes well,<br>
        ///         <b>False</b> otherwise.
        bool removeBuilding(Item * item);

        /// @brief Remove the passed character from room's character list.
        /// @param character The character to remove from the list.
        void removeCharacter(Character * character);

        /// @brief Select from the room all the players and return them.
        /// @param exception The exception from the list.
        /// @return The list of all the players in the room.
        std::set<Player *> getAllPlayer(Character * exception);

        /// @brief Select from the room all the mobiles and return them.
        /// @param exception The exception from the list.
        /// @return The list of all the mobiles in the room.
        std::vector<Mobile *> getAllMobile(Character * exception);

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
        std::vector<Item *> findBuildings(ModelType type);

        /// @brief Search for the character in the room.
        /// @param target    The character to search.
        /// @param number    Number of the player we are looking for.
        /// @param exceptions The list of exceptions.
        /// @return The character, if it's in the room.
        Character * findCharacter(
            std::string target,
            int & number,
            const std::vector<Character *> & exceptions);

        /// @brief Search for the player in the room.
        /// @param target    The player to search.
        /// @param number    Number of the player we are looking for.
        /// @param exceptions The list of exceptions.
        /// @return The player, if it's in the room.
        Player * findPlayer(
            std::string target,
            int & number,
            const std::vector<Character *> & exceptions);

        /// @brief Search for the mobile in the room.
        /// @param target    The mobile to search.
        /// @param number    Number of the mobile we are looking for.
        /// @param exceptions The list of exceptions.
        /// @return The mobile, if it's in the room.
        Mobile * findMobile(
            std::string target,
            int & number,
            const std::vector<Character *> & exceptions);

        /// @brief Add the provided exit to the room list of exits.
        /// @param exit The exit to add to the list.
        /// @return <b>True</b> if there is NO other exits in the same direction,<br>
        ///         <b>False</b> otherwise.
        bool addExit(std::shared_ptr<Exit> exit);

        /// @brief Remove from the list of exits the one on the given direction.
        /// @param direction The direction to removed.
        /// @return <b>True</b> if the execution goes well,<br>
        ///         <b>False</b> otherwise.
        bool removeExit(const Direction & direction);

        /// @brief Search for the desired exit in a direction, provided as an enumerator.
        /// @param direction The direction to search.
        /// @return The desired exit.
        std::shared_ptr<Exit> findExit(Direction direction);

        /// @brief Search for the desired exit in a direction, provided as a string.
        /// @param direction The direction to search.
        /// @return The desired exit.
        std::shared_ptr<Exit> findExit(const std::string & direction);

        /// @brief Search for the desired exit of this room.
        /// @param destination The destination room.
        /// @return The desired exit.
        std::shared_ptr<Exit> findExit(Room * destination);

        /// @brief Search for a built door inside the room.
        /// @return The contained door if there is one.
        Item * findDoor();

        /// @brief Provides the list of directions where an exit is present.
        /// @return Vector of directions.
        std::vector<Direction> getAvailableDirections();

        /// @brief Provide a detailed description of the room.
        /// @param exception The one who are looking.
        /// @return A detailed description of the room.
        std::string getLook(Character * exception);

        /// @brief Send a message to all the player in the room, can specify exceptions.
        /// @param message    The message to send.
        /// @param exceptions The list of exceptions.
        void sendToAll(const std::string & message, const std::vector<Character *> & exceptions);

        /// @brief Print to consol and to logging file the gievn string.
        /// @param message    The message to send.
        /// @param exceptions The list of exceptions.
        /// @param first The first unpacked argument.
        /// @param args  Packed arguments.
        template<typename ... Args>
        void sendToAll(
            const std::string & message,
            const std::vector<Character *> & exceptions,
            const std::string & first,
            const Args & ... args)
        {
            std::string::size_type pos = message.find("%s");
            if (pos == std::string::npos)
            {
                this->sendToAll(message, exceptions);
            }
            else
            {
                std::string working(message);
                working.replace(pos, 2, first);
                this->sendToAll(working, exceptions, args ...);
            }
        }

        /// @brief Returns the list of available exits from the current room
        ///         using the vector structure made for lua environment.
        /// @return The vector of exits.
        VectorHelper<Exit *> luaGetExits();

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);

        /// @brief Define operator lesser than.
        /// @param right The comparison room.
        /// @return <b>True</b> if the room vnum is lesser than the other,<br>
        ///         <b>False</b> otherwise.
        bool operator<(const Room & right) const;

        /// @brief Define operator eual.
        /// @param right The comparison room.
        /// @return <b>True</b> if this and the other room have the same vnum,<br>
        ///         <b>False</b> otherwise.
        bool operator==(const Room & right) const;
};

/// @brief Create a room in the desired position.
/// @param coord       The coordinates where create the room.
/// @param source_room During mining, the emthod uses source_room to connect rooms.
/// @return <b>True</b> if the execution goes well,<br><b>False</b> otherwise.
bool CreateRoom(Coordinates coord, Room * source_room = NULL);

/// @brief Connect the room with the near rooms.
/// @param room The room to be connected.
/// @return <b>True</b> if the execution goes well,<br>
///         <b>False</b> otherwise.
bool ConnectRoom(Room * room);

/// @addtogroup FlagsToList
/// @{

/// Return a list of string containg the Room flags contained inside the value.
std::string GetRoomFlagString(unsigned int flags);

/// @}
