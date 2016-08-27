/// @file   sqliteLoadFunctions.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 18 2016
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

#pragma once

#include "sqliteWrapper.hpp"

/// @defgroup SQLLoadFun Sqlite3 Loading Functions
/// @brief All the functions necessary to load information from a SQLITE3 database.

/// @{
/// @brief Function used to retrieve information about bad names.
bool LoadBadName(ResultSet * result);
/// @brief Function used to retrieve information about blocked ips.
bool LoadBlockedIp(ResultSet * result);
/// @brief Function used to retrieve information about the news from the developers.
bool LoadNews(ResultSet * result);
/// @brief Function used to retrieve information about items inside containers.
bool LoadContent(ResultSet * result);
/// @brief Function used to retrieve information about all the items.
bool LoadItem(ResultSet * result);
/// @brief Function used to retrieve information about skills.
bool LoadSkill(ResultSet * result);
/// @brief Function used to retrieve information about factions.
bool LoadFaction(ResultSet * result);
/// @brief Function used to retrieve information about models of items.
bool LoadModel(ResultSet * result);
/// @brief Function used to retrieve information about races.
bool LoadRace(ResultSet * result);
/// @brief Function used to retrieve information about all the mobiles.
bool LoadMobile(ResultSet * result);
/// @brief Function used to retrieve information about all the rooms.
bool LoadRoom(ResultSet * result);
/// @brief Function used to retrieve information about exits of rooms.
bool LoadExit(ResultSet * result);
/// @brief Function used to retrieve information about the item placed inside the rooms.
bool LoadItemRoom(ResultSet * result);
/// @brief Function used to retrieve information about all the areas of the mud.
bool LoadArea(ResultSet * result);
/// @brief Function used to retrieve information about the rooms associated with every area.
bool LoadAreaList(ResultSet * result);
/// @brief Function used to retrieve the writings
bool LoadWriting(ResultSet * result);
/// @brief Function used to retrieve information about all the continents of the mud.
bool LoadContinent(ResultSet * result);
/// @brief Function used to retrieve information about all the materials.
bool LoadMaterial(ResultSet * result);
/// @brief Function used to retrieve information about all the professions.
bool LoadProfession(ResultSet * result);
/// @brief Function used to retrieve information about all the productions.
bool LoadProduction(ResultSet * result);
/// @brief Function used to retrieve information about all the liquids.
bool LoadLiquid(ResultSet * result);
/// @brief Function used to retrieve information about liquids inside containers.
bool LoadContentLiq(ResultSet * result);
/// @brief Function used to retrieve information about all the travelling point.
bool LoadTravelPoint(ResultSet * result);
/// @brief Function used to retrieve information about all the buildings.
bool LoadBuilding(ResultSet * result);
/// @brief Function used to load shops.
bool LoadShop(ResultSet * result);
/// @brief Function used to load currencies.
bool LoadCurrency(ResultSet * result);
///@}
