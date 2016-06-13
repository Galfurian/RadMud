/// @file   room.cpp
/// @brief  Implements room methods.
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
#include "room.hpp"

#include <algorithm>
#include <cmath>

// Other Include.
#include "mud.hpp"
#include "logger.hpp"
#include "constants.hpp"
#include "formatter.hpp"
#include "generator.hpp"
#include "luabridge/LuaBridge.h"

using namespace std;

Room::Room() :
    vnum(),
    area(),
    continent(),
    coord(),
    terrain(),
    name(),
    description(),
    exits(),
    items(),
    characters(),
    flags()
{
    // Nothing to do.
}

Room::~Room()
{
    Logger::log(LogLevel::Debug, "Deleted: Room (" + this->name + ").");
    // Remove all the exits.
    for (auto iterator : exits)
    {
        delete (iterator);
    }
    // Remove the room from the area.
    if (area != nullptr)
    {
        area->remRoom(this);
    }
}

bool Room::check(bool complete)
{
    assert(vnum > 0);
    if (complete)
    {
        assert((area != nullptr) || (continent != nullptr));
        assert(!((area != nullptr) && (continent != nullptr)));
    }
    assert(coord.x <= 100);
    assert(coord.y <= 100);
    assert(coord.z <= 100);
    assert(!terrain.empty());
    assert(!name.empty());
    assert(!description.empty());
    assert(!terrain.empty());
    return true;
}

void Room::addItem(Item * item)
{
    items.push_back(item);
    item->room = this;
    Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' added to '" + this->name + "';");
}

void Room::addBuilding(Item * item)
{
    this->addItem(item);
    SetFlag(item->flags, ItemFlag::Built);
}

void Room::addCharacter(Character * character)
{
    characters.push_back(character);
    character->room = this;
}

void Room::removeItem(Item * item)
{
    if (FindErase(items, item))
    {
        Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' removed from '" + this->name + "';");
        item->room = nullptr;
    }
    else
    {
        Logger::log(LogLevel::Error, "Error during item removal from room.");
    }
}

void Room::removeBuilding(Item * item)
{
    if (FindErase(items, item))
    {
        Logger::log(LogLevel::Debug, "Building '" + item->getName() + "' removed from '" + this->name + "';");
        item->room = nullptr;
        ClearFlag(item->flags, ItemFlag::Built);
    }
    else
    {
        Logger::log(LogLevel::Error, "Error during item removal from room.");
    }
}

void Room::removeCharacter(Character * character)
{
    if (FindErase(characters, character))
    {
        character->room = nullptr;
    }
    else
    {
        Logger::log(LogLevel::Error, "Error during character removal from room.");
    }
}

PlayerList Room::getAllPlayer(Character * exception)
{
    PlayerList plvec;
    for (auto iterator : characters)
    {
        if (exception)
        {
            if (iterator == exception)
            {
                continue;
            }
        }
        if (iterator->isMobile())
        {
            continue;
        }
        if (!iterator->toPlayer()->isPlaying())
        {
            continue;
        }
        plvec.insert(iterator->toPlayer());
    }
    return plvec;
}

MobileList Room::getAllMobile(Character * exception)
{
    MobileList movec;
    for (auto iterator : characters)
    {
        if (exception)
        {
            if (iterator == exception)
            {
                continue;
            }
        }
        if (!iterator->isMobile())
        {
            continue;
        }
        movec.push_back(iterator->toMobile());
    }
    return movec;
}

bool Room::updateOnDB()
{
    QueryList value;
    value.push_back(std::make_pair("x", ToString(coord.x)));
    value.push_back(std::make_pair("y", ToString(coord.y)));
    value.push_back(std::make_pair("z", ToString(coord.z)));
    value.push_back(std::make_pair("terrain", terrain));
    value.push_back(std::make_pair("name", name));
    value.push_back(std::make_pair("description", description));
    value.push_back(std::make_pair("flag", ToString(flags)));
    QueryList where;
    where.push_back(std::make_pair("vnum", ToString(vnum)));
    if (!SQLiteDbms::instance().updateInto("Room", value, where))
    {
        Logger::log(LogLevel::Error, "Can't save the room!");
        return false;
    }
    return true;
}

bool Room::removeOnDB()
{
    if (items.size() > 0)
    {
        Logger::log(LogLevel::Error, "[Room::RemoveOnDB] There are still items in the room!");
        return false;
    }
    if (characters.size() > 0)
    {
        Logger::log(LogLevel::Error, "[Room::RemoveOnDB] There are still characters in the room!");
        return false;
    }

    //----------Remove from Room-----------
    SQLiteDbms::instance().deleteFrom("Room",
    { std::make_pair("vnum", ToString(vnum)) });
    return true;
}

