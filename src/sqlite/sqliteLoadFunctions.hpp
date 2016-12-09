/// @file   sqliteLoadFunctions.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 18 2016
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

#include "sqliteWrapper.hpp"

/// @defgroup SQLLoadFun Sqlite3 Loading Functions
/// @brief All the functions necessary to load information from a SQLITE3 database.

/// @{
/// @brief Function used to retrieve information about bad names.
bool LoadBadName(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about blocked ips.
bool LoadBlockedIp(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about the news from the developers.
bool LoadNews(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about items inside containers.
bool LoadContent(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the items.
bool LoadItem(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about skills.
bool LoadSkill(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about factions.
bool LoadFaction(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about models of items.
bool LoadModel(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about races.
bool LoadRace(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the mobiles.
bool LoadMobile(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the rooms.
bool LoadRoom(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about exits of rooms.
bool LoadExit(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about the item placed inside the rooms.
bool LoadItemRoom(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the areas of the mud.
bool LoadArea(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about the rooms associated with every area.
bool LoadAreaList(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve the writings
bool LoadWriting(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the continents of the mud.
bool LoadContinent(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the materials.
bool LoadMaterial(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the professions.
bool LoadProfession(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the productions.
bool LoadProduction(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the liquids.
bool LoadLiquid(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about liquids inside containers.
bool LoadContentLiq(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the travelling point.
bool LoadTravelPoint(std::shared_ptr<ResultSet> result);

/// @brief Function used to retrieve information about all the buildings.
bool LoadBuilding(std::shared_ptr<ResultSet> result);

/// @brief Function used to load shops.
bool LoadShop(std::shared_ptr<ResultSet> result);

/// @brief Function used to load currencies.
bool LoadCurrency(std::shared_ptr<ResultSet> result);

/// @brief Function used to load terrains.
bool LoadTerrain(std::shared_ptr<ResultSet> result);
///@}
