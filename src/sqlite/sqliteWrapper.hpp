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

#include <sqlite3.h>
#include <string>

/// @brief Interface class for result set of all the query.
class ResultSet
{
public:
    /// @brief Destructor.
    virtual ~ResultSet();

    /// @brief Advance in the rows.
    /// @return <b>True</b> if there are still rows in the resultset of the last executed query,<br>
    ///         <b>False</b> if no row present.
    virtual bool next() = 0;

    /// @brief Release all the result set, as well as reset all the data.
    /// @return <b>True</b> if the finalize is OK,<br>
    ///         <b>False</b> otherwise.
    virtual bool release() = 0;

    /// @brief This function return the number of columns present in the resultset of the last executed query.
    /// @return The number of the columns.
    virtual int getColumnCount() = 0;

    /// @brief Get the name of the given column.
    /// @param column       The column number.
    /// @param columnName   The name of the column.
    /// @return <b>True</b> if the data has been retrieved,<br>
    ///         <b>False</b> otherwise.
    virtual bool getColumnName(const int & column, std::string & columnName) = 0;

    /// @brief Get the given coloumn data as a string.
    /// @param column The column number.
    /// @param data   The string retrieved from the cell.
    /// @return <b>True</b> if the data has been retrieved,<br>
    ///         <b>False</b> otherwise.
    virtual bool getDataString(const int & column, std::string & data) = 0;

    /// @brief Get the given coloumn data as an integer.
    /// @param column The number of the column.
    /// @param data   The integer retrieved from the cell.
    /// @return <b>True</b> if the data has been retrieved,<br>
    ///         <b>False</b> otherwise.
    virtual bool getDataInteger(const int & column, int & data) = 0;

    /// @brief Get the given coloumn data as an unsigned integer.
    /// @param column The number of the column.
    /// @param data   The unsigned integer retrieved from the cell.
    /// @return <b>True</b> if the data has been retrieved,<br>
    ///         <b>False</b> otherwise.
    virtual bool getDataUnsignedInteger(const int & column, unsigned int & data) = 0;

    /// @brief Get the given coloumn data as a double.
    /// @param column The number of the column.
    /// @param data   The double retrieved from the cell.
    /// @return <b>True</b> if the data has been retrieved,<br>
    ///         <b>False</b> otherwise.
    virtual bool getDataDouble(const int & column, double & data) = 0;

    /// @brief Get the next coloumn data as a string.
    /// @return The string retrieved from the cell.
    virtual std::string getNextString() = 0;

    /// @brief Get the next coloumn data as an integer.
    /// @return The integer retrieved from the cell.
    virtual int getNextInteger() = 0;

    /// @brief Get the next coloumn data as an unsigned integer.
    /// @return The unsigned integer retrieved from the cell.
    virtual unsigned int getNextUnsignedInteger() = 0;

    /// @brief Get the next coloumn data as a double.
    /// @return The double retrieved from the cell.
    virtual double getNextDouble() = 0;
};

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

    /// @brief Get the last error message.
    /// @return The string of the last error message.
    std::string getLastErrorMsg() const;

    /// @brief Get the last error code.
    /// @return The last error code.
    int getLastErrorCode() const;

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
