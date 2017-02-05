/// @file   sqliteWriteFunctions.cpp
/// @author Enrico Fraccaroli
/// @date   feb 05 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "sqliteWriteFunctions.hpp"
#include "sqliteDbms.hpp"
#include "logger.hpp"
#include "area.hpp"
#include "room.hpp"

bool SaveArea(Area * area)
{
    std::vector<std::string> arguments;
    arguments.push_back(ToString(area->vnum));
    arguments.push_back(area->name);
    arguments.push_back(area->builder);
    arguments.push_back(ToString(area->width));
    arguments.push_back(ToString(area->height));
    arguments.push_back(ToString(area->elevation));
    arguments.push_back("0");
    arguments.push_back(EnumToString(area->type));
    arguments.push_back(EnumToString(area->status));
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Area", arguments, false, true))
    {
        Logger::log(LogLevel::Error, "I was not able to save the area.");
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool SaveRoom(Room * room)
{
    // Insert into table Room the newly created room.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(room->vnum));
    arguments.push_back(ToString(room->coord.x));
    arguments.push_back(ToString(room->coord.y));
    arguments.push_back(ToString(room->coord.z));
    arguments.push_back(ToString(room->terrain->vnum));
    arguments.push_back(room->name);
    arguments.push_back(room->description);
    arguments.push_back(ToString(room->flags));
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Room", arguments, false, true))
    {
        Logger::log(LogLevel::Error, "I was not able to save the room.");
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool SaveAreaList(Area * area, Room * room)
{
    // Insert Room in AreaList.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(area->vnum));
    arguments.push_back(ToString(room->vnum));
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("AreaList", arguments, false, true))
    {
        Logger::log(LogLevel::Error, "I was not able to save the area list.");
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}

bool SaveRoomExit(const std::shared_ptr<Exit> & roomExit)
{
    // Update the values on Database.
    std::vector<std::string> arguments;
    arguments.push_back(ToString(roomExit->source->vnum));
    arguments.push_back(ToString(roomExit->destination->vnum));
    arguments.push_back(ToString(roomExit->direction.toUInt()));
    arguments.push_back(ToString(roomExit->flags));
    // Start a transaction.
    SQLiteDbms::instance().beginTransaction();
    if (!SQLiteDbms::instance().insertInto("Exit", arguments, false, true))
    {
        Logger::log(LogLevel::Error, "I was not able to save the exit.");
        SQLiteDbms::instance().rollbackTransection();
        return false;
    }
    SQLiteDbms::instance().endTransaction();
    return true;
}