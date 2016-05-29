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
#include "utils.hpp"

#include <ctime>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <fstream>
#include <random>
#include <iomanip>
#include <iterator>
#include <cstring>
#include <errno.h>
#include <string>
#include <limits.h>

// Other Include.
#include "constants.hpp"
#include "logger.hpp"

bool BeginWith(const std::string & source, const std::string & target)
{
    return !source.compare(0, target.size(), target);
}

bool EndWith(const std::string & source, const std::string & target)
{
    if (source.size() > target.size())
    {
        return !source.compare(source.size() - target.size(), source.size(), target);
    }
    return false;
}

std::string FindAndReplace(std::string & source, const std::string & target, const std::string & replacement)
{
    if (target.empty())
    {
        return target;
    }
    size_t start_pos = 0;
    while ((start_pos = source.find(target, start_pos)) != std::string::npos)
    {
        source.replace(start_pos, target.length(), replacement);
        start_pos += replacement.length();
    }
    return source;
}

std::string Trim(const std::string & source, const std::string & trim)
{
    std::string working = source;
    std::string::size_type i = working.find_last_not_of(trim);
    if (i == std::string::npos)
    {
        return "";
    }
    else
    {
        return working.erase(i + 1).erase(0, source.find_first_not_of(trim));
    }
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
    for (unsigned int i = 0; i < working.length(); i++)
    {
        working[i] = static_cast<char>(tolower(working[i]));
    }
    // Capitalize the first letter.
    working[0] = static_cast<char>(toupper(working[0]));
    // Capitalize the rest of the std::string.
    for (unsigned int i = 1; i < working.length(); i++)
    {
        //prints next char providing not a space
        if (working[i] == ' ')
        {
            // Move to the next letter.
            i++;
            // Capitalize letter after space.
            working[i] = static_cast<char>(toupper(working[i]));
        }
    }
    return working;
}
std::vector<std::string> SplitString(const std::string & source, const std::string & delimiter)
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

std::vector<int> GetIntVect(const std::string & source)
{
    std::stringstream line(source);
    std::vector<int> output_vector;
    int buffer;
    while (line >> buffer)
    {
        output_vector.push_back(buffer);
    }
    return output_vector;
}

std::vector<Byte> GetByteVect(const std::string & source)
{
    std::stringstream line(source);
    std::vector<Byte> output_vector;
    int buffer;
    while (line >> buffer)
    {
        output_vector.push_back(static_cast<Byte>(buffer));
    }
    return output_vector;
}

std::string GetFormattedTime()
{
    time_t now = time(NULL);
    char buffer[32];
    // Format: H:M
    strftime(buffer, 32, "%H:%M", localtime(&now));
    return buffer;
}

std::string GetDate()
{
    time_t now = time(NULL);
    tm * ptm = localtime(&now);
    char buffer[32];

    // Format: %Y_%m_%d_%H-%M
    strftime(buffer, 32, "%Y_%m_%d", ptm);
    return buffer;
}

std::vector<std::string> GetAllFilesInFolder(const std::string & folder, const std::string & extension)
{
    DIR *directory;
    std::vector<std::string> files_name;
    directory = opendir(folder.c_str());
    if (directory)
    {
        int i = 0;
        struct dirent *dir;
        while ((dir = readdir(directory)) != NULL)
        {
            i++;
            if (EndWith(dir->d_name, extension))
            {
                files_name.push_back(dir->d_name);
            }
        }
        closedir(directory);
    }
    return files_name;
}

std::string BoolToString(const bool & value)
{
    return ((value) ? "Yes" : "No");
}

int ToInt(const std::string & source)
{
    char * pEnd;
    return static_cast<int>(strtol(source.c_str(), &pEnd, 10));
}

std::string AlignString(const std::string & source, const StringAlign & alignment, const size_t & width)
{
    unsigned int padding;
    // Create a string stream.
    std::ostringstream oss;
    // Align the string.
    switch (alignment)
    {
        case kAlignLeft:
            oss << ' ';
            // Set the width.
            oss << std::setw(static_cast<int>(width) - 1);
            // Set the alignment.
            oss << std::left;
            // Set the string.
            oss << source;
            break;
        case kAlignCenter:
            padding = static_cast<unsigned int>(width - source.size());
            for (unsigned int i = 0; i < (padding / 2); i++)
            {
                oss << ' ';
            }
            oss << source;
            for (unsigned int i = 0; i < (padding / 2); i++)
            {
                oss << ' ';
            }
            // if odd #, add 1 space
            if (padding > 0 && padding % 2 != 0)
            {
                oss << ' ';
            }
            break;
        case kAlignRight:
            // Set the width.
            oss << std::setw(static_cast<int>(width) - 1);
            // Set the alignment.
            oss << std::right;
            // Set the string.
            oss << source;
            oss << ' ';
            break;
    }
    return oss.str();
}

bool IsAllASCII(const char * string_to_check)
{
    for (; *string_to_check != 0; string_to_check++)
    {
        if (*string_to_check & 0x80)
        {
            return false;
        }
    }
    return true;
}

int RandInteger(const int & nMin, const int & nMax)
{
    std::random_device rng;
    std::uniform_int_distribution<int> uid(nMin, nMax);
    return uid(rng);
}

