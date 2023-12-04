/// @file   utils.cpp
/// @brief  Implements the methods used to manipulate objects.
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

// Basic Include.

#include "utilities/utils.hpp"

#include <dirent.h>
#include <ctime>
#include <fstream>
#include <iterator>

#include "utilities/logger.hpp"

bool DoubleEquality(double a, double b)
{
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

double SafeLog10(const double & source)
{
    if (source > 0)
    {
        return log10(source);
    }
    return 0;
}

bool BeginWith(const std::string & source, const std::string & prefix)
{
    return source.compare(0, prefix.size(), prefix) == 0;
}

bool EndWith(const std::string & source, const std::string & suffix)
{
    return (source.size() >= suffix.size()) &&
           (source.compare(source.size() - suffix.size(),
                           suffix.size(),
                           suffix) == 0);
}

void FindAndReplace(std::string * source,
                    const std::string & target,
                    const std::string & replacement)
{
    size_t start_pos = 0;
    while ((start_pos = source->find(target, start_pos)) != std::string::npos)
    {
        source->replace(start_pos, target.length(), replacement);
        start_pos += replacement.length();
    }
}

std::string Trim(const std::string & source, const std::string & trim)
{
    std::string working = source;
    std::string::size_type i = working.find_last_not_of(trim);
    if (i == std::string::npos)
    {
        return "";
    }
    return working.erase(i + 1).erase(0, source.find_first_not_of(trim));
}

std::string ToLower(const std::string & source)
{
    std::string working = source;
    // Put all the letter to lower case.
    for (unsigned int i = 0; i < working.length(); i++)
    {
        working[i] = static_cast<char>(tolower(working[i]));
    }
    return working;
}

std::string ToCapitals(const std::string & source)
{
    std::string working = source;
    // First of all put all the letter to lower case.
    for (auto & c : working)
    {
        c = static_cast<char>(tolower(c));
    }
    // Capitalize the first letter.
    working[0] = static_cast<char>(toupper(working[0]));
    bool previousWasSpace = false;
    // Capitalize the rest of the std::string.
    for (auto & c : working)
    {
        //prints next char providing not a space
        if (c == ' ')
        {
            previousWasSpace = true;
            continue;
        }
        else if (previousWasSpace)
        {
            // Capitalize letter after space.
            c = static_cast<char>(toupper(c));
            // Reset the flag.
            previousWasSpace = false;
        }
    }
    return working;
}

std::vector<std::string> SplitString(const std::string & source,
                                     const std::string & delimiter)
{
    std::vector<std::string> result;
    size_t pos = 0;
    std::string tmp = source;
    while ((pos = tmp.find(delimiter)) != std::string::npos)
    {
        result.push_back(tmp.substr(0, pos));
        tmp.erase(0, pos + delimiter.length());
    }
    if (!tmp.empty())
    {
        result.push_back(tmp);
    }
    return result;
}

std::vector<std::string> GetWords(const std::string & source)
{
    std::istringstream iss(source);
    std::istream_iterator<std::string> begin(iss);
    std::istream_iterator<std::string> end;

    std::vector<std::string> working(begin, end);
    return working;
}

std::string GetFormattedTime()
{
    time_t now = time(nullptr);
    char buffer[32];
    // Format: H:M
    strftime(buffer, 32, "%H:%M", localtime(&now));
    return std::string(buffer);
}

std::string GetDate()
{
    time_t now = time(nullptr);
    tm * ptm = localtime(&now);
    char buffer[32];

    // Format: %Y_%m_%d_%H-%M
    strftime(buffer, 32, "%Y_%m_%d", ptm);
    return std::string(buffer);
}

std::vector<std::string> GetAllFilesInFolder(
    const std::string & folder,
    const std::string & extension)
{
    DIR * directory;
    std::vector<std::string> files_name;
    directory = opendir(folder.c_str());
    if (directory != nullptr)
    {
        int i = 0;
        struct dirent * dir;
        while ((dir = readdir(directory)) != nullptr)
        {
            i++;
            if (EndWith(dir->d_name, extension))
            {
                files_name.emplace_back(dir->d_name);
            }
        }
        closedir(directory);
    }
    return files_name;
}

bool IsNumber(const std::string & source)
{
    for (auto c : source)
    {
        if (isdigit(c) == 0)
        {
            return false;
        }
    }
    return true;
}

bool GetFileContents(const char * filename, std::string & contents)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in.good())
    {
        in.seekg(0, std::ios::end);
        std::streamoff totalSize = in.tellg();
        if (totalSize > 0)
        {
            contents.resize(static_cast<std::size_t>(totalSize));
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], totalSize);
            in.close();
        }
        return true;
    }
    Logger::log(LogLevel::Error, "Can't open :" + std::string(filename));
    return false;
}

/// Check if the return code from Zlib is an error.
#define ZCHECK_ERROR(err, msg)\
if ((err) != Z_OK) {\
    std::cerr << #msg" error: "#err"\n";\
    exit(1);\
}\

