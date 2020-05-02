/// @file   logger.hpp
/// @brief  Define the logger class.
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

#pragma once

#include "tinyformat.hpp"

/// @brief Enumerator which identifies the severity of the log entry.
enum class LogLevel {
	Global = 1, ///< General logging level.
	Trace = 2, ///< Log events useful for back-tracing.
	Debug = 4, ///< Log events useful for developers.
	Fatal = 8, ///< Log events which may lead to errors.
	Error = 16, ///< Log events which concerns errors.
	Warning = 32, ///< Log events which can harm the stability of the mud.
	Info = 64 ///< Log of events which involves the action of characters.
};

bool OpenLog(const std::string &filename);

/// @brief Print to console and to logging file the given string.
/// @param level  The category of the message.
/// @param msg    The message to log.
void _mudlog(const char *file, int line, const LogLevel &level, const std::string &msg);

/// @brief Print to consol and to logging file the given string.
/// @param level The category of the message.
/// @param msg   The message to log.
/// @param args  Packed arguments.
template <typename... Args>
void _mudlog(const char *file, int line, const LogLevel &level, std::string const &fmt,
			 const Args &... args)
{
	std::stringstream ss;
	tfm::format(ss, fmt.c_str(), args...);
	_mudlog(file, line, level, ss.str());
}

#define MudLog(level, ...) _mudlog(__FILE__, __LINE__, level, __VA_ARGS__)