bool IsNumber(const std::string & source)
{
    for (auto c : source)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

int GetAbilityModifier(const int & value)
{
    int modifier = 0;
    if ((value - 10) > 0)
    {
        modifier = (value - 10) / 2;
    }
    return modifier;
}

void ExtractNumber(std::string & source, int & number)
{
    // If the entire string is a number, set it.
    if (!IsNumber(source))
    {
        // Otherwise try to find a number if there is one.
        size_t pos = source.find('.');
        if (pos != std::string::npos)
        {
            // Extract the digits.
            std::string digits = source.substr(0, pos);
            // Check the digits.
            if (IsNumber(digits))
            {
                // Get the number.
                long int value = atol(digits.c_str());
                if (value < INT_MAX)
                {
                    // Set the number.
                    number = static_cast<int>(value);
                }
                // Remove the digits.
                source = source.substr(pos + 1, source.size());
            }
        }
    }
}

std::string GetFileContents(const char * filename)
{
    std::string contents;
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
    }
    else
    {
        contents = "";
        Logger::log(LogLevel::Error, "Can't open :" + std::string(filename));
    }
    return (contents);
}

std::string GetAttributeName(const int & id, const bool & abbreviated)
{
    std::string output;
    switch (id)
    {
        case 1:
            output = (abbreviated) ? "Str" : "Strength";
            break;
        case 2:
            output = (abbreviated) ? "Agi" : "Agility";
            break;
        case 3:
            output = (abbreviated) ? "Per" : "Perception";
            break;
        case 4:
            output = (abbreviated) ? "Con" : "Constitution";
            break;
        case 5:
            output = (abbreviated) ? "Int" : "Intelligence";
            break;
        default:
            output = (abbreviated) ? "Err:" + ToString(id) : "Error:" + ToString(id);
            break;
    }
    return output;
}

/// Check if the return code from Zlib is an error.
#define ZCHECK_ERROR(err, msg)\
if (err != Z_OK) {\
    std::cerr << #msg" error: "#err"\n";\
    exit(1);\
}\

/// Creates compressed strem of data.
void DeflateStream(std::vector<uint8_t> & uncompressed, std::vector<uint8_t> & compressed)
{
    // ZLib compression stream.
    z_stream c_stream;
    // The error code returned from every call to zlib
    int errmsg;

    // Clear the input std::vector.
    compressed.clear();

    c_stream.zalloc = (alloc_func) 0;
    c_stream.zfree = (free_func) 0;
    c_stream.opaque = (voidpf) 0;

    errmsg = deflateInit(&c_stream, Z_BEST_COMPRESSION);
    ZCHECK_ERROR(errmsg, "deflateInit");

    c_stream.next_in = &uncompressed[0];
    c_stream.avail_in = static_cast<uint32_t>(uncompressed.size());

    while (true)
    {
        uint8_t c_buffer[10] =
        { };
        c_stream.next_out = &c_buffer[0];
        c_stream.avail_out = 10;

        errmsg = deflate(&c_stream, Z_FINISH);
        if (errmsg == Z_STREAM_END)
        {
            for (unsigned int i = 0; i < (10 - c_stream.avail_out); i++)
            {
                compressed.push_back(c_buffer[i]);
            }
            break;
        }
        ZCHECK_ERROR(errmsg, "deflate");
        for (unsigned int i = 0; i < (10 - c_stream.avail_out); i++)
        {
            compressed.push_back(c_buffer[i]);
        }
    }

    errmsg = deflateEnd(&c_stream);
    ZCHECK_ERROR(errmsg, "deflateReset");
}

/// Creates decompressed strem of data.
void InflateStream(std::vector<uint8_t> & compressed, std::vector<uint8_t> & uncompressed)
{
    // ZLib decompression stream.
    z_stream d_stream;
    // The error code returned from every call to zlib
    int errmsg;

    // Clear the input std::vector.
    uncompressed.clear();

    d_stream.zalloc = Z_NULL;
    d_stream.zfree = Z_NULL;
    d_stream.opaque = Z_NULL;
    d_stream.avail_in = 0;
    d_stream.next_in = Z_NULL;

    errmsg = inflateInit(&d_stream);
    ZCHECK_ERROR(errmsg, "inflateInit");

    d_stream.avail_in = static_cast<uint32_t>(compressed.size());
    d_stream.next_in = &compressed[0];

    do
    {
        uint8_t d_buffer[10] =
        { };
        d_stream.next_out = &d_buffer[0];
        d_stream.avail_out = 10;

        errmsg = inflate(&d_stream, Z_NO_FLUSH);
        if (errmsg == Z_STREAM_END)
        {
            for (unsigned int i = 0; i < (10 - d_stream.avail_out); i++)
            {
                uncompressed.push_back(d_buffer[i]);
            }
            if (d_stream.avail_in == 0)
            {
                break;
            }
        }
        else
        {
            ZCHECK_ERROR(errmsg, "inflate");
        }

        for (unsigned int i = 0; i < (10 - d_stream.avail_out); i++)
        {
            uncompressed.push_back(d_buffer[i]);
        }
    }
    while (d_stream.avail_out == 0);

    errmsg = inflateEnd(&d_stream);
    ZCHECK_ERROR(errmsg, "inflateEnd");
}
