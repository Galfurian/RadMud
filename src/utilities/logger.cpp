/// @file   logger.cpp
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

#include "logger.hpp"
#include "../utils.hpp"

Logger::Logger()
{
    // Nothing to do.
}

Logger::~Logger()
{
    // Nothing to do.
}

Logger & Logger::instance()
{
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static Logger instance;
    // Return a reference to our instance.
    return instance;
}

bool Logger::openLog(const std::string & filename)
{
    if (!Logger::getStream().is_open())
    {
        Logger::getStream().open(filename.c_str(), std::ios::in | std::ios::out | std::ios::app);
    }
    return Logger::getStream().is_open();
}

bool Logger::getLog(const LogLevel & level, std::string * result)
{
    if (Logger::getStream().is_open())
    {
        // Clear the output string.
        result->clear();
        std::streamoff totalSize = Logger::getStream().tellg();
        if (totalSize > 0)
        {
            // Resize the log string.
            result->resize(static_cast<std::size_t>(totalSize));
            // Move the input position to the beginning of the string.
            Logger::getStream().seekg(0, std::ios::beg);
            // Create a string for storing the line.
            std::string line;
            // Create a string which contains the given level.
            std::string logLevel = "[" + Logger::levelToString(level) + "]";
            while (std::getline(Logger::getStream(), line) != nullptr)
            {
                // If the line starts with the sa
                if (BeginWith(line, logLevel))
                {
                    result->append(line + "\n");
                }
            }
            return true;
        }
    }
    return false;
}

LogLevel Logger::castFromInt(const unsigned int & level)
{
    return static_cast<LogLevel>(level);
}

void Logger::log(const LogLevel & level, const std::string & msg)
{
    std::string output("[" + Logger::levelToString(level) + "][" + Logger::getDateTime() + "] " + msg + "\n");
    if (Logger::getStream().is_open())
    {
        // Write the log message inside the file.
        Logger::getStream() << output;
    }
    Logger::getOutputStream(level) << output;
}

std::fstream & Logger::getStream()
{
    static std::fstream out_stream;
    return out_stream;
}

std::string Logger::getDateTime()
{
    time_t now = time(nullptr);
    char buffer[32];
    // Format: H:M
    strftime(buffer, 32, "%H:%M", localtime(&now));
    return buffer;
}

std::ostream & Logger::getOutputStream(const LogLevel & level)
{
    if (level == LogLevel::Global)
    {
        return std::cout;
    }
    if (level == LogLevel::Debug)
    {
        return std::cout;
    }
    if (level == LogLevel::Info)
    {
        return std::cout;
    }
    if (level == LogLevel::Warning)
    {
        return std::cerr;
    }
    if (level == LogLevel::Error)
    {
        return std::cerr;
    }
    if (level == LogLevel::Fatal)
    {
        return std::cout;
    }
    if (level == LogLevel::Trace)
    {
        return std::cout;
    }
    return std::cout;
}

std::string Logger::levelToString(const LogLevel & level)
{
    if (level == LogLevel::Global)
    {
        return " GLOBAL";
    }
    if (level == LogLevel::Debug)
    {
        return "  DEBUG";
    }
    if (level == LogLevel::Info)
    {
        return "   INFO";
    }
    if (level == LogLevel::Warning)
    {
        return "WARNING";
    }
    if (level == LogLevel::Error)
    {
        return "  ERROR";
    }
    if (level == LogLevel::Fatal)
    {
        return "  FATAL";
    }
    if (level == LogLevel::Trace)
    {
        return "  TRACE";
    }
    return "UNKNOWN";
}
