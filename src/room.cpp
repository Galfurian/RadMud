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
#include <utility> // std::move

// Other Include.
#include "mud.hpp"
#include "constants.hpp"
#include "formatter.hpp"
#include "generator.hpp"
#include "utilities/logger.hpp"
#include "luabridge/LuaBridge.h"
#include "model/mechanismModel.hpp"

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
    for (auto it : exits)
    {
        std::shared_ptr<Exit> oppositeExit = it->getOppositeExit();
        if (oppositeExit != nullptr)
        {
            oppositeExit->unlink();
        }
    }
    // Remove the room from the area.
    if (area != nullptr)
    {
        area->remRoom(this);
    }
    Logger::log(LogLevel::Debug, "Deleted room\t\t[%s]\t\t(%s)", ToString(this->vnum), this->name);
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
    item = items.addItem(item);
    item->room = this;
    Logger::log(LogLevel::Debug, "Item '" + item->getName() + "' added to '" + this->name + "';");
}

void Room::addBuilding(Item * item)
{
    SetFlag(item->flags, ItemFlag::Built);
    for (auto iterator : this->items)
    {
        if (iterator->vnum == item->vnum)
        {
            return;
        }
    }
    this->addItem(item);
}

void Room::addCharacter(Character * character)
{
    characters.push_back(character);
    character->room = this;
}

bool Room::removeItem(Item * item)
{
    bool removed = false;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if ((*it)->vnum == item->vnum)
        {
            Logger::log(
                LogLevel::Debug,
                "Item '" + item->getName() + "' removed from '" + this->name + "';");
            item->room = nullptr;
            items.erase(it);
            removed = true;
            break;
        }
    }
    return removed;
}

