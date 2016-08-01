/// @file   SQLiteDbms.hpp
/// @brief  Definition of the class SQLiteDbms, that allows to interact whit a SQLITE3 Database.
/// @author Enrico Fraccaroli
/// @date   23 Agosto 2014
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

#include <functional>
#include <vector>
#include "SQLiteWrapper.hpp"

class Character;
class Player;
class Skill;
class Item;
class Room;

/// @brief A query list of FIELD+VALUE.
typedef std::vector<std::pair<std::string, std::string> > QueryList;

/// @defgroup SQLLoadFun Sqlite3 Loading Functions
/// @brief All the functions necessary to load information from a SQLITE3 database.

/// @{
/// @brief Function used to retrieve information about Player.
bool LoadPlayerInformation(ResultSet * result, Player * player);
/// @brief Function used to retrieve information about the item posessed by the player.
bool LoadPlayerItems(ResultSet * result, Character * player);
/// @brief Function used to retrieve information about the skills of the player.
bool LoadPlayerSkill(ResultSet * result, Player * player);
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
///@}

/// @brief It's used to connect to a database and retrieve information as well as update them.
class SQLiteDbms
{
    private:
        /// @brief Constructor.
        SQLiteDbms();

        /// @brief Destructor.
        ~SQLiteDbms();

    public:
        /// @brief Disable Copy Construct.
        SQLiteDbms(SQLiteDbms const &) = delete;

        /// @brief Disable Move construct.
        SQLiteDbms(SQLiteDbms &&) = delete;

        /// @brief Disable Copy assign.
        SQLiteDbms & operator=(SQLiteDbms const &) = delete;

        /// @brief Disable Move assign.
        SQLiteDbms & operator=(SQLiteDbms &&) = delete;

        /// @brief Get the singleton istance of the DBMS.
        /// @return The static and uniquie DBMS variable.
        static SQLiteDbms & instance();

        /// @brief Open database connection.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool openDatabase();

        /// @brief Close database connection.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool closeDatabase();

        /// @brief Load tables values.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool loadTables();

        /// @brief Load all the vital information about the player.
        /// @param player The address of the player, where to load information.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool loadPlayer(Player * player);

        /// @brief Search for the player in the table.
        /// @param name The name of the player.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool searchPlayer(std::string name);

        /// @brief Execute an Insert Into query.
        /// @param table     Name of the table.
        /// @param args      Vector of arguments.
        /// @param orIgnore  Flag used to enable the OR IGNORE option.
        /// @param orReplace Flag used to enable the OR REPLACE option.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool insertInto(
            std::string table,
            std::vector<std::string> args,
            bool orIgnore = true,
            bool orReplace = false);

        /// @brief Execute a Delete From query.
        /// @param table The name of the table.
        /// @param where Vector of where clause.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool deleteFrom(std::string table, QueryList where);

        /// @brief Execute an Update query.
        /// @param table The name of the table.
        /// @param value Vector of values.
        /// @param where Vector of where clause.
        /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
        bool updateInto(std::string table, QueryList value, QueryList where);

        /// @brief Execute a select.
        /// @param table The name of the table.
        /// @param where Vector of where clause.
        /// @return The result of the operation.
        ResultSet * executeSelect(std::string table, QueryList where);

        /// @brief Begin a transaction.
        void beginTransaction();

        /// @brief Rollback a transaction.
        void rollbackTransection();

        /// @brief End a Transaction.
        void endTransaction();

    private:
        /// The connection, used to communicate with the database.
        SQLiteWrapper dbConnection;

        /// Loading function for sqlite3 tables.
        typedef std::function<bool(ResultSet * result)> LoadingFunction;
        /// List of the tables with their loader.
        std::vector<std::pair<std::string, LoadingFunction> > tableLoaders;
};
