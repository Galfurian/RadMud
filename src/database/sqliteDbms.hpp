/// @file   sqliteDbms.hpp
/// @brief  Definition of the class SQLiteDbms, that allows to interact
///          with a SQLITE3 Database.
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

#include <vector>

#include "sqliteWrapper.hpp"
#include "tableLoader.hpp"

class Player;

/// @brief A query list of FIELD+VALUE.
using QueryList = std::vector<std::pair<std::string, std::string> >;

/// @brief It's used to connect to a database and retrieve information
///         as well as update them.
class SQLiteDbms
{
private:
    /// The connection, used to communicate with the database.
    SQLiteWrapper dbConnection;
    /// List of the tables with their loader.
    std::vector<TableLoader> loaders;

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
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool openDatabase();

    /// @brief Close database connection.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool closeDatabase();

    /// @brief Load tables values.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool loadTables();

    /// @brief Load all the vital information about the player.
    /// @param player The player.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool loadPlayer(Player * player);

    /// @brief Search for the player in the table.
    /// @param name The name of the player.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool searchPlayer(const std::string & name);

    /// @brief Execute an Insert Into query.
    /// @param table     Name of the table.
    /// @param args      Vector of arguments.
    /// @param orIgnore  Flag used to enable the OR IGNORE option.
    /// @param orReplace Flag used to enable the OR REPLACE option.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool insertInto(std::string table,
                    std::vector<std::string> args,
                    bool orIgnore = true,
                    bool orReplace = false);

    /// @brief Execute a Delete From query.
    /// @param table The name of the table.
    /// @param where Vector of where clause.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool deleteFrom(std::string table, QueryList where);

    /// @brief Execute an Update query.
    /// @param table The name of the table.
    /// @param value Vector of values.
    /// @param where Vector of where clause.
    /// @return <b>True</b> if the operations succeeded,<br>
    ///         <b>False</b> Otherwise.
    bool updateInto(std::string table, QueryList value, QueryList where);

    /// Updates all the connected players.
    bool updatePlayers();

    /// Updates all the items.
    bool updateItems();

    /// Update all the rooms.
    bool updateRooms();

    /// Update all the rooms.
    bool updateInMemoryDatabase();

    /// @brief Begin a transaction.
    void beginTransaction();

    /// @brief Rollback a transaction.
    void rollbackTransection();

    /// @brief End a Transaction.
    void endTransaction();

    /// @brief Prints last error message and code.
    void showLastError() const;

private:
    /// @brief Function used to retrieve information about Player.
    bool loadPlayerInformation(ResultSet * result, Player * player);

    /// @brief Function used to retrieve information about the item
    ///         possessed by the player.
    bool loadPlayerItems(Player * player);

    /// @brief Function used to retrieve information about the
    ///         skills of the player.
    bool loadPlayerSkill(Player * player);

    /// @brief Function used to retrieve information about the
    ///         lua variables of the player.
    bool loadPlayerLuaVariables(Player * player);
};
