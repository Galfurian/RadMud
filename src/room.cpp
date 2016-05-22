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

#include "constants.hpp"
#include "generator.hpp"
// Other Include.
#include "luabridge/LuaBridge.h"
#include "mud.hpp"
#include "telnet.hpp"

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
    assert((coord.x >= -100) && (coord.x <= 100));
    assert((coord.y >= -100) && (coord.y <= 100));
    assert((coord.z >= -100) && (coord.z <= 100));
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
    if (remove_erase(items, item) != items.end())
    {
        item->room = nullptr;
    }
    else
    {
        LogError("Error during item removal from room.");
    }
}

void Room::removeBuilding(Item * item)
{
    if (remove_erase(items, item) != items.end())
    {
        item->room = nullptr;
        ClearFlag(item->flags, ItemFlag::Built);
    }
    else
    {
        LogError("Error during item removal from room.");
    }
}

void Room::removeCharacter(Character * character)
{
    if (remove_erase(characters, character) != characters.end())
    {
        character->room = nullptr;
    }
    else
    {
        LogError("Error during character removal from room.");
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
    if (!Mud::getInstance().getDbms().updateInto("Room", value, where))
    {
        LogError("Can't save the room!");
        return false;
    }
    return true;
}

bool Room::removeOnDB()
{
    if (items.size() > 0)
    {
        LogError("[Room::RemoveOnDB] There are still items in the room!");
        return false;
    }
    if (characters.size() > 0)
    {
        LogError("[Room::RemoveOnDB] There are still characters in the room!");
        return false;
    }

    //----------Remove from Room-----------
    Mud::getInstance().getDbms().deleteFrom("Room",
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
Item * Room::findBuilding(int vnum)
{
    for (auto iterator : items)
    {
        if (iterator->vnum == vnum)
        {
            return iterator;
        }
    }
    return nullptr;
}

ItemList Room::findBuildings(ModelType type)
{
    ItemList buildingsList;
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
bool Room::removeExit(Direction direction)
{
    for (auto iterator : exits)
    {
        if (iterator->direction == direction)
        {
            remove_erase(exits, iterator);
            delete (iterator);
            return true;
        }
    }
    return false;
}

string Room::getLook(Character * exception)
{
    string output = "";

    // The player want to look at the entire room.
    output += Telnet::bold() + name + Telnet::reset() + "\n";
    output += description + "\n";

    if (exits.empty())
    {
        output += "There is no exit from here!\n";
    }
    else
    {
        if (area != nullptr)
        {
            std::vector<std::string> layers = area->drawFov(this, exception->getViewDistance());
            output += Telnet::doClearMap();
            for (auto layer : layers)
            {
                output += Telnet::doDrawMap();
                output += layer;
                output += Telnet::dontDrawMap();
            }
        }
        else if (continent != nullptr)
        {
            std::vector<std::string> layers = continent->drawFov(this, exception->getViewDistance());
            output += Telnet::doClearMap();
            for (auto layer : layers)
            {
                output += Telnet::doDrawMap();
                output += layer;
                output += Telnet::dontDrawMap();
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
            output += Telnet::cyan() + it.first->getNameCapital() + Telnet::reset() + " are here.["
                + ToString(it.second) + "]\n";
        }
        else
        {
            output += Telnet::cyan() + it.first->getNameCapital() + Telnet::reset() + " is here.\n";
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
        output += Telnet::blue() + iterator->getStaticDesc() + Telnet::reset() + "\n";
    }

    ///////////////////////////////////////////////////////////////////////////
    // List other players in the same room.
    for (auto iterator : getAllPlayer(exception))
    {
        if (!exception->canSee(iterator))
        {
            continue;
        }
        output += Telnet::gray() + iterator->getStaticDesc() + Telnet::reset() + "\n";
    }

    return output;
}

void Room::sendToAll(string message, Character * source, Character * exception)
{
    for (auto iterator : getAllPlayer(source))
    {
        if (source)
        {
            if (iterator == source)
            {
                continue;
            }
        }
        if (exception)
        {
            if (iterator == exception)
            {
                continue;
            }
        }
        iterator->sendMsg(message + "\n");
    }
}

void Room::connectExits()
{
    for (auto iterator : Mud::getInstance().mudDirections)
    {
        Coordinates nearCoord = GetCoordinates(iterator.second);
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
                exits.push_back(forward);
                near->exits.push_back(backward);
            }
        }
    }
}

void Room::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Room>("Room") //
    .addData("vnum", &Room::vnum, false) //
    .addData("name", &Room::name, false) //
    .addData("coord", &Room::coord, false) //
    .addData("terrain", &Room::terrain, false) //
    .endClass();
}

bool CreateRoom(Coordinates coord, Room * source_room)
{
    Room * new_room = new Room();
    pair<string, string> details;
    vector<string> arguments;
    string name, description;
    Generator generator;

    // Generate room description and name.
    LogMessage(kMDev, "[CreateRoom] Generating room details...");
    details = generator.generateRoom("cavern", "normal");
    name = details.first;
    description = details.second;

    // Create a new room.
    LogMessage(kMDev, "[CreateRoom] Setting up the room variables...");
    new_room->vnum = Mud::getInstance().getMaxVnumRoom() + 1;
    new_room->area = source_room->area;
    new_room->coord = coord;
    new_room->terrain = source_room->terrain;
    new_room->name = name;
    new_room->description = description;
    new_room->flags = 0;

    // Insert into table Room the newly created room.
    LogMessage(kMDev, "[CreateRoom] Saving room details on the database...");
    arguments.push_back(ToString(new_room->vnum));
    arguments.push_back(ToString(new_room->coord.x));
    arguments.push_back(ToString(new_room->coord.y));
    arguments.push_back(ToString(new_room->coord.z));
    arguments.push_back(new_room->terrain);
    arguments.push_back(new_room->name);
    arguments.push_back(new_room->description);
    arguments.push_back(ToString(new_room->flags));
    if (!Mud::getInstance().getDbms().insertInto("Room", arguments))
    {
        return false;
    }
    arguments.clear();

    // Insert Room in AreaList.
    arguments.push_back(ToString(new_room->area->vnum));
    arguments.push_back(ToString(new_room->vnum));
    if (!Mud::getInstance().getDbms().insertInto("AreaList", arguments))
    {
        return false;
    }
    arguments.clear();

    // Update rooms connection if there is a source_room.
    ConnectRoom(new_room);

    // Add the created room to the room_map.
    LogMessage(kMDev, "[CreateRoom] Adding the room to the global list...");
    Mud::getInstance().addRoom(new_room);
    new_room->area->addRoom(new_room);

    return true;
}

void ConnectRoom(Room * room)
{
    Coordinates poss_coord, coord = room->coord;
    vector<string> arguments;

    LogMessage(kMDev, "[ConnectRoom] Connecting the room to near rooms...");
    for (auto iterator : Mud::getInstance().mudDirections)
    {
        // Get the coordinates.
        poss_coord = GetCoordinates(iterator.second);
        // Get the room at the given coordinates.
        Room * near = room->area->getRoom(coord + poss_coord);
        if (near != nullptr)
        {
            // Create the two exits.
            Exit * forward = new Exit(room, near, iterator.second, 0);
            Exit * backward = new Exit(near, room, forward->getOppositeDirection(), 0);

            // In case the connection is Up/Down set the presence of stairs.
            if (iterator.second == Direction::Up || iterator.second == Direction::Down)
            {
                SetFlag(forward->flags, ExitFlag::Stairs);
                SetFlag(backward->flags, ExitFlag::Stairs);
            }

            // Insert in both the rooms exits the connection.
            room->exits.push_back(forward);
            near->exits.push_back(backward);

            // Update the values on Database.
            arguments.push_back(ToString(forward->source->vnum));
            arguments.push_back(ToString(forward->destination->vnum));
            arguments.push_back(GetDirectionName(forward->direction));
            arguments.push_back(ToString(forward->flags));
            Mud::getInstance().getDbms().insertInto("Exit", arguments);
            arguments.clear();

            arguments.push_back(ToString(backward->source->vnum));
            arguments.push_back(ToString(backward->destination->vnum));
            arguments.push_back(GetDirectionName(backward->direction));
            arguments.push_back(ToString(backward->flags));
            Mud::getInstance().getDbms().insertInto("Exit", arguments);
            arguments.clear();
        }
    }
}