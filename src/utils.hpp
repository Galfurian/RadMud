/// @file   utils.hpp
/// @brief  Define methods used to manipulate objects.
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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <zlib.h>
#include <type_traits>
#include <iostream>
#include <chrono>
#include <functional>
#include <cassert>
#include <sstream>
#include <type_traits>

/// 8 bits
typedef unsigned char Byte;
/// Type used by the mud to store time.
typedef std::chrono::nanoseconds TimeNS;
/// Time point in microseconds.
typedef std::chrono::time_point<std::chrono::microseconds> TimeUS;
/// Time point in milliseconds.
typedef std::chrono::time_point<std::chrono::milliseconds> TimeMS;
/// Time point in seconds.
typedef std::chrono::time_point<std::chrono::seconds> TimeSEC;
/// Clock type of time.
typedef std::chrono::time_point<std::chrono::system_clock> TimeClock;

/// Allows to define a non-aborting assertion.
#define SafeAssert(e) ( (e) ? true : (\
    std::cerr << "Assertion :"#e<<"\n",\
    std::cerr << "File      :"<<std::string(__FILE__)<<"\n",\
    std::cerr << "Line      :"<<ToString(__LINE__)<<"\n",\
    false))

/// @struct CommandHelp
/// @brief  Contains the string for showing a command help.
struct CommandHelp
{
    public:
        /// The name of the command.
        std::string command;
        /// The arguemtns of the command.
        std::string arguments;
        /// The help message of the command.
        std::string help;
};

/// @brief Check if the passed value has the given flag set.
/// @param flags The value to check.
/// @param flag  The flag to search.
/// @return <b>True</b> if the value contain the flag, <br> <b>False</b> otherwise.
template<typename Enum>
bool HasFlag(unsigned int & flags, Enum flag)
{
    static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type");
    return (flags & static_cast<unsigned int>(flag)) != 0;
}

/// @brief Set the passed flag to the value.
/// @param flags The destination value.
/// @param flag  The flag to set.
template<typename Enum>
void SetFlag(unsigned int & flags, Enum flag)
{
    static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type");
    flags |= static_cast<unsigned int>(flag);
}

/// @brief Clear the passed flag to the value.
/// @param flags The destination value.
/// @param flag  The flag to clear.
template<typename Enum>
void ClearFlag(unsigned int & flags, Enum flag)
{
    static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type");
    flags &= ~static_cast<unsigned int>(flag);
}

/// @brief Check if the source string begin with a given prefix.
/// @param source Source string.
/// @param target Prefix string.
/// @return <b>True</b> if the source begin with the given prefix, <br> <b>False</b> otherwise.
bool BeginWith(const std::string & source, const std::string & target);

/// @brief Check if the source string end with a given string.
/// @param source Source string.
/// @param target Final string.
/// @return <b>True</b> if the source end with the given postfix, <b>False</b> otherwise.
bool EndWith(const std::string & source, const std::string & target);

/// @brief Find and replace in a string.
/// @param source      Source string.
/// @param target      Target sub-string.
/// @param replacement Replacement for the sub-string.
/// @return String with sub-string replaced.
std::string FindAndReplace(std::string & source, const std::string & target, const std::string & replacement);

/// @brief Get rid of leading and trailing spaces from a string
/// @param source Source string.
/// @param trim   Trim condition.
/// @return String modified.
std::string Trim(const std::string & source, const std::string & trim = " \t\r\n");

/// @brief Convert all the stirng to lowercase.
/// @param source Source string.
/// @return String modified.
std::string ToLower(const std::string & source);

/// @brief Capitalise the first letters of all the words in the string.
/// @param source Source string.
/// @return String modified.
std::string ToCapitals(const std::string & source);

/// @brief Given a string and a delimiter, the string is splitted using the delimiter.
/// @param source    The source string.
/// @param delimiter The delimiter which has to be used.
/// @return The splitted string.
std::vector<std::string> SplitString(const std::string & source, const std::string & delimiter);

