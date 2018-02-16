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

#include <iostream>
#include <fstream>

#include "stringBuilder.hpp"

/// @brief Enumerator which identifies the severity of the log entry.
enum class LogLevel
{
    Global = 1,     ///< General logging level.
    Trace = 2,      ///< Log events useful for back-tracing.
    Debug = 4,      ///< Log events useful for developers.
    Fatal = 8,      ///< Log events which may lead to errors.
    Error = 16,     ///< Log events which concerns errors.
    Warning = 32,   ///< Log events which can harm the stability of the mud.
    Info = 64       ///< Log of events which involves the action of characters.
};

/// @brief Class used to handle mud logging.
class Logger
{
private:
    /// @brief Constructor.
    Logger();

    /// @brief Destructor.
    ~Logger();

public:
    /// @brief Disable Copy Construct.
    Logger(Logger const &) = delete;

    /// @brief Disable Move construct.
    Logger(Logger &&) = delete;

    /// @brief Disable Copy assign.
    Logger & operator=(Logger const &) = delete;

    /// @brief Disable Move assign.
    Logger & operator=(Logger &&) = delete;

    /// @brief Get the singleton istance of the Logger.
    /// @return The static and uniquie Logger variable.
    static Logger & instance();

    /// @brief Opens the logging stream.
    /// @param filename The filename.
    /// @return <b>True</b> if the stream has been opened,<br>
    ///         <b>False</b> otherwise.
    static bool openLog(const std::string & filename);

    /// @brief Retrieve the log and select only the line of the given logging level.
    /// @param level  The category of the message.
    /// @param result For efficiency, receive a reference to the result.
    /// @return <b>True</b> if the log has been retrieved,<br>
    ///         <b>False</b> otherwise.
    static bool getLog(const LogLevel & level, std::string * result);

    /// @brief Casts an unsigned int to a possible logging level.
    /// @param level  The logging level.
    /// @return The enum which identifies the given logging level.
    static LogLevel castFromInt(const unsigned int & level);

    /// @brief Print to consol and to logging file the given string.
    /// @param level  The category of the message.
    /// @param msg    The message to log.
    static void log(const LogLevel & level, const std::string & msg);

    /// @brief Print to consol and to logging file the given string.
    /// @param level The category of the message.
    /// @param msg   The message to log.
    /// @param args  Packed arguments.
    template<typename ... Args>
    static void log(const LogLevel & level,
                    const std::string & msg,
                    const Args & ... args)
    {
        log(level, StringBuilder::build(msg, args ...));
    }

private:
    /// @brief Provide the logging stream.
    /// @return A static std::fstream, which initially <b>is not opened</b>.
    static std::fstream & getStream();

    /// @brief Return the current timestamp as "Hours:Minute".
    /// @return The current timestamp.
    static std::string getDateTime();

    /// @brief Given a logging level returns the associated output stream.
    /// @param level The level of the output stream.
    /// @return The output stream.
    static std::ostream & getOutputStream(const LogLevel & level);

    /// @brief Converts a logging level to the curresponding string.
    /// @param level The level to transform into string.
    /// @return Logging level string.
    static std::string levelToString(const LogLevel & level);
};