Item * Room::findItem(string search_parameter, int & number)
{
    for (auto iterator : items)
    {
        if (iterator->hasKey(ToLower(search_parameter)))
        {
            if (number == 1)
            {
                return iterator;
            }
            number -= 1;
        }
    }
    return nullptr;
}

Item * Room::findBuilding(std::string target, int & number)
{
    for (auto iterator : items)
    {
        if (iterator->hasKey(ToLower(target)) && HasFlag(iterator->flags, ItemFlag::Built))
        {
            if (number == 1)
            {
                return iterator;
            }
            number -= 1;
        }
    }
    return nullptr;
}
Item * Room::findBuilding(int buildingVnum)
{
    for (auto iterator : items)
    {
        if (iterator->vnum == buildingVnum)
        {
            return iterator;
        }
    }
    return nullptr;
}

ItemVector Room::findBuildings(ModelType type)
{
    ItemVector buildingsList;
    for (auto iterator : items)
    {
        if ((iterator->model->type == type) && HasFlag(iterator->flags, ItemFlag::Built))
        {
            buildingsList.push_back(iterator);
        }
    }
    return buildingsList;
}

Character * Room::findCharacter(string target, int & number, Character * exception)
{
    Player * player;
    Mobile * mobile;

    for (auto iterator : characters)
    {
        // Exclude the exception.
        if (iterator == exception)
        {
            continue;
        }

        // Check if the character is a mobile or a player.
        if (iterator->isMobile())
        {
            mobile = iterator->toMobile();
            if (mobile->hasKey(ToLower(target)))
            {
                if (number > 1)
                {
                    number -= 1;
                }
                else
                {
                    return mobile;
                }
            }
        }
        else
        {
            player = iterator->toPlayer();
            if (!player->isPlaying())
            {
                continue;
            }
            if (BeginWith(player->getName(), ToLower(target)))
            {
                if (number > 1)
                {
                    number -= 1;
                }
                else
                {
                    return player;
                }
            }
        }
    }
    return nullptr;
}

Player * Room::findPlayer(string target, int & number, Player * exception)
{
    Player * player;

    for (auto iterator : characters)
    {
        // Exclude the exception.
        if (iterator == exception)
        {
            continue;
        }
        // Check if the character is a mobile.
        if (iterator->isMobile())
        {
            continue;
        }

        player = iterator->toPlayer();
        if (!player->isPlaying())
        {
            continue;
        }
        if (BeginWith(player->getName(), ToLower(target)))
        {
            if (number == 1)
            {
                return player;
            }
            number -= 1;
        }
    }
    return nullptr;
}

Mobile * Room::findMobile(string target, int & number, Mobile * exception)
{
    Mobile * mobile;

    for (auto iterator : characters)
    {
        // Exclude the exception.
        if (iterator == exception)
        {
            continue;
        }
        // Check if the character is a player.
        if (!iterator->isMobile())
        {
            continue;
        }

        mobile = iterator->toMobile();
        if (mobile->hasKey(ToLower(target)))
        {
            if (number == 1)
            {
                return mobile;
            }
            number -= 1;
        }
    }
    return nullptr;
}

Exit * Room::findExit(Direction direction)
{
    for (auto iterator : exits)
    {
        if (iterator->direction == direction)
        {
            return iterator;
        }
    }
    return nullptr;
}

Exit * Room::findExit(std::string direction)
{
    Direction dir = GetDirection(direction);
    for (auto iterator : exits)
    {
        if (iterator->direction == dir)
        {
            return iterator;
        }
    }
    return nullptr;
}

Exit * Room::findExit(Room * destination)
{
    for (auto iterator : exits)
    {
        if (iterator->destination == destination)
        {
            return iterator;
        }
    }
    return nullptr;
}
Item * Room::findDoor()
{
    for (auto iterator : items)
    {
        if ((iterator->model->type == ModelType::Mechanism) && HasFlag(iterator->flags, ItemFlag::Built))
        {
            MechanismFunc mechanismFunc = iterator->model->getMechanismFunc();
            if (mechanismFunc.type == MechanismType::Door)
            {
                return iterator;
            }
        }
    }
    return nullptr;
}
std::vector<Direction> Room::getAvailableDirections()
{
    std::vector<Direction> directions;
    for (auto iterator : exits)
    {
        directions.push_back(iterator->direction);
    }
    return directions;
}
bool Room::addExit(Exit * exit)
{
    if (this->findExit(exit->direction))
    {
        return false;
    }
    this->exits.push_back(exit);
    return true;
}
bool Room::removeExit(Direction direction)
{
    for (auto iterator : exits)
    {
        if (iterator->direction == direction)
        {
            FindErase(exits, iterator);
            return true;
        }
    }
    return false;
}

