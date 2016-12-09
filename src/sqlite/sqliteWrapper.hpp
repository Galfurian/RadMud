/// @file   sqliteWrapper.hpp
/// @brief  Define a wrapper for interacting with a sqlite3 database.
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

#include "resultSet.hpp"
#include <sqlite3.h>

/// @brief Class necessary to execute query on the Database.
class SQLiteWrapper :
    public ResultSet
{
private:
    /// @brief SQLite Connection Object.
    using DBDetails = struct
    {
        /// SQLite connection object.
        sqlite3 * dbConnection;
        /// Database name.
        std::string dbName;
        /// Databse file directory.
        std::string dbDirectory;
        /// SQLite statement object.
        sqlite3_stmt * dbStatement;
    };

    /// SQLite Connection Details.
    DBDetails dbDetails;

    /// Connection status to the database.
    bool connected;

    /// Last error message.
    std::string errorMessage;

    /// Last error code.
    int errorCode;

    /// Number of column in the result set.
    int num_col;

    /// Current column.
    int currentColumn;

public:
    /// @brief Constructor.
    SQLiteWrapper();

    /// Deconstructor for the class SQLiteWrapper.
    virtual ~SQLiteWrapper();

    /// @brief Open database connection.
    /// @param dbName      The name of the database.
    /// @param dbDirectory The directory where the database resides.
    /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
    bool openConnection(std::string dbName, std::string dbDirectory);

    /// @brief Close database connection.
    /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
    bool closeConnection();

    /// @brief Get the last error message.
    /// @return The string of the last error message.
    std::string getLastErrorMsg() const;

    /// @brief Get the last error code.
    /// @return The last error code.
    int getLastErrorCode() const;

    /// @brief This method is used to execute a SELECT Query.
    /// @param query The query that has to be executed.
    /// @return <b>True</b> if the operations succeeded,<br> <b>False</b> Otherwise.
    ResultSet * executeSelect(const char * query);

    /// @brief This method is used to execute a INSERT/DELETE/UPDATE Query.
    /// @param query The query that has to be executed.
    /// @return The number of affected data by the query.
    int executeQuery(const char * query);

    /// @brief Begin a transaction.
    void beginTransaction();

    /// @brief End a transaction.
    void endTransaction();

    /// @brief Rollback a Transaction.
    void rollbackTransection();

    /// @brief Check if the databse is connected
    /// @return <b>True</b> if databse is connected,<br> <b>False</b> Otherwise.
    bool isConnected();

private:
    bool next() override;

    bool release() override;

    int getColumnCount() override;

    bool getColumnName(const int & column, std::string & columnName) override;

    bool getDataString(const int & column, std::string & data) override;

    bool getDataInteger(const int & column, int & data) override;

    bool getDataUnsignedInteger(const int & column, unsigned int & data) override;

    bool getDataDouble(const int & column, double & data) override;

    std::string getNextString() override;

    int getNextInteger() override;

    unsigned int getNextUnsignedInteger() override;

    double getNextDouble() override;
};
