/// @file   sqliteWrapper.cpp
/// @brief  Implement a wrapper for interacting with a sqlite3 dbDetails.
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

#include "sqliteWrapper.hpp"

#include "../utils.hpp"
#include "../utilities/logger.hpp"

ResultSet::~ResultSet()
{

}

SQLiteWrapper::SQLiteWrapper()
{
    currentColumn = 0;
    num_col = 0;
    errorCode = 0;
    connected = false;
}

SQLiteWrapper::~SQLiteWrapper()
{
}

bool SQLiteWrapper::openConnection(std::string dbName, std::string dbDirectory)
{
    dbDetails.dbName = dbName;
    dbDetails.dbDirectory = dbDirectory;

    connected = true;

    errorCode = sqlite3_open(
        (dbDetails.dbDirectory + dbDetails.dbName).c_str(),
        &(dbDetails.dbConnection));
    errorMessage = sqlite3_errmsg(dbDetails.dbConnection);

    if (errorCode != SQLITE_OK)
    {
        if (errorMessage.find("not an error") == std::string::npos)
        {
            connected = false;
        }
    }
    errorCode = executeQuery("PRAGMA foreign_keys = ON;");
    if (errorCode != SQLITE_OK)
    {
        connected = false;
    }

    return connected;
}

bool SQLiteWrapper::closeConnection()
{
    bool result = false;
    if (dbDetails.dbConnection)
    {
        errorCode = sqlite3_close(dbDetails.dbConnection);
        if (errorCode != SQLITE_OK)
        {
            if (errorCode == SQLITE_BUSY)
            {
                bool retry = false;
                int numberOfRetries = 0;
                Logger::log(LogLevel::Error, "Database busy, can't be closed.");
                do
                {
                    errorCode = sqlite3_close(dbDetails.dbConnection);
                    if (SQLITE_BUSY == errorCode)
                    {
                        retry = true;
                        if (numberOfRetries++ > 10)
                        {
                            sqlite3_finalize(dbDetails.dbStatement);
                        }
                    }
                } while (retry);
            }
        }
        else
        {
            result = true;
        }
    }
    return result;
}

ResultSet * SQLiteWrapper::executeSelect(const char * query)
{
    if (!isConnected())
    {
        return NULL;
    }
    if (sqlite3_prepare_v2(dbDetails.dbConnection, query, -1, &dbDetails.dbStatement,
                           NULL) != SQLITE_OK)
    {
        errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
        errorCode = sqlite3_finalize(dbDetails.dbStatement);
        Logger::log(LogLevel::Error, "Error code :" + ToString(errorCode));
        Logger::log(LogLevel::Error, "Last error :" + errorMessage);
        return NULL;
    }
    else
    {
        num_col = sqlite3_column_count(dbDetails.dbStatement);
        ResultSet * ires = this;
        return ires;
    }
}

int SQLiteWrapper::executeQuery(const char * query)
{
    if (!isConnected())
    {
        return 0;
    }

    errorCode = sqlite3_exec(dbDetails.dbConnection, query, NULL, NULL, NULL);
    errorMessage = sqlite3_errmsg(dbDetails.dbConnection);

    if (errorCode != SQLITE_OK)
    {
        Logger::log(LogLevel::Error, "Error code :" + ToString(errorCode));
        Logger::log(LogLevel::Error, "Last error :" + errorMessage);
        return 0;
    }
    return sqlite3_total_changes(dbDetails.dbConnection);
}

void SQLiteWrapper::beginTransaction()
{
    executeQuery("BEGIN TRANSACTION");
}

void SQLiteWrapper::endTransaction()
{
    executeQuery("END TRANSACTION");
}

void SQLiteWrapper::rollbackTransection()
{
    executeQuery("ROLLBACK TRANSACTION");
}

std::string SQLiteWrapper::getLastErrorMsg() const
{
    return errorMessage;
}

int SQLiteWrapper::getLastErrorCode() const
{
    return errorCode;
}

bool SQLiteWrapper::isConnected()
{
    return connected;
}

bool SQLiteWrapper::next()
{
    // Reset the column number.
    currentColumn = 0;
    return (sqlite3_step(dbDetails.dbStatement) == SQLITE_ROW);
}

bool SQLiteWrapper::release()
{
    errorCode = sqlite3_finalize(dbDetails.dbStatement);
    if (errorCode != SQLITE_OK)
    {
        errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
        Logger::log(LogLevel::Error, "Error code :" + ToString(errorCode));
        Logger::log(LogLevel::Error, "Last error :" + errorMessage);
        return false;
    }
    num_col = 0;
    errorMessage = "";
    return true;
}

int SQLiteWrapper::getColumnCount()
{
    return num_col;
}

std::string SQLiteWrapper::getColumnName(const int & column)
{
    if (column > num_col)
    {
        return "";
    }
    return sqlite3_column_name(dbDetails.dbStatement, column);
}

std::string SQLiteWrapper::getDataString(const int & column)
{
    // Check if the given column is beyond the limit.
    if (column > num_col)
    {
        return "";
    }
    // Check the input in case is a valid value.
    const char * ptr = reinterpret_cast<const char *>(sqlite3_column_text(
        dbDetails.dbStatement,
        column));
    if (ptr)
    {
        return std::string(ptr);
    }
    else
    {
        return "";
    }
}

int SQLiteWrapper::getDataInteger(const int & column)
{
    // Check if the given column is beyond the limit.
    if (column > num_col)
    {
        return -1;
    }
    return sqlite3_column_int(dbDetails.dbStatement, column);
}

std::string SQLiteWrapper::getNextString()
{
    // Check if the column is beyond the limit.
    if (currentColumn >= num_col)
    {
        release();
        throw std::runtime_error(
            "Number column exceded (" + ToString(currentColumn) + ">=" + ToString(num_col) + ").");
    }
    // Check the input in case is a valid value.
    const char * ptr = reinterpret_cast<const char *>(sqlite3_column_text(
        dbDetails.dbStatement,
        currentColumn));
    // Increase the column index.
    currentColumn += 1;
    if (ptr)
    {
        return std::string(ptr);
    }
    else
    {
        return "";
    }
}

int SQLiteWrapper::getNextInteger()
{
    // Check if the column is beyond the limit.
    if (currentColumn >= num_col)
    {
        release();
        throw std::runtime_error(
            "Number column exceded (" + ToString(currentColumn) + ">=" + ToString(num_col) + ").");
    }
    int value = sqlite3_column_int(dbDetails.dbStatement, currentColumn);
    // Increase the column index.
    currentColumn += 1;
    return value;
}

unsigned int SQLiteWrapper::getNextUnsignedInteger()
{
    // Check if the column is beyond the limit.
    if (currentColumn >= num_col)
    {
        release();
        throw std::runtime_error(
            "Number column exceded (" + ToString(currentColumn) + ">=" + ToString(num_col) + ").");
    }
    int value = sqlite3_column_int(dbDetails.dbStatement, currentColumn);
    // Increase the column index.
    currentColumn += 1;
    if (value < 0)
    {
        return 0;
    }
    else
    {
        return static_cast<unsigned int>(value);
    }
}

double SQLiteWrapper::getNextDouble()
{
    // Check if the column is beyond the limit.
    if (currentColumn >= num_col)
    {
        release();
        throw std::runtime_error(
            "Number column exceded (" + ToString(currentColumn) + ">=" + ToString(num_col) + ").");
    }
    auto value = sqlite3_column_double(dbDetails.dbStatement, currentColumn);
    // Increase the column index.
    currentColumn += 1;
    return value;
}