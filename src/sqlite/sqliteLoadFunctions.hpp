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
/// @brief All the functions necessary to load information from
///         a SQLITE3 database.

/// @{
/// @brief Function used to retrieve information about bad names.
bool LoadBadName(ResultSet * result);

/// @brief Function used to retrieve information about blocked ips.
bool LoadBlockedIp(ResultSet * result);

/// @brief Function used to retrieve information about the news from
///         the developers.
bool LoadNews(ResultSet * result);

/// @brief Function used to retrieve information about items inside containers.
bool LoadContent(ResultSet * result);

/// @brief Function used to retrieve information about all the items.
bool LoadItem(ResultSet * result);

/// @brief Function used to retrieve information about skills.
bool LoadSkill(ResultSet * result);

/// @brief Function used to retrieve information about skills prerequisites.
bool LoadSkillPrerequisite(ResultSet * result);

/// @brief Function used to retrieve information about skills benefits.
bool LoadSkillBenefit(ResultSet * result);

/// @brief Function used to retrieve information about factions.
bool LoadFaction(ResultSet * result);

/// @brief Function used to retrieve information about models of items.
bool LoadModel(ResultSet * result);

/// @brief Function used to retrieve information about races.
bool LoadRace(ResultSet * result);

/// @brief Function used to load body parts associated to races.
bool LoadRaceBodyPart(ResultSet * result);

/// @brief Function used to load corpses associated to races.
bool LoadRaceCorpse(ResultSet * result);

/// @brief Function used to load the base skills of a race.
bool LoadRaceBaseSkill(ResultSet * result);

/// @brief Function used to load the base abilities of a race.
bool LoadRaceBaseAbility(ResultSet * result);

/// @brief Function used to retrieve information about all the mobiles.
bool LoadMobile(ResultSet * result);

/// @brief Function used to retrieve information about all the rooms.
bool LoadRoom(ResultSet * result);

/// @brief Function used to retrieve information about exits of rooms.
bool LoadExit(ResultSet * result);

/// @brief Function used to retrieve information about the item
///         placed inside the rooms.
bool LoadItemRoom(ResultSet * result);

/// @brief Function used to retrieve information about all the areas of the mud.
bool LoadArea(ResultSet * result);

/// @brief Function used to retrieve information about the rooms
///         associated with every area.
bool LoadAreaList(ResultSet * result);

/// @brief Function used to retrieve the writings
bool LoadWriting(ResultSet * result);

/// @brief Function used to retrieve information about all the
///         continents of the mud.
bool LoadContinent(ResultSet * result);

/// @brief Function used to retrieve information about all the materials.
bool LoadMaterial(ResultSet * result);

/// @brief Function used to retrieve information about all the professions.
bool LoadProfession(ResultSet * result);

/// @brief Function used to retrieve information about all the productions.
bool LoadProduction(ResultSet * result);

/// @brief Function used to retrieve information about all the liquids.
bool LoadLiquid(ResultSet * result);

/// @brief Function used to retrieve information about liquids
///         inside containers.
bool LoadContentLiq(ResultSet * result);

/// @brief Function used to retrieve information about all the travelling point.
bool LoadTravelPoint(ResultSet * result);

/// @brief Function used to retrieve information about all the buildings.
bool LoadBuilding(ResultSet * result);

/// @brief Function used to load shops.
bool LoadShop(ResultSet * result);

/// @brief Function used to load currencies.
bool LoadCurrency(ResultSet * result);

/// @brief Function used to load terrains.
bool LoadTerrain(ResultSet * result);

/// @brief Function used to load body parts.
bool LoadBodyPart(ResultSet * result);

/// @brief Function used to load the body parts where a model can be equipped.
bool LoadModelBodyPart(ResultSet * result);

/// @brief Function used to load the resources produced from a body part.
bool LoadBodyPartResources(ResultSet * result);

///@}
