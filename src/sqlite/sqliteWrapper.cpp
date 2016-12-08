/// @file   sqliteWrapper.cpp
/// @brief  Implement a wrapper for interacting with a sqlite3 dbDetails.
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

#include "sqliteWrapper.hpp"

#include "utils.hpp"
#include "logger.hpp"

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
    if (sqlite3_prepare_v2(dbDetails.dbConnection, query, -1, &dbDetails.dbStatement, NULL) != SQLITE_OK)
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

bool SQLiteWrapper::getColumnName(const int & column, std::string & columnName)
{
    // Check if the given column is inside the boundaries.
    if ((column < 0) || (column > num_col))
    {
        Logger::log(LogLevel::Error, "Column index (%s) is outside the boundaries.", column);
        return false;
    }
    columnName = sqlite3_column_name(dbDetails.dbStatement, column);
    return true;
}

bool SQLiteWrapper::getDataString(const int & column, std::string & data)
{
    // Check if the given column is inside the boundaries.
    if ((column < 0) || (column > num_col))
    {
        Logger::log(LogLevel::Error, "Column index (%s) is outside the boundaries.", column);
        return false;
    }
    // Check if the retrieved data is a string.
    if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_TEXT)
    {
        Logger::log(LogLevel::Error, "Column at index (%s) does not contain a Text.", column);
        return false;
    }
    // Check the input in case is a valid value.
    const char * ptr = reinterpret_cast<const char *>(sqlite3_column_text(dbDetails.dbStatement, column));
    if (ptr == nullptr)
    {
        return false;
    }
    // Set the data.
    data = std::string(ptr);
    return true;
}

bool SQLiteWrapper::getDataInteger(const int & column, int & data)
{
    // Check if the given column is inside the boundaries.
    if ((column < 0) || (column > num_col))
    {
        Logger::log(LogLevel::Error, "Column index (%s) is outside the boundaries.", column);
        return false;
    }
    // Check if the retrieved data is an integer.
    if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_INTEGER)
    {
        Logger::log(LogLevel::Error, "Column at index (%s) does not contain an Integer.", column);
        return false;
    }
    data = sqlite3_column_int(dbDetails.dbStatement, column);
    return true;
}

bool SQLiteWrapper::getDataUnsignedInteger(const int & column, unsigned int & data)
{
    // Check if the given column is inside the boundaries.
    if ((column < 0) || (column > num_col))
    {
        Logger::log(LogLevel::Error, "Column index is outside the boundaries.");
        return false;
    }
    // Check if the retrieved data is an integer.
    if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_INTEGER)
    {
        Logger::log(LogLevel::Error, "Column at index (%s) does not contain an Unsigned Integer.", column);
        return false;
    }
    int retrievedData = sqlite3_column_int(dbDetails.dbStatement, column);
    if (retrievedData < 0)
    {
        Logger::log(LogLevel::Error, "Column at index (%s) does not contain an Unsigned Integer.", column);
        return false;
    }
    data = static_cast<unsigned int>(retrievedData);
    return true;
}

bool SQLiteWrapper::getDataDouble(const int & column, double & data)
{
    // Check if the given column is inside the boundaries.
    if ((column < 0) || (column > num_col))
    {
        Logger::log(LogLevel::Error, "Column index is outside the boundaries.");
        return false;
    }
    // Check if the retrieved data is an integer.
    if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_FLOAT)
    {
        if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_INTEGER)
        {
            Logger::log(LogLevel::Error, "Column at index (%s) does not contain a Double.", column);
            return false;
        }
    }
    data = sqlite3_column_double(dbDetails.dbStatement, column);
    return true;
}

std::string SQLiteWrapper::getNextString()
{
    std::string data;
    if (this->getDataString(currentColumn, data))
    {
        // Increase the column index.
        currentColumn++;
        return data;
    }
    else
    {
        return "";
    }
}

int SQLiteWrapper::getNextInteger()
{
    int data;
    if (this->getDataInteger(currentColumn, data))
    {
        // Increase the column index.
        currentColumn++;
        return data;
    }
    else
    {
        return 0;
    }
}

unsigned int SQLiteWrapper::getNextUnsignedInteger()
{
    unsigned int data;
    if (this->getDataUnsignedInteger(currentColumn, data))
    {
        // Increase the column index.
        currentColumn++;
        return data;
    }
    else
    {
        return 0;
    }
}

double SQLiteWrapper::getNextDouble()
{
    double data;
    if (this->getDataDouble(currentColumn, data))
    {
        // Increase the column index.
        currentColumn++;
        return data;
    }
    else
    {
        return 0;
    }
}
