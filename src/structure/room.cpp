/// @file   room.cpp
/// @brief  Implements room methods.
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

#include "structure/room.hpp"

#include "model/submodel/mechanismModel.hpp"
#include "model/submodel/lightModel.hpp"
#include "item/subitem/lightItem.hpp"
#include "structure/generator.hpp"
#include "utilities/logger.hpp"
#include "mud.hpp"
#include "structure/structureUtils.hpp"

Room::Room() :
    vnum(),
    area(),
    coord(),
    terrain(),
    name(),
    description(),
    exits(),
    items(),
    characters(),
    flags(),
    liquidContent()
{
    // Nothing to do.
}

Room::~Room()
{
    for (auto it : exits)
    {
        auto oppositeExit = it->getOppositeExit();
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
//    Logger::log(LogLevel::Debug,
//                "Deleted room\t\t[%s]\t\t(%s)",
//                ToString(vnum),
//                name);
}

bool Room::check(bool complete)
{
    if (vnum <= 0) return false;
    if (complete)
    {
        if (area == nullptr) return false;
    }
    if (coord.x > 100) return false;
    if (coord.y > 100) return false;
    if (coord.z > 100) return false;
    if (terrain == nullptr) return false;
    if (name.empty()) return false;
    return true;
}

void Room::addItem(Item *& item, bool updateDB)
{
    // Add the item.
    items.push_back_item(item);
    // Set the room attribute of the item.
    item->room = this;
    // Update the database.
    if (updateDB && (item->getType() != ModelType::Corpse))
    {
        SQLiteDbms::instance().insertInto(
            "ItemRoom",
            {
                ToString(vnum), ToString(item->vnum)
            }, false, true);
    }
    Logger::log(LogLevel::Debug,
                "Item '%s' added to '%s';", item->getName(), name);
}

void Room::addBuilding(Item * item, bool updateDB)
{
    // Set the item as built.
    SetFlag(item->flags, ItemFlag::Built);
    // Check if the item is already inside the room.
    for (auto iterator : items)
    {
        if (iterator->vnum == item->vnum)
        {
            return;
        }
    }
    // Otherwise, add the item.
    this->addItem(item, updateDB);
}

void Room::addCharacter(Character * character)
{
    characters.push_back(character);
    character->room = this;
}

bool Room::removeItem(Item * item, bool updateDB)
{
    if (items.removeItem(item))
    {
        item->room = nullptr;
        // Update the database.
        if (updateDB && (item->getType() != ModelType::Corpse))
        {
            SQLiteDbms::instance().deleteFrom(
                "ItemRoom",
                {
                    std::make_pair("item", ToString(item->vnum))
                });
        }
        // Log it.
        Logger::log(LogLevel::Debug, "Item '%s' removed from '%s';",
                    item->getName(),
                    name);
        return true;
    }
    return false;
}

bool Room::removeBuilding(Item * item, bool updateDB)
{
    if (this->removeItem(item, updateDB))
    {
        // Clear the built flag from the item.
        ClearFlag(item->flags, ItemFlag::Built);
        return true;
    }
    return false;
}

void Room::removeCharacter(Character * character)
{
    for (auto it = characters.begin(); it != characters.end(); ++it)
    {
        if ((*it)->name == character->name)
        {
            characters.erase(it);
            character->room = nullptr;
            return;
        }
    }
    Logger::log(LogLevel::Error, "Error during character removal from room.");
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
    std::vector<std::string> arguments;
    arguments.push_back(ToString(vnum));
    arguments.push_back(ToString(coord.x));
    arguments.push_back(ToString(coord.y));
    arguments.push_back(ToString(coord.z));
    arguments.push_back(ToString(terrain->vnum));
    arguments.push_back(name);
    arguments.push_back(description);
    arguments.push_back(ToString(flags));
    return SQLiteDbms::instance().insertInto("Room", arguments, false, true);
}

bool Room::removeOnDB()
{
    if (items.size() > 0)
    {
        Logger::log(LogLevel::Error,
                    "[Room::RemoveOnDB] There are still items in the room!");
        return false;
    }
    if (characters.size() > 0)
    {
        Logger::log(LogLevel::Error,
                    "[Room::RemoveOnDB] There are still characters in the room!");
        return false;
    }
    SQLiteDbms::instance().deleteFrom("Room",
                                      {
                                          std::make_pair("vnum", ToString(vnum))
                                      });
    return true;
}

Item * Room::findBuilding(std::string target, int & number)
{
    for (auto iterator : items)
    {
        if (iterator->hasKey(ToLower(target)) &&
            HasFlag(iterator->flags, ItemFlag::Built))
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
        if ((iterator->model->getType() == type) &&
            HasFlag(iterator->flags, ItemFlag::Built))
        {
            buildingsList.push_back(iterator);
        }
    }
    return buildingsList;
}

Player * Room::findPlayer(
    std::string target,
    int & number,
    const std::vector<Character *> & exceptions) const
{
    Character * foundCharacter = characters.findCharacter(target,
                                                          number,
                                                          exceptions,
                                                          true);
    if (foundCharacter != nullptr)
    {
        return foundCharacter->toPlayer();
    }
    return nullptr;
}

Mobile * Room::findMobile(
    std::string target,
    int & number,
    const std::vector<Character *> & exceptions) const
{
    Character * foundCharacter = characters.findCharacter(target,
                                                          number,
                                                          exceptions,
                                                          false,
                                                          true);
    if (foundCharacter != nullptr)
    {
        return foundCharacter->toMobile();
    }
    return nullptr;
}

std::shared_ptr<Exit> Room::findExit(Direction direction)
{
    for (auto it = exits.begin(); it != exits.end(); ++it)
    {
        if ((*it)->direction == direction)
        {
            return *it;
        }
    }
    return nullptr;
}

bool Room::isLit()
{
//    Stopwatch<std::chrono::microseconds> stopwatch("isList");
    auto CheckRoomForLights = [this](Room * room)
    {
        auto LightIsActiveAndInRange = [this, room](Item * item)
        {
            if (item != nullptr)
            {
                if (item->getType() == ModelType::Light)
                {
                    if (static_cast<LightItem *>(item)->isActive())
                    {
                        if (StructUtils::getDistance(coord, room->coord)
                            <= item->model->toLight()->radius)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        };
        for (auto it : room->items)
        {
            if (LightIsActiveAndInRange(it))
            {
                return true;
            }
        }
        for (auto it: room->characters)
        {
            for (auto it2: it->equipment)
            {
                if (LightIsActiveAndInRange(it2))
                {
                    return true;
                }
            }
        }
        return false;
    };
    // If the room has a natural light.
    if (HasFlag(terrain->flags, TerrainFlag::NaturalLight))
    {
        return true;
    }
    // Get the day phase.
    auto dayPhase = MudUpdater::instance().getDayPhase();
    if (!HasFlag(terrain->flags, TerrainFlag::Indoor) &&
        (dayPhase != DayPhase::Night))
    {
//        Logger::log(LogLevel::Debug,
//                    "Room is lit (outside)(!night)(%s us)",
//                    stopwatch.stop());
        return true;
    }
    // First check inside the current room.
    auto validCoordinates = area->fov(coord, 10);
    for (auto coordinates : validCoordinates)
    {
        auto room = area->getRoom(coordinates);
        if (room != nullptr)
        {
            if (CheckRoomForLights(room))
            {
//                Logger::log(LogLevel::Debug,
//                            "Room is lit (LitByLight)     (%s us)",
//                            stopwatch.stop());
                return true;
            }
        }
    }
//    Logger::log(LogLevel::Debug,
//                "Room is not lit              (%s us)",
//                stopwatch.stop());
    return false;
}

bool Room::addExit(const std::shared_ptr<Exit> & exit)
{
    if (this->findExit(exit->direction))
    {
        return false;
    }
    exits.emplace_back(exit);
    return true;
}

bool Room::removeExit(const Direction & direction)
{
    for (auto it = exits.begin(); it != exits.end(); ++it)
    {
        if ((*it)->direction == direction)
        {
            exits.erase(it);
            return true;
        }
    }
    return false;
}

std::string Room::getLook(Character * actor)
{
    std::string output = "";
    // Check if the room is lit.
    bool roomIsLit = this->isLit();
    // Show the name of the room.
    output += Formatter::bold() + name + Formatter::reset() + "\n";
    // Show the description of the room only if it is lit.
    if (roomIsLit)
    {
        output += description + "\n";
    }
    else
    {
        output += "You don't see anything nearby.\n";
    }
    // Show the map.
    if (exits.empty())
    {
        output += "There is no exit from here!\n";
    }
    else
    {
        if (area != nullptr)
        {
            if (Formatter::getFormat() == Formatter::CLIENT)
            {
                output += Formatter::doClearMap();
                for (auto layer : area->drawFov(this, actor->getViewDistance()))
                {
                    output += Formatter::doDrawMap();
                    output += layer;
                    output += Formatter::dontDrawMap();
                }
            }
            else
            {
                output += area->drawASCIIFov(this,
                                             actor->getViewDistance());
            }
        }
    }
    // Show the characters/items inside the room only if it is lit.
    if (roomIsLit)
    {
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
                output += Formatter::cyan() + it->getNameCapital() +
                          Formatter::reset() + " are here.[" +
                          ToString(it->quantity) + "]\n";
            }
            else
            {
                output += Formatter::cyan() + it->getNameCapital() +
                          Formatter::reset() + " is here.\n";
            }
        }
        // List mobile in the same room.
        for (auto iterator : getAllMobile(actor))
        {
            if (!actor->canSee(iterator))
            {
                continue;
            }
            output += Formatter::blue() + iterator->getStaticDesc() +
                      Formatter::reset() + "\n";
        }
        // List other players in the same room.
        for (auto iterator : getAllPlayer(actor))
        {
            if (!actor->canSee(iterator))
            {
                continue;
            }
            output += Formatter::gray() + iterator->getStaticDesc() +
                      Formatter::reset() + "\n";
        }
    }
    return output;
}

void Room::sendToAll(const std::string & message,
                     const std::vector<Character *> & exceptions)
{
    for (auto iterator : characters)
    {
        if (!exceptions.empty())
        {
            if (std::find(exceptions.begin(), exceptions.end(), iterator) !=
                exceptions.end())
            {
                continue;
            }
        }
        iterator->sendMsg(message + "\n");
    }
}

void Room::funcSendToAll(const std::string & message,
                         std::function<bool(Character * character)> checkException)
{
    for (auto iterator : characters)
    {
        if (checkException)
        {
            if (!checkException(iterator))
            {
                continue;
            }
        }
        iterator->sendMsg(message + "\n");
    }
}

bool Room::operator<(const Room & right) const
{
    return vnum < right.vnum;
}

bool Room::operator==(const Room & right) const
{
    return vnum == right.vnum;
}

void Room::updateTicImpl()
{
    // Nothing to do.
}

void Room::updateHourImpl()
{
    // Nothing to do.
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
    Logger::log(LogLevel::Info,
                "[CreateRoom] Saving room details on the database...");
    std::vector<std::string> arguments;
    arguments.push_back(ToString(new_room->vnum));
    arguments.push_back(ToString(new_room->coord.x));
    arguments.push_back(ToString(new_room->coord.y));
    arguments.push_back(ToString(new_room->coord.z));
    arguments.push_back(ToString(new_room->terrain->vnum));
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
    std::vector<std::string> arguments2;
    arguments2.push_back(ToString(new_room->area->vnum));
    arguments2.push_back(ToString(new_room->vnum));
    if (!SQLiteDbms::instance().insertInto("AreaList", arguments2))
    {
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    // Add the created room to the room_map.
    Logger::log(LogLevel::Info,
                "[CreateRoom] Adding the room to the global list...");
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
    std::vector<std::shared_ptr<Exit> > generatedExits;
    Logger::log(LogLevel::Info,
                "[ConnectRoom] Connecting the room to near rooms...");
    std::vector<Direction> directions = {
        Direction::North,
        Direction::South,
        Direction::West,
        Direction::East,
        Direction::Up,
        Direction::Down
    };
    for (auto direction : directions)
    {
        // Get the coordinate modifier.
        auto coordinates = room->coord + direction.getCoordinates();
        // Get the room at the given coordinates.
        auto near = room->area->getRoom(coordinates);
        if (near != nullptr)
        {
            // Create the two exits.
            auto forward = std::make_shared<Exit>(room,
                                                  near,
                                                  direction, 0);
            auto backward = std::make_shared<Exit>(near,
                                                   room,
                                                   direction.getOpposite(),
                                                   0);
            generatedExits.push_back(forward);
            generatedExits.push_back(backward);
            // In case the connection is Up/Down set the presence of stairs.
            if (direction == Direction::Up || direction == Direction::Down)
            {
                SetFlag(forward->flags, ExitFlag::Stairs);
                SetFlag(backward->flags, ExitFlag::Stairs);
            }
            // Insert in both the rooms exits the connection.
            room->addExit(forward);
            near->addExit(backward);
            // Update the values on Database.
            std::vector<std::string> arguments;
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
            std::vector<std::string> arguments2;
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
                Logger::log(LogLevel::Fatal,
                            "I was not able to remove an unwanted exit.");
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
    if (HasFlag(flags, RoomFlag::SpawnTree)) flagList += "|SpawnTree";
    flagList += "|";
    return flagList;
}
