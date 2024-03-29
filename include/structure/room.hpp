/// @file   room.hpp
/// @brief  Define the room class.
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

#pragma once

#include <map>

#include "structure/exit.hpp"
#include "structure/coordinates.hpp"
#include "utilities/utils.hpp"
#include "item/item.hpp"
#include "character/mobile.hpp"
#include "character/characterVector.hpp"
#include "item/itemVector.hpp"
#include "structure/terrain/terrain.hpp"

class Item;

class Area;

/// Used to determine the flag of the room.
using RoomFlag = enum class RoomFlags
{
    Rent = 1,           ///< A player can rent and disconnect.
    Peaceful = 2,       ///< Everyone here can't be harmful.
    TravelPoint = 4,    ///< From here a player can travel to another location.
    SpawnTree = 8,      ///< The room spawns trees.
    Air = 16,           ///< The room has no structure is just air.
};

/// @brief Holds details about room.
class Room :
    public UpdateInterface
{
public:
    /// The current room vnum.
    int vnum;
    /// The area where is located the room.
    Area * area;
    /// The current room coordinates.
    Coordinates coord;
    /// The type of terrain of the room.
    std::shared_ptr<Terrain> terrain;
    /// The name of the room.
    std::string name;
    /// A long description of the room.
    std::string description;
    /// List of exits.
    std::vector<std::shared_ptr<Exit> > exits;
    /// List of items in the room.
    ItemVector items;
    /// List of characters in the room.
    CharacterVector characters;
    /// Integer that describe the flags of the room.
    unsigned int flags;
    /// The liquid which fills the room.
    std::pair<Liquid *, unsigned int> liquidContent;

    /// @brief Constructor.
    Room();

    /// @brief Destructor.
    virtual ~Room();

    /// @brief Function used to check the correctness of the room.
    /// @param complete If set to true, the function check if the room has
    ///                  been placed inside an area.
    /// @return <b>True</b> if the room values are correct,<br>
    ///         <b>False</b> otherwise.
    bool check(bool complete = false);

    /// @brief In addition to adding the the passed item to room's items list.
    /// @param item     The item to add to the list.
    /// @param updateDB If <b>True</b> all the changes are also updated on DB.
    void addItem(Item *& item, bool updateDB = true);

    /// @brief Add the passed item to room's buildings list.
    /// @param item     The building to add to the list.
    /// @param updateDB If <b>True</b> all the changes are also updated on DB.
    void addBuilding(Item * item, bool updateDB = true);

    /// @brief Add the passed character to room's character list.
    /// @param character The character to add to the list.
    void addCharacter(Character * character);

    /// @brief Remove the passed item from room's item list.
    /// @param item     The item to remove from the list.
    /// @param updateDB If <b>True</b> all the changes are also updated on DB.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    bool removeItem(Item * item, bool updateDB = true);

    /// @brief Remove the passed item from room's building list.
    /// @param item     The building to remove from the list.
    /// @param updateDB If <b>True</b> all the changes are also updated on DB.
    /// @return <b>True</b> if the operation goes well,<br>
    ///         <b>False</b> otherwise.
    bool removeBuilding(Item * item, bool updateDB = true);

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
    /// @return <b>True</b> if the execution goes well,<br>
    ///         <b>False</b> otherwise.
    bool updateOnDB();

    /// @brief Remove the room from the database.
    /// @return <b>True</b> if the execution goes well,<br>
    ///         <b>False</b> otherwise.
    bool removeOnDB();

    /// @brief Search for the item in the room.
    /// @param key    The item to search.
    /// @param number Position of the item we want to look for.
    /// @return The item, if it's in the room.
    inline Item * findItem(std::string const & key, int & number)
    {
        return ItemUtils::FindItemIn(items, key, number);
    }

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
    /// @param number    Number of the player we are looking for.
    /// @param exceptions The list of exceptions.
    /// @return The character, if it's in the room.
    inline Character * findCharacter(
        std::string const & target,
        int & number,
        std::vector<Character *> const & exceptions = std::vector<Character *>()) const
    {
        return characters.findCharacter(target, number, exceptions);
    }

    /// @brief Search for the player in the room.
    /// @param target    The player to search.
    /// @param number    Number of the player we are looking for.
    /// @param exceptions The list of exceptions.
    /// @return The player, if it's in the room.
    Player * findPlayer(std::string target,
                        int & number,
                        const std::vector<Character *> & exceptions =
                        std::vector<Character *>()) const;

    /// @brief Search for the mobile in the room.
    /// @param target    The mobile to search.
    /// @param number    Number of the mobile we are looking for.
    /// @param exceptions The list of exceptions.
    /// @return The mobile, if it's in the room.
    Mobile * findMobile(std::string target,
                        int & number,
                        const std::vector<Character *> & exceptions =
                        std::vector<Character *>()) const;

    /// @brief Add the provided exit to the room list of exits.
    /// @param exit The exit to add to the list.
    /// @return <b>True</b> if there is NO other exits
    ///                      in the same direction,<br>
    ///         <b>False</b> otherwise.
    bool addExit(const std::shared_ptr<Exit> & exit);

    /// @brief Remove from the list of exits the one on the given direction.
    /// @param direction The direction to removed.
    /// @return <b>True</b> if the execution goes well,<br>
    ///         <b>False</b> otherwise.
    bool removeExit(const Direction & direction);

    /// @brief Search for the desired exit in a direction,
    ///         provided as an enumerator.
    /// @param direction The direction to search.
    /// @return The desired exit.
    std::shared_ptr<Exit> findExit(Direction direction);

    /// @brief Check if the room is lit or not.
    bool isLit();

    /// @brief Provide a detailed description of the room.
    /// @param actor The one who is looking.
    /// @return A detailed description of the room.
    std::string getLook(Character * actor);

    /// @brief Send a message to all the player in the room,
    ///         can specify exceptions.
    /// @param message    The message to send.
    /// @param exceptions The list of exceptions.
    void sendToAll(const std::string & message,
                   const std::vector<Character *> & exceptions);

    /// @brief Sends a message to all the characters inside the room.
    /// @param message    The message to send.
    /// @param exceptions The list of exceptions.
    /// @param args  Packed arguments.
    template<typename ... Args>
    void sendToAll(const std::string & message,
                   const std::vector<Character *> & exceptions,
                   const Args & ... args)
    {
        sendToAll(StringBuilder::build(message, args...), exceptions);
    }

    /// @brief Send a message to all the characters inside the room which
    ///         pass the checking function.
    /// @param message        The message to send.
    /// @param checkException The checking function.
    void funcSendToAll(const std::string & message,
                       std::function<bool(Character * character)> checkException);

    /// @brief Send a message to all the characters inside the room
    ///         which pass the checking function.
    /// @param message        The message to send.
    /// @param checkException The checking function.
    /// @param args           The rest of the arguments.
    template<typename ... Args>
    void funcSendToAll(const std::string & message,
                       std::function<bool(Character * character)> checkException,
                       const Args & ... args)
    {
        funcSendToAll(StringBuilder::build(message, args...), checkException);
    }

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

protected:
    void updateTicImpl() override;

    void updateHourImpl() override;
};

/// @brief Create a room in the desired position.
/// @param coord       The coordinates where create the room.
/// @param source_room Used to connect rooms.
/// @return <b>True</b> if the execution goes well,<br>
///         <b>False</b> otherwise.
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