bool Room::removeBuilding(Item * item)
{
    if (this->removeItem(item))
    {
        ClearFlag(item->flags, ItemFlag::Built);
        return true;
    }
    return false;
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

std::set<Player *> Room::getAllPlayer(Character * exception)
{
    std::set<Player *> plvec;
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

std::vector<Mobile *> Room::getAllMobile(Character * exception)
{
    std::vector<Mobile *> movec;
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
    /*
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
     */
    return false;
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
    SQLiteDbms::instance().deleteFrom("Room", { std::make_pair("vnum", ToString(vnum)) });
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

std::vector<Item *> Room::findBuildings(ModelType type)
{
    std::vector<Item *> buildingsList;
    for (auto iterator : items)
    {
        if ((iterator->model->getType() == type) && HasFlag(iterator->flags, ItemFlag::Built))
        {
            buildingsList.push_back(iterator);
        }
    }
    return buildingsList;
}

Character * Room::findCharacter(string target, int & number, const std::vector<Character *> & exceptions)
{
    for (auto iterator : characters)
    {
        // Check exceptions.
        if (!exceptions.empty())
        {
            if (std::find(exceptions.begin(), exceptions.end(), iterator) != exceptions.end())
            {
                continue;
            }
        }
        // Check if the character is a mobile or a player.
        if (iterator->isMobile())
        {
            if (iterator->toMobile()->hasKey(ToLower(target)))
            {
                if (number > 1)
                {
                    number -= 1;
                }
                else
                {
                    return iterator->toMobile();
                }
            }
        }
        else
        {
            if (iterator->toPlayer()->isPlaying())
            {
                if (BeginWith(iterator->toPlayer()->getName(), ToLower(target)))
                {
                    if (number > 1)
                    {
                        number -= 1;
                    }
                    else
                    {
                        return iterator->toPlayer();
                    }
                }
            }
        }
    }
    return nullptr;
}

Player * Room::findPlayer(string target, int & number, const std::vector<Character *> & exceptions)
{
    for (auto iterator : characters)
    {
        // Check if the character is a mobile.
        if (iterator->isMobile())
        {
            continue;
        }
        // Check exceptions.
        if (!exceptions.empty())
        {
            if (std::find(exceptions.begin(), exceptions.end(), iterator) != exceptions.end())
            {
                continue;
            }
        }
        // Check if it is the desired target.
        if (iterator->toPlayer()->isPlaying())
        {
            if (BeginWith(iterator->toPlayer()->getName(), ToLower(target)))
            {
                if (number == 1)
                {
                    return iterator->toPlayer();
                }
                number -= 1;
            }
        }
    }
    return nullptr;
}

Mobile * Room::findMobile(string target, int & number, const std::vector<Character *> & exceptions)
{
    for (auto iterator : characters)
    {
        // Check if the character is a player.
        if (!iterator->isMobile())
        {
            continue;
        }
        // Check exceptions.
        if (!exceptions.empty())
        {
            if (std::find(exceptions.begin(), exceptions.end(), iterator) != exceptions.end())
            {
                continue;
            }
        }
        // Check if it is the desired target.
        if (iterator->toMobile()->hasKey(ToLower(target)))
        {
            if (number == 1)
            {
                return iterator->toMobile();
            }
            number -= 1;
        }
    }
    return nullptr;
}

std::shared_ptr<Exit> Room::findExit(Direction direction)
{
    for (auto it : exits)
    {
        if (it->direction == direction)
        {
            return it;
        }
    }
    return nullptr;
}

std::shared_ptr<Exit> Room::findExit(const std::string & direction)
{
    if (Direction::isValid(direction))
    {
        return this->findExit(Direction(direction));
    }
    return nullptr;
}

std::shared_ptr<Exit> Room::findExit(Room * destination)
{
    for (auto it : exits)
    {
        if (it->destination == destination)
        {
            return it;
        }
    }
    return nullptr;
}
Item * Room::findDoor()
{
    for (auto iterator : items)
    {
        if (iterator->model->getType() == ModelType::Mechanism)
        {
            if (HasFlag(iterator->flags, ItemFlag::Built))
            {
                if (iterator->model->toMechanism()->mechanismType == MechanismType::Door)
                {
                    return iterator;
                }
            }
        }
    }
    return nullptr;
}
std::vector<Direction> Room::getAvailableDirections()
{
    std::vector<Direction> directions;
    for (auto it : exits)
    {
        directions.push_back(it->direction);
    }
    return directions;
}
bool Room::addExit(std::shared_ptr<Exit> exit)
{
    if (this->findExit(exit->direction))
    {
        return false;
    }
    this->exits.push_back(std::move(exit));
    return true;
}
bool Room::removeExit(const Direction & direction)
{
    for (auto it : exits)
    {
        if (it->direction == direction)
        {
            FindErase(this->exits, it);
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
            std::vector<std::string> layers = continent->drawFov(
                this,
                exception->getViewDistance());
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
    for (auto it : items)
    {
        // If the item is invisible, don't show it.
        if (HasFlag(it->model->modelFlags, ModelFlag::Invisible))
        {
            continue;
        }

        if (HasFlag(it->flags, ItemFlag::Built))
        {
            output += "[B]";
        }

        // If there are more of this item, show the counter.
        if (it->quantity > 1)
        {
            output += Formatter::cyan() + it->getNameCapital() + Formatter::reset() + " are here.["
                + ToString(it->quantity) + "]\n";
        }
        else
        {
            output += Formatter::cyan() + it->getNameCapital() + Formatter::reset() + " is here.\n";
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

void Room::sendToAll(const std::string & message, const std::vector<Character *> & exceptions)
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

VectorHelper<Exit *> Room::luaGetExits()
{
    VectorHelper<Exit *> ret;
    for (auto it : this->exits)
    {
        ret.push_back(it.get());
    }
    return ret;
}

void Room::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginClass<Room>("Room") //
    .addData("vnum", &Room::vnum, false) //
    .addData("name", &Room::name, false) //
    .addData("coord", &Room::coord, false) //
    .addData("terrain", &Room::terrain, false) //
    .addFunction("getExits", &Room::luaGetExits) //
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

bool CreateRoom(Coordinates coord, Room * source_room)
{
    Room * new_room = new Room();

    // Create a new room.
    Logger::log(LogLevel::Info, "[CreateRoom] Setting up the room...");
    new_room->vnum = Mud::instance().getMaxVnumRoom() + 1;
    new_room->area = source_room->area;
    new_room->coord = coord;
    new_room->terrain = source_room->terrain;
    new_room->name = Generator::instance().generateName(
        source_room->area->type,
        source_room->area->status);
    new_room->description = Generator::instance().generateDescription(
        source_room->area->type,
        source_room->area->status,
        new_room->name);
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
        if (!Mud::instance().remRoom(new_room))
        {
            Logger::log(
                LogLevel::Error,
                "During rollback I was not able to remove the room from the mud.\n");
        }
        if (!new_room->area->remRoom(new_room))
        {
            Logger::log(
                LogLevel::Error,
                "During rollback I was not able to remove the room from the area.\n");
        }
        delete (new_room);
        return false;
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool ConnectRoom(Room * room)
{
    bool status = true;
    ExitVector generatedExits;
    Logger::log(LogLevel::Info, "[ConnectRoom] Connecting the room to near rooms...");
    for (auto iterator : Mud::instance().mudDirections)
    {
        // Get the coordinate modifier.
        Coordinates coordinates = room->coord + iterator.second.getCoordinates();
        // Get the room at the given coordinates.
        Room * near = room->area->getRoom(coordinates);
        if (near != nullptr)
        {
            // Create the two exits.
            std::shared_ptr<Exit> forward = std::make_shared<Exit>(room, near, iterator.second, 0);
            std::shared_ptr<Exit> backward = std::make_shared<Exit>(
                near,
                room,
                forward->getOppositeDirection(),
                0);
            generatedExits.push_back(forward);
            generatedExits.push_back(backward);

            // In case the connection is Up/Down set the presence of stairs.
            if (iterator.second == Direction::Up || iterator.second == Direction::Down)
            {
                SetFlag(forward->flags, ExitFlag::Stairs);
                SetFlag(backward->flags, ExitFlag::Stairs);
            }

            // Insert in both the rooms exits the connection.
            room->addExit(forward);
            near->addExit(backward);

            // Update the values on Database.
            vector<string> arguments;
            arguments.push_back(ToString(forward->source->vnum));
            arguments.push_back(ToString(forward->destination->vnum));
            arguments.push_back(ToString(forward->direction.toUInt()));
            arguments.push_back(ToString(forward->flags));
            if (!SQLiteDbms::instance().insertInto("Exit", arguments))
            {
                Logger::log(LogLevel::Error, "I was not able to save an exit.");
                status = false;
                break;
            }

            vector<string> arguments2;
            arguments2.push_back(ToString(backward->source->vnum));
            arguments2.push_back(ToString(backward->destination->vnum));
            arguments2.push_back(ToString(backward->direction.toUInt()));
            arguments2.push_back(ToString(backward->flags));
            if (!SQLiteDbms::instance().insertInto("Exit", arguments2))
            {
                Logger::log(LogLevel::Error, "I was not able to save an exit.");
                status = false;
                break;
            }
        }
    }
    if (!status)
    {
        Logger::log(LogLevel::Error, "Disconnecting all the rooms.");
        for (auto exitIterator : generatedExits)
        {
            if (!exitIterator->unlink())
            {
                Logger::log(LogLevel::Fatal, "I was not able to remove an unwanted exit.");
            }
        }
    }
    return status;
}

std::string GetRoomFlagString(unsigned int flags)
{
    std::string flagList;
    if (HasFlag(flags, RoomFlag::Rent)) flagList += "|Rent";
    if (HasFlag(flags, RoomFlag::Peaceful)) flagList += "|Peaceful";
    if (HasFlag(flags, RoomFlag::TravelPoint)) flagList += "|TravelPoint";
    if (HasFlag(flags, RoomFlag::SpawnPoint)) flagList += "|SpawnPoint";
    flagList += "|";
    return flagList;
}