string Room::getLook(Character * exception)
{
    string output = "";

    // The player want to look at the entire room.
    output += Formatter::bold() + name + Formatter::reset() + "\n";
    output += description + "\n";

    if (exits.empty())
    {
        output += "There is no exit from here!\n";
    }
    else
    {
        if (area != nullptr)
        {
            if (Formatter::getFormat() == Formatter::TELNET)
            {
                std::vector<std::string> layers = area->drawFov(this, exception->getViewDistance());
                output += Formatter::doClearMap();
                for (auto layer : layers)
                {
                    output += Formatter::doDrawMap();
                    output += layer;
                    output += Formatter::dontDrawMap();
                }
            }
            else
            {
                output += area->drawASCIIFov(this, exception->getViewDistance());
            }
        }
        else if (continent != nullptr)
        {
            std::vector<std::string> layers = continent->drawFov(this, exception->getViewDistance());
            output += Formatter::doClearMap();
            for (auto layer : layers)
            {
                output += Formatter::doDrawMap();
                output += layer;
                output += Formatter::dontDrawMap();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // List all the items placed in the same room.
    for (auto it : GroupItems(items))
    {
        // If the item is invisible, don't show it.
        if (HasFlag(it.first->model->flags, ModelFlag::Invisible))
        {
            continue;
        }

        if (HasFlag(it.first->flags, ItemFlag::Built))
        {
            output += "[B]";
        }

        // If there are more of this item, show the counter.
        if (it.second > 1)
        {
            output += Formatter::cyan() + it.first->getNameCapital() + Formatter::reset() + " are here.["
                + ToString(it.second) + "]\n";
        }
        else
        {
            output += Formatter::cyan() + it.first->getNameCapital() + Formatter::reset() + " is here.\n";
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // List mobile in the same room.
    for (auto iterator : getAllMobile(exception))
    {
        if (!exception->canSee(iterator))
        {
            continue;
        }
        output += Formatter::blue() + iterator->getStaticDesc() + Formatter::reset() + "\n";
    }

    ///////////////////////////////////////////////////////////////////////////
    // List other players in the same room.
    for (auto iterator : getAllPlayer(exception))
    {
        if (!exception->canSee(iterator))
        {
            continue;
        }
        output += Formatter::gray() + iterator->getStaticDesc() + Formatter::reset() + "\n";
    }

    return output;
}

void Room::sendToAll(const std::string & message, const CharacterVector & exceptions)
{
    for (auto iterator : characters)
    {
        if (!exceptions.empty())
        {
            if (std::find(exceptions.begin(), exceptions.end(), iterator) != exceptions.end())
            {
                continue;
            }
        }
        iterator->sendMsg(message + "\n");
    }
}

void Room::connectExits()
{
    for (auto iterator : Mud::instance().mudDirections)
    {
        Coordinates<int> nearCoord = GetCoordinates(iterator.second);
        if (coord < nearCoord)
        {
            continue;
        }
        if (area != nullptr)
        {
            Room * near = area->getRoom(coord + nearCoord);
            if (near != nullptr)
            {
                // Create the two exits.
                Exit * forward = new Exit(this, near, iterator.second, 0);
                Exit * backward = new Exit(near, this, forward->getOppositeDirection(), 0);

                // In case the connection is Up/Down set the presence of stairs.
                if (iterator.second == Direction::Up || iterator.second == Direction::Down)
                {
                    SetFlag(forward->flags, ExitFlag::Stairs);
                    SetFlag(backward->flags, ExitFlag::Stairs);
                }

                // Insert in both the rooms exits the connection.
                exits.push_back(forward);
                near->exits.push_back(backward);
            }
        }
        else if (continent != nullptr)
        {
            Room * near = continent->getRoom(coord + nearCoord);

            if (iterator.second == Direction::Up || iterator.second == Direction::Down)
            {
                continue;
            }
            if (near != nullptr)
            {
                // Create the two exits.
                Exit * forward = new Exit(this, near, iterator.second, 0);
                Exit * backward = new Exit(near, this, forward->getOppositeDirection(), 0);
                // Insert in both the rooms exits the connection.
                this->addExit(forward);
                near->addExit(backward);
            }
        }
    }
}

void Room::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass < Room > ("Room") //
    .addData("vnum", &Room::vnum, false) //
    .addData("name", &Room::name, false) //
    .addData("coord", &Room::coord, false) //
    .addData("terrain", &Room::terrain, false) //
    .endClass();
}

bool Room::operator<(const Room & right) const
{
    return vnum < right.vnum;
}

bool Room::operator==(const Room & right) const
{
    return vnum == right.vnum;
}

bool CreateRoom(Coordinates<int> coord, Room * source_room)
{
    Room * new_room = new Room();

    // Create a new room.
    Logger::log(LogLevel::Info, "[CreateRoom] Setting up the room...");
    new_room->vnum = Mud::instance().getMaxVnumRoom() + 1;
    new_room->area = source_room->area;
    new_room->coord = coord;
    new_room->terrain = source_room->terrain;
    new_room->name = Generator::instance().generateName(source_room->area->type, source_room->area->status);
    new_room->description = Generator::instance().generateDescription(source_room->area->type,
        source_room->area->status, new_room->name);
    new_room->flags = 0;

    // Insert into table Room the newly created room.
    Logger::log(LogLevel::Info, "[CreateRoom] Saving room details on the database...");
    vector<string> arguments;
    arguments.push_back(ToString(new_room->vnum));
    arguments.push_back(ToString(new_room->coord.x));
    arguments.push_back(ToString(new_room->coord.y));
    arguments.push_back(ToString(new_room->coord.z));
    arguments.push_back(new_room->terrain);
    arguments.push_back(new_room->name);
    arguments.push_back(new_room->description);
    arguments.push_back(ToString(new_room->flags));

    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Room", arguments))
    {
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }

    // Insert Room in AreaList.
    vector<string> arguments2;
    arguments2.push_back(ToString(new_room->area->vnum));
    arguments2.push_back(ToString(new_room->vnum));
    if (!SQLiteDbms::instance().insertInto("AreaList", arguments2))
    {
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    // Add the created room to the room_map.
    Logger::log(LogLevel::Info, "[CreateRoom] Adding the room to the global list...");
    if (!Mud::instance().addRoom(new_room))
    {
        Logger::log(LogLevel::Error, "Cannot add the room to the mud.\n");
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    if (!new_room->area->addRoom(new_room))
    {
        Logger::log(LogLevel::Error, "Cannot add the room to the area.\n");
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    // Update rooms connection if there is a source_room.
    if (!ConnectRoom(new_room))
    {
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool ConnectRoom(Room * room)
{
    bool status = true;
    RoomList connectedRooms;
    ExitList generatedExits;

    Logger::log(LogLevel::Info, "[ConnectRoom] Connecting the room to near rooms...");
    for (auto iterator : Mud::instance().mudDirections)
    {
        // Get the coordinate modifier.
        Coordinates<int> coordinates = room->coord + GetCoordinates(iterator.second);
        // Get the room at the given coordinates.
        Room * near = room->area->getRoom(coordinates);
        if (near != nullptr)
        {
            Logger::log(LogLevel::Error, "Found a room:%s\n", near->name);
            // Create the two exits.
            Exit * forward = new Exit(room, near, iterator.second, 0);
            generatedExits.push_back(forward);
            Exit *backward = new Exit(near, room, forward->getOppositeDirection(), 0);
            generatedExits.push_back(backward);

            // In case the connection is Up/Down set the presence of stairs.
            if (iterator.second == Direction::Up || iterator.second == Direction::Down)
            {
                SetFlag(forward->flags, ExitFlag::Stairs);
                SetFlag(backward->flags, ExitFlag::Stairs);
            }

            // Insert in both the rooms exits the connection.
            room->addExit(forward);
            connectedRooms.push_back(room);
            near->addExit(forward);
            connectedRooms.push_back(near);

            // Update the values on Database.
            vector<string> arguments;
            arguments.push_back(ToString(forward->source->vnum));
            arguments.push_back(ToString(forward->destination->vnum));
            arguments.push_back(EnumToString(forward->direction));
            arguments.push_back(ToString(forward->flags));
            if (!SQLiteDbms::instance().insertInto("Exit", arguments))
            {
                status = false;
                break;
            }
            else
            {
                status = false;
                break;
            }

            vector<string> arguments2;
            arguments2.push_back(ToString(backward->source->vnum));
            arguments2.push_back(ToString(backward->destination->vnum));
            arguments2.push_back(EnumToString(backward->direction));
            arguments2.push_back(ToString(backward->flags));
            if (!SQLiteDbms::instance().insertInto("Exit", arguments2))
            {
                status = false;
                break;
            }
        }
    }
    if (!status)
    {
        Logger::log(LogLevel::Error, "Disconnecting all the rooms.");
        for (auto iterator : connectedRooms)
        {
            for (auto iterator2 : generatedExits)
            {
                if (iterator2->source == iterator)
                {
                    if (!iterator->removeExit(iterator2->direction))
                    {
                        Logger::log(LogLevel::Fatal, "I was not able to remove an unwanted exit.");
                    }
                }
            }
        }
    }
    return status;
}
