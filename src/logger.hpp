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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>

/// @brief Enumerator which identifies the severity of the log entry.
typedef enum class LoggingLevels
{
    /// General logging level.
    Global = 1,
    /// Log events useful for back-tracing.
    Trace = 2,
    /// Log events useful for developers.
    Debug = 4,
    /// Log events which may lead to errors.
    Fatal = 8,
    /// Log events which concerns errors.
    Error = 16,
    /// Log events which can harm the stability of the mud.
    Warning = 32,
    /// Log of events which involves the action of characters.
    Info = 64
} LogLevel;

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

        /// @brief Print to consol and to logging file the gievn string.
        /// @param level  The category of the message.
        /// @param log    The message to log.
        static void log(const LogLevel & level, const std::string & log);

        /// @brief Retrieve the log and select only the line of the given logging level.
        /// @param level  The category of the message.
        /// @param result For efficiency, receive a reference to the result.
        /// @return <b>True</b> if the log has been retrieved,<br>
        ///         <b>False</b> otherwise.
        static bool getLog(const LogLevel & level, std::string & result);

        /// @brief Casts an unsigned int to a possible logging level.
        /// @param level  The logging level.
        /// @return The enum which identifies the given logging level.
        static LogLevel castFromInt(const unsigned int & level);

        static void tlog(
            LogLevel level,
            std::string log)
        {
            std::string output("[" + Logger::levelToString(level) + "][" + Logger::getDateTime() + "] " + log + "\n");
            if (Logger::getStream().is_open())
            {
                // Write the log message inside the file.
                Logger::getStream() << output;
            }
            Logger::getOutputStream(level) << output;
        }
        template<typename T, typename ... Args>
        static void tlog(
            LogLevel level,
            std::string log,
            T first,
            Args ... args)
        {
            for (size_t pos = 0; pos < log.size(); ++pos)
            {
                if (log.at(pos) == '%')
                {
                    std::cout << log << "\n";
                    log.replace(pos, pos + 1, first);
                    std::cout << log << "\n";
                    // Call recursively.
                    tlog(level, log, args...);
                    break;
                }
            }
        }

    private:
        /// @brief Provide the logging stream.
        /// @return A static std::fstream, which initially <b>is not opened</b>.
        static std::fstream & getStream();

        /// @brief Return the current timestamp as "Hours:Minute".
        /// @return The current timestamp.
        static std::string getDateTime();

        /// @brief Given a logging level returns the associated output stream.
        /// @return The output stream.
        static std::ostream & getOutputStream(const LogLevel & level);

        /// @brief Converts a logging level to the curresponding string.
        /// @param level The level to transform into string.
        /// @return Logging level string.
        static std::string levelToString(const LogLevel & level);
};

#endif /* SRC_LOGGER_HPP_ */
