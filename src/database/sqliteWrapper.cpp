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
#include "sqliteException.hpp"
#include "logger.hpp"
#include "utils.hpp"

SQLiteWrapper::SQLiteWrapper() :
	dbDetails(),
	errorMessage(),
	errorCode(),
	num_col(),
	currentColumn()
{
	// Nothing to do.
}

SQLiteWrapper::~SQLiteWrapper()
{
	// Nothing to do.
}

bool SQLiteWrapper::openConnection(const std::string &dbName,
								   const std::string &dbDirectory,
								   const bool &openInMemory)
{
	// Set the database details.
	dbDetails.dbName = dbName;
	dbDetails.dbDirectory = dbDirectory;
	dbDetails.connected = true;
	dbDetails.dbIsInMemory = openInMemory;
	auto dbPath = dbDetails.dbDirectory + dbDetails.dbName;
	// Open an in-memory database.
	if (openInMemory) {
		// First open the database in memory.
		errorCode = sqlite3_open(":memory:", &(dbDetails.dbConnection));
		errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
		if (errorCode != SQLITE_OK) {
			if (errorMessage.find("not an error") == std::string::npos) {
				dbDetails.connected = false;
				return false;
			}
		}
		errorCode = this->loadOrSaveDb(false);
		errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
		if (errorCode != SQLITE_OK) {
			if (errorMessage.find("not an error") == std::string::npos) {
				dbDetails.connected = false;
				return false;
			}
		}
	} else {
		// Open the database.
		errorCode = sqlite3_open(dbPath.c_str(), &(dbDetails.dbConnection));
		errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
		if (errorCode != SQLITE_OK) {
			if (errorMessage.find("not an error") == std::string::npos) {
				dbDetails.connected = false;
				return false;
			}
		}
	}
	errorCode = this->executeQuery("PRAGMA foreign_keys=ON;");
	errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
	if (errorCode != SQLITE_OK) {
		if (errorMessage.find("not an error") == std::string::npos) {
			dbDetails.connected = false;
			return false;
		}
	}
	return true;
}

bool SQLiteWrapper::updateInMemoryDatabase()
{
	if (!dbDetails.dbIsInMemory)
		return true;
	auto dbPath = dbDetails.dbDirectory + dbDetails.dbName;
	errorCode = this->loadOrSaveDb(true);
	errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
	if (errorCode != SQLITE_OK) {
		MudLog(LogLevel::Error,
			   "Error while saving the in-memory database to file.");
		return false;
	}
	return true;
}

bool SQLiteWrapper::closeConnection()
{
	if (dbDetails.dbConnection) {
		bool retry = false;
		int numberOfRetries = 0;
		do {
			// If the database has been opened in memory, save it to the file.
			this->updateInMemoryDatabase();

			errorCode = sqlite3_close(dbDetails.dbConnection);
			if (errorCode == SQLITE_OK) {
				return true;
			}
			if (errorCode == SQLITE_BUSY) {
				MudLog(LogLevel::Error, "Database busy, can't be closed.");
				retry = true;
				if (numberOfRetries++ > 10) {
					sqlite3_finalize(dbDetails.dbStatement);
				}
			}
		} while (retry);
	}
	return false;
}

std::string SQLiteWrapper::getLastErrorMsg() const
{
	return errorMessage;
}

int SQLiteWrapper::getLastErrorCode() const
{
	return errorCode;
}

ResultSet *SQLiteWrapper::executeSelect(const char *query)
{
	if (this->isConnected()) {
		if (sqlite3_prepare_v2(dbDetails.dbConnection, query, -1,
							   &dbDetails.dbStatement, NULL) == SQLITE_OK) {
			num_col = sqlite3_column_count(dbDetails.dbStatement);
			return this;
		}
		errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
		errorCode = sqlite3_finalize(dbDetails.dbStatement);
		MudLog(LogLevel::Error, "Error code : %d", errorCode);
		MudLog(LogLevel::Error, "Last error : %s", errorMessage);
	}
	return nullptr;
}

