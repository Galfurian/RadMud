/// @file   sqliteWrapper.hpp
/// @brief  Define a wrapper for interacting with a sqlite3 database.
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

#pragma once

#include <string>
#include <sqlite3.h>

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
        /// @param column The column number.
        /// @return The name of the column.
        virtual std::string getColumnName(const int & column) = 0;

        /// @brief Get the given coloumn data as a string.
        /// @param column The column number.
        /// @return The string retrieved from the cell.
        virtual std::string getDataString(const int & column) = 0;

        /// @brief Get the given coloumn data as an integer.
        /// @param column The number of the column.
        /// @return The integer retrieved from the cell.
        virtual int getDataInteger(const int & column) = 0;

        /// @brief Get the next coloumn data as a string.
        /// @return The string retrieved from the cell.
        virtual std::string getNextString() = 0;

        /// @brief Get the next coloumn data as an integer.
        /// @return The integer retrieved from the cell.
        virtual int getNextInteger() = 0;

        /// @brief Get the next coloumn data as an unsigned integer.
        /// @return The unsigned integer retrieved from the cell.
        virtual unsigned int getNextUnsignedInteger() = 0;
};

/// @brief Class necessary to execute query on the Database.
class SQLiteWrapper: public ResultSet
{
    public:
        /// @brief Constructor.
        SQLiteWrapper();

        /// Deconstructor for the class SQLiteWrapper.
        ~SQLiteWrapper();

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
        std::string getLastErrorMsg();

        /// @brief Get the last error code.
        /// @return The last error code.
        int getLastErrorCode();

        /// @brief Check if the databse is connected
        /// @return <b>True</b> if databse is connected,<br> <b>False</b> Otherwise.
        bool isConnected();

    private:
        /// @brief SQLite Connection Object.
        typedef struct
        {
            public:
                /// SQLite connection object.
                sqlite3 * dbConnection;
                /// Database name.
                std::string dbName;
                /// Databse file directory.
                std::string dbDirectory;
                /// SQLite statement object.
                sqlite3_stmt * dbStatement;
        } DBDetails;

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

        /// SQLite Connection Details.
        DBDetails dbDetails;

        bool next();
        bool release();
        int getColumnCount();
        std::string getColumnName(const int & column);
        std::string getDataString(const int & column);
        int getDataInteger(const int & column);
        std::string getNextString();
        int getNextInteger();
        unsigned int getNextUnsignedInteger();
};
