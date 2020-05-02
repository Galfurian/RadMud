/// @file   logger.cpp
/// @brief  Define the logging functions.
/// @author Enrico Fraccaroli
/// @date   May 28 2016
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

#include "utilities/logger.hpp"
#include "utilities/utils.hpp"

#include <mutex>
#include <fstream>
#include <cstring>

#define FILENAME(FILE) (strrchr((FILE), '/') ? strrchr((FILE), '/') + 1 : (FILE))

/// Output file.
static std::fstream log_file;
/// Define a mutex for the log function.
static std::mutex log_mutex;

static inline std::string LogGetDateTime()
{
	time_t now = time(nullptr);
	char buffer[32];
	// Format: H:M
	strftime(buffer, 32, "%H:%M:%S", localtime(&now));
	return std::string(buffer);
}

static inline std::string LogLevelToString(const LogLevel &level)
{
	if (level == LogLevel::Global)
		return " GLOBAL";
	if (level == LogLevel::Debug)
		return "  DEBUG";
	if (level == LogLevel::Info)
		return "   INFO";
	if (level == LogLevel::Warning)
		return "WARNING";
	if (level == LogLevel::Error)
		return "  ERROR";
	if (level == LogLevel::Fatal)
		return "  FATAL";
	if (level == LogLevel::Trace)
		return "  TRACE";
	return "UNKNOWN";
}

bool OpenLog(const std::string &filename)
{
	if (!log_file.is_open())
		log_file.open(filename.c_str(), std::ios::out | std::ios::app);
	return log_file.is_open();
}

void _mudlog(const char *file, int line, const LogLevel &level, const std::string &msg)
{
	std::lock_guard<std::mutex> lock(log_mutex);
	if (log_file.is_open()) {
		// Write the log message inside the file.
		log_file << '[' << LogGetDateTime() << ']';
		log_file << '[' << LogLevelToString(level) << ']';
		log_file << '[' << FILENAME(file) << ':' << line << "] ";
		log_file << msg << "\n" << std::flush;
	}
	std::cout << '[' << LogGetDateTime() << ']';
	std::cout << '[' << LogLevelToString(level) << "] ";
	std::cout << '[' << FILENAME(file) << ':' << line << "] ";
	std::cout << msg << "\n" << std::flush;
}