int SQLiteWrapper::executeQuery(const char *query)
{
	if (!isConnected()) {
		MudLog(LogLevel::Error, "Cannot execute query, db is not connected!");
		return 0;
	}
	errorCode = sqlite3_exec(dbDetails.dbConnection, query, NULL, NULL, NULL);
	errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
	if (errorCode != SQLITE_OK) {
		MudLog(LogLevel::Error, "Error code : %d", errorCode);
		MudLog(LogLevel::Error, "Last error : %s", errorMessage);
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

bool SQLiteWrapper::isConnected()
{
	return dbDetails.connected;
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
	if (errorCode != SQLITE_OK) {
		errorMessage = sqlite3_errmsg(dbDetails.dbConnection);
		MudLog(LogLevel::Error, "Error code : %d", errorCode);
		MudLog(LogLevel::Error, "Last error : %s", errorMessage);
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

bool SQLiteWrapper::getColumnName(const int &column, std::string &columnName)
{
	// Check if the given column is inside the boundaries.
	if ((column < 0) || (column > num_col)) {
		errorMessage = "Column index (" + ToString(column) +
					   ") is outside the boundaries.";
		errorCode = SQLITE_CONSTRAINT;
		return false;
	}
	columnName = sqlite3_column_name(dbDetails.dbStatement, column);
	return true;
}

bool SQLiteWrapper::getDataString(const int &column, std::string &data)
{
	// Check if the given column is inside the boundaries.
	if ((column < 0) || (column > num_col)) {
		errorMessage = "Column index (" + ToString(column) +
					   ") is outside the boundaries.";
		errorCode = SQLITE_CONSTRAINT;
		return false;
	}
	// Check if the retrieved data is a string.
	if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_TEXT) {
		errorMessage = "Column at index (" + ToString(column) +
					   ") is not of type SQLITE_TEXT.";
		errorCode = SQLITE_MISMATCH;
		return false;
	}
	// Check the input in case is a valid value.
	const char *ptr = reinterpret_cast<const char *>(
		sqlite3_column_text(dbDetails.dbStatement, column));
	if (ptr == nullptr) {
		return false;
	}
	// Set the data.
	data = std::string(ptr);
	return true;
}

bool SQLiteWrapper::getDataInteger(const int &column, int &data)
{
	// Check if the given column is inside the boundaries.
	if ((column < 0) || (column > num_col)) {
		errorMessage = "Column index (" + ToString(column) +
					   ") is outside the boundaries.";
		errorCode = SQLITE_CONSTRAINT;
		return false;
	}
	// Check if the retrieved data is an integer.
	if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_INTEGER) {
		errorMessage = "Column at index (" + ToString(column) +
					   ") is not of type SQLITE_INTEGER.";
		errorCode = SQLITE_MISMATCH;
		return false;
	}
	data = sqlite3_column_int(dbDetails.dbStatement, column);
	return true;
}

bool SQLiteWrapper::getDataUnsignedInteger(const int &column,
										   unsigned int &data)
{
	// Check if the given column is inside the boundaries.
	if ((column < 0) || (column > num_col)) {
		errorMessage = "Column index (" + ToString(column) +
					   ") is outside the boundaries.";
		errorCode = SQLITE_CONSTRAINT;
		return false;
	}
	// Check if the retrieved data is an integer.
	if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_INTEGER) {
		errorMessage = "Column at index (" + ToString(column) +
					   ") is not of type SQLITE_INTEGER.";
		errorCode = SQLITE_MISMATCH;
		return false;
	}
	int retrievedData = sqlite3_column_int(dbDetails.dbStatement, column);
	if (retrievedData < 0) {
		errorMessage = "Column at index (" + ToString(column) +
					   ") does not contain an UNSIGNED.";
		errorCode = SQLITE_MISMATCH;
		return false;
	}
	data = static_cast<unsigned int>(retrievedData);
	return true;
}

bool SQLiteWrapper::getDataDouble(const int &column, double &data)
{
	// Check if the given column is inside the boundaries.
	if ((column < 0) || (column > num_col)) {
		errorMessage = "Column index (" + ToString(column) +
					   ") is outside the boundaries.";
		errorCode = SQLITE_CONSTRAINT;
		return false;
	}
	// Check if the retrieved data is an integer.
	if (sqlite3_column_type(dbDetails.dbStatement, column) != SQLITE_FLOAT) {
		if (sqlite3_column_type(dbDetails.dbStatement, column) !=
			SQLITE_INTEGER) {
			errorMessage = "Column at index (" + ToString(column) +
						   ") does not contain a Double.";
			errorCode = SQLITE_MISMATCH;
			return false;
		}
	}
	data = sqlite3_column_double(dbDetails.dbStatement, column);
	return true;
}

std::string SQLiteWrapper::getNextString()
{
	std::string data;
	if (this->getDataString(currentColumn, data)) {
		// Increase the column index.
		currentColumn++;
		return data;
	}
	throw SQLiteException(errorCode, errorMessage);
}

int SQLiteWrapper::getNextInteger()
{
	int data;
	if (this->getDataInteger(currentColumn, data)) {
		// Increase the column index.
		currentColumn++;
		return data;
	}
	throw SQLiteException(errorCode, errorMessage);
}

unsigned int SQLiteWrapper::getNextUnsignedInteger()
{
	unsigned int data;
	if (this->getDataUnsignedInteger(currentColumn, data)) {
		// Increase the column index.
		currentColumn++;
		return data;
	}
	throw SQLiteException(errorCode, errorMessage);
}

double SQLiteWrapper::getNextDouble()
{
	double data;
	if (this->getDataDouble(currentColumn, data)) {
		// Increase the column index.
		currentColumn++;
		return data;
	}
	throw SQLiteException(errorCode, errorMessage);
}

int SQLiteWrapper::loadOrSaveDb(bool save)
{
	// Prepare the path to the database.
	auto dbPath = dbDetails.dbDirectory + dbDetails.dbName;
	// Function return code.
	int rc;
	// Database connection opened on dbPath.
	sqlite3 *dbOnFile;
	// Backup object used to copy data.
	sqlite3_backup *dbBackup;
	// Database to copy to.
	sqlite3 *dbTo;
	// Database to copy from.
	sqlite3 *dbFrom;
	// Open the database file identified by dbPath.
	// Exit early if this fails for any reason.
	rc = sqlite3_open(dbPath.c_str(), &dbOnFile);
	if (rc == SQLITE_OK) {
		// If this is a 'load' operation (save == false), then data is copied
		// from the on-file database just opened to the in-memory database.
		// Otherwise, if this is a 'save' operation (save == true), then data is
		// copied from the in-memory database to the on-file database.
		// Set the variables dbFrom and dbTo accordingly.
		dbFrom = (save ? dbDetails.dbConnection : dbOnFile);
		dbTo = (save ? dbOnFile : dbDetails.dbConnection);
		// Set up the backup procedure to copy from the "main" of the dbFrom
		// connection to the main of the dbTo connection.
		// If something goes wrong, dbBackup will be set to NULL and an error
		// code and message left in connection dbTo.
		// If the backup object is successfully created, call backup_step()
		// to copy data from dbFrom to dbTo.
		// Then call backup_finish() to release resources associated with
		// the backup object.
		// If an error occurred, then an error code and message will be left in
		// connection dbTo. If no error occurred, then the error code belonging
		// to dbTo is set to SQLITE_OK.
		dbBackup = sqlite3_backup_init(dbTo, "main", dbFrom, "main");
		if (dbBackup) {
			sqlite3_backup_step(dbBackup, -1);
			sqlite3_backup_finish(dbBackup);
		}
		rc = sqlite3_errcode(dbTo);
	}
	// Close the database connection opened on database file and return the
	// result of this function.
	sqlite3_close(dbOnFile);
	return rc;
}