template<typename T, typename std::enable_if<!std::is_integral<T>::value>::type>
std::vector<T> SplitString(const std::string & source, const std::string & delimiter)
{
    std::vector<T> result;
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

template<typename T, typename std::enable_if<std::is_integral<T>::value>::type>
std::vector<T> SplitString(const std::string & source, const std::string & delimiter)
{
    std::vector<T> result;
    size_t pos = 0;
    std::string tmp = source;
    while ((pos = tmp.find(delimiter)) != std::string::npos)
    {
        char * pEnd;
        result.push_back(static_cast<T>(strtol(tmp.substr(0, pos).c_str(), &pEnd, 10)));
        tmp.erase(0, pos + delimiter.length());
    }
    if (!tmp.empty())
    {
        result.push_back(tmp);
    }
    return result;
}

/// @brief Get all the words in the source string.
/// @param source The source string.
/// @return A vector containing the words of the source string.
std::vector<std::string> GetWords(const std::string & source);

/// @brief Get all the integers in the source string.
/// @param source The source string.
/// @return A vector containing the integers of the source string.
std::vector<int> GetIntVect(const std::string & source);

/// @brief Get all the integers in the source string.
/// @param source The source string.
/// @return A vector containing the integers of the source string.
std::vector<Byte> GetByteVect(const std::string & source);

template<typename ValueType>
std::vector<ValueType> GetNumberVect(const std::string & source)
{
    static_assert((
            std::is_same<ValueType, bool>::value ||
            std::is_same<ValueType, int>::value ||
            std::is_same<ValueType, long int>::value ||
            std::is_same<ValueType, unsigned int>::value ||
            std::is_same<ValueType, long unsigned int>::value ||
            std::is_same<ValueType,double>::value), "template parameter is of the wrong type");
    std::stringstream line(source);
    std::vector<ValueType> output_vector;
    ValueType buffer;
    while (line >> buffer)
    {
        output_vector.push_back(buffer);
    }
    return output_vector;
}

/// @brief Return the current timestamp as "Hours:Minute".
/// @return The current timestamp.
std::string GetFormattedTime();

/// @brief Return the current date.
/// @return The current date.
std::string GetDate();

/// @brief Return a vector containing all the files having the given extension in a folder.
/// @param folder    The folder where to look for files.
/// @param extension Needed files extension.
/// @return The list of the files.
std::vector<std::string> GetAllFilesInFolder(const std::string & folder, const std::string & extension);

/// @brief Transform a boolean into Yes or No.
/// @param value The value to turn into string.
/// @return The value turned into string.
std::string BoolToString(const bool & value);

/// @brief Transform a string into an integer.
/// @param source The string to turn into an integer.
/// @return The resulting integer.
int ToInt(const std::string & source);

/// @brief Transform a string into a numeric value.
/// @param source The string to turn into a number.
/// @return The number.
template<typename ValueType>
ValueType ToNumber(const std::string & source)
{
    static_assert((
            std::is_same<ValueType, bool>::value ||
            std::is_same<ValueType, int>::value ||
            std::is_same<ValueType, long int>::value ||
            std::is_same<ValueType, unsigned int>::value ||
            std::is_same<ValueType, long unsigned int>::value ||
            std::is_same<ValueType,double>::value), "template parameter is of the wrong type");
    char * pEnd;
    return static_cast<ValueType>(strtol(source.c_str(), &pEnd, 10));
}

/// @brief Transform a numeric value into a string.
/// @param value The value to turn into a string.
/// @return The resulting string.
template<typename ValueType>
std::string ToString(const ValueType & value)
{
    static_assert((
            std::is_same<ValueType, bool>::value ||
            std::is_same<ValueType, int>::value ||
            std::is_same<ValueType, long int>::value ||
            std::is_same<ValueType, unsigned int>::value ||
            std::is_same<ValueType, long unsigned int>::value ||
            std::is_same<ValueType,double>::value), "template parameter is of the wrong type");
    std::ostringstream stm;
    stm << value;
    return stm.str();
}

/// @brief Transform a numeric value into a string.
/// @param value The value to turn into a string.
/// @return The resulting string.
template<typename Enum>
std::string EnumToString(const Enum & value)
{
    static_assert(( std::is_enum<Enum>::value), "template parameter is not an enum type");
    std::ostringstream stm;
    stm << static_cast<unsigned int>(value);
    return stm.str();
}

/// Kinds of string alignment.
typedef enum StringAlignments
{
    /// Left alignment.
    kAlignLeft,
    /// Center alignment.
    kAlignCenter,
    /// Right alignment.
    kAlignRight,
} StringAlign;

/// @brief Align the given string.
/// @param source    The source string.
/// @param alignment The kind of alignment.
/// @param width     The total width of the string.
/// @return The aligned string.
std::string AlignString(const std::string & source, const StringAlign & alignment, const size_t & width);

/// @brief Check if all the character in the string it's ASCII.
/// @param string_to_check The string to check.
/// @return <b>True</b> if the string it's made of all ASCII characters.<br><b>False</b> otherwise.
bool IsAllASCII(const char *string_to_check);

/// @brief Generate a random integer between the defined range.
/// @param nMin Min value.
/// @param nMax Max value.
/// @return The generated integer.
int RandInteger(const int & nMin, const int & nMax);

/// @brief Transform a numeric value into a string.
/// @param value The value to turn into a string.
/// @return The resulting string.
template<typename ValueType>
ValueType TRandInteger(const ValueType & lower_bound, const ValueType & upper_bound)
{
    static_assert((
            std::is_same<ValueType, bool>::value ||
            std::is_same<ValueType, int>::value ||
            std::is_same<ValueType, long int>::value ||
            std::is_same<ValueType, unsigned int>::value ||
            std::is_same<ValueType, long unsigned int>::value ||
            std::is_same<ValueType,double>::value), "template parameter is of the wrong type");

    std::random_device rng;
    std::uniform_int_distribution<ValueType> uid(lower_bound, upper_bound);
    return uid(rng);
}

/// @brief Check if the string is a number.
/// @param source The string to check.
/// @return <b>True</b> if the string it's a number, <b>False</b> otherwise.
bool IsNumber(const std::string & source);

/// @brief Return the modifier of the given ability.
/// @param value The total ability value.
/// @return The ability modifier.
unsigned int GetAbilityModifier(const unsigned int & value);

/// @brief Given a player target, eg.: 2.rat.<br>Extract the number <b>2</b>.
/// @param source The source string.
/// @param number Pointer where must put the number once retrieved.
void ExtractNumber(std::string & source, int & number);

/// @brief  Retrieve the content of a file.
/// @param  filename The path and name of the file to read.
/// @return The content of the file.
std::string GetFileContents(const char * filename);

/// @brief  Return the string of the attribute.
/// @param  id          The id of the attribute.
/// @param  abbreviated If <b>true</b> abbreviate the name of the attribute.
/// @return The name of the attribute.
std::string GetAttributeName(const int & id, const bool & abbreviated = false);

/// @brief It creates a compressed strem of data.
/// @param uncompressed The input non-compressed stream of data.
/// @param compressed   The resulting compressed stream of data.
void DeflateStream(std::vector<uint8_t> & uncompressed, std::vector<uint8_t> & compressed);

/// @brief It decompress a compressed strem of data.
/// @param compressed   The input compressed stream of data.
/// @param uncompressed The resulting non-compressed stream of data.
void InflateStream(std::vector<uint8_t> & compressed, std::vector<uint8_t> & uncompressed);

/// @brief Remove an element from a vector.
/// @param v    The vector.
/// @param item The element to remove.
/// @return <b>True</b> if the remove is successful,<br>
///         <b>False</b> otherwise.
/// Complexity: <b>Linear</b>.<br>
/// Proportional to distance(target)s.
template<typename T>
bool FindErase(std::vector<T> & v, const T & item)
{
    auto iterator = std::find(v.begin(), v.end(), item);
    if (iterator != v.end())
    {
        v.erase(iterator);
        v.shrink_to_fit();
        return true;
    }
    else
    {
        return false;
    }
}

/// @brief Remove an element from a list.
/// @param l    The list.
/// @param item The element to remove.
/// @return <b>True</b> if the remove is successful,<br>
///         <b>False</b> otherwise.
/// Complexity: <b>Linear</b>.<br>
/// Proportional to distance(target)s.
template<typename T>
typename std::list<T>::iterator FindErase(std::list<T> & l, const T & item)
{
    auto iterator = std::find(l.begin(), l.end(), item);
    if (iterator != l.end())
    {
        iterator = l.erase(iterator, l.end());
    }
    return iterator;
}

/// @brief Remove an element from a set.
/// @param set  The set.
/// @param item The element to remove.
/// @return <b>True</b> if the remove is successful,<br><b>False</b> otherwise.
/// Complexity: <b>Linear</b>.<br>
/// Proportional to distance(target)s.
template<typename T>
typename std::set<T>::iterator FindErase(std::set<T> & set, const T & item)
{
    auto it = set.find(item);
    if (it != set.end())
    {
        set.erase(it);
    }
    return it;
}

/// @brief Remove an element from a map.
/// @param map The target map.
/// @param key The key to remove.
/// @return <b>True</b> if the remove is successful,<br><b>False</b> otherwise.
/// Complexity: <b>Log(n)</b>.<br>
/// Where n is the number of members in the map.
template<typename K, typename T>
inline typename std::map<K, T>::iterator FindErase(std::map<K, T> & map, const K & key)
{
    auto it = map.find(key);
    if (it != map.end())
    {
        map.erase(it);
    }
    return it;
}

/// @brief Class which allows to gather timings.
template<typename TimeT, typename ClockT = std::chrono::high_resolution_clock, typename DurationT = double>
class Stopwatch
{
    private:
        /// The header used during the printing of the elapsed time.
        std::string header;
        /// The starting time.
        std::chrono::time_point<ClockT> timeStart;
        /// The ending time.
        std::chrono::time_point<ClockT> timeEnd;
    public:
        /// @brief Constructor.
        /// @param _header The header of the stopwatch.
        Stopwatch(std::string _header) :
            header(_header),
            timeStart(ClockT::now()),
            timeEnd(ClockT::now())
        {
            // Nothing to do.
        }
        /// @brief Starts the stopwatch.
        void start()
        {
            timeStart = timeEnd = ClockT::now();
        }
        /// @brief Stops the stopwatch.
        /// @return The elapsed time.
        DurationT stop()
        {
            timeEnd = ClockT::now();
            return elapsed();
        }
        /// @brief Restarts the stopwatch and provides the elapsed time.
        /// @return The elapsed time.
        DurationT restart()
        {
            DurationT ret = stop();
            start();
            return ret;
        }

        /// @brief Provides the current elapsed time.
        /// @return The elapsed time.
        DurationT elapsed() const
        {
            auto delta = std::chrono::duration_cast<TimeT>(timeEnd - timeStart);
            return static_cast<DurationT>(delta.count());
        }

        /// @brief Print the current elapsed time.
        void print() const
        {
            std::cout << header << " : " << elapsed() << std::endl;
        }
};
#endif
