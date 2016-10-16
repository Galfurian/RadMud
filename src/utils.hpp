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

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

/// Allows to define a non-aborting assertion for correct guards.
#define CorrectAssert(e) ( (e) ? true : (\
    std::cerr << "Assertion :"#e<<"\n",\
    std::cerr << "File      :"<<std::string(__FILE__)<<"\n",\
    std::cerr << "Line      :"<<ToString(__LINE__)<<"\n",\
    false))

/// Allows to define a non-aborting assertion for wrong guards.
#define WrongAssert(e) ( (e) ? (\
    std::cerr << "Assertion :"#e<<"\n",\
    std::cerr << "File      :"<<std::string(__FILE__)<<"\n",\
    std::cerr << "Line      :"<<ToString(__LINE__)<<"\n",\
    true) : false)

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

/// @brief Checks the equality between two doubles.
/// @param a The first value.
/// @param b The second value.
/// @return <b>True</b> if they are the same value, <br>
///         <b>False</b> otherwise.
bool DoubleEquality(double a, double b);

/// @brief Check if the passed value has the given flag set.
/// @param flags The value to check.
/// @param flag  The flag to search.
/// @return <b>True</b> if the value contain the flag, <br>
///         <b>False</b> otherwise.
template<typename Enum>
bool HasFlag(const unsigned int & flags, Enum flag)
{
    static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type");
    return (flags & static_cast<unsigned int>(flag)) != 0;
}

/// @brief Set the passed flag to the value.
/// @param flags The destination value.
/// @param flag  The flag to set.
template<typename Enum>
void SetFlag(unsigned int * flags, Enum flag)
{
    static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type");
    (*flags) |= static_cast<unsigned int>(flag);
}

/// @brief Clear the passed flag to the value.
/// @param flags The destination value.
/// @param flag  The flag to clear.
template<typename Enum>
void ClearFlag(unsigned int * flags, Enum flag)
{
    static_assert(std::is_enum<Enum>::value, "template parameter is not an enum type");
    (*flags) &= ~static_cast<unsigned int>(flag);
}

/// @brief Check if the source string begin with a given prefix.
/// @param source Source string.
/// @param prefix Prefix string.
/// @return <b>True</b> if the source begin with the given prefix, <br> <b>False</b> otherwise.
bool BeginWith(const std::string & source, const std::string & prefix);

/// @brief Check if the source string end with a given string.
/// @param source Source string.
/// @param suffix Final string.
/// @return <b>True</b> if the source end with the given postfix, <b>False</b> otherwise.
bool EndWith(const std::string & source, const std::string & suffix);

/// @brief Find and replace in a string.
/// @param source      Source string.
/// @param target      Target sub-string.
/// @param replacement Replacement for the sub-string.
void FindAndReplace(std::string * source, const std::string & target, const std::string & replacement);

/// @brief Get rid of leading and trailing spaces from a string
/// @param source Source string.
/// @param trim   Trim condition.
/// @return String modified.
std::string Trim(const std::string & source, const std::string & trim = std::string(" \t\r\n"));

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

/// @brief Get all the words in the source string.
/// @param source The source string.
/// @return A vector containing the words of the source string.
std::vector<std::string> GetWords(const std::string & source);

/// @brief Get all the integral values inside the source string.
/// @param source The source string.
/// @return A vector containing the retrieved integral values.
template<typename ValueType>
std::vector<ValueType> GetNumberVect(const std::string & source)
{
    static_assert((
                      std::is_same<ValueType, bool>::value ||
                      std::is_same<ValueType, int>::value ||
                      std::is_same<ValueType, long int>::value ||
                      std::is_same<ValueType, unsigned int>::value ||
                      std::is_same<ValueType, long unsigned int>::value ||
                      std::is_same<ValueType, double>::value), "template parameter is of the wrong type");
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

/// @brief Transform a string into a numeric value.
/// @param source The string to turn into a number.
/// @return The number.
template<typename ValueType>
ValueType ToNumber(const std::string & source)
{
    static_assert((
                      std::is_same<ValueType, bool>::value ||
                      std::is_same<ValueType, int>::value ||
                      std::is_same<ValueType, int64_t>::value ||
                      std::is_same<ValueType, unsigned int>::value ||
                      std::is_same<ValueType, uint64_t>::value ||
                      std::is_same<ValueType, double>::value), "template parameter is of the wrong type");
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
                      std::is_same<ValueType, int64_t>::value ||
                      std::is_same<ValueType, unsigned int>::value ||
                      std::is_same<ValueType, uint64_t>::value ||
                      std::is_same<ValueType, double>::value), "template parameter is of the wrong type");
    std::ostringstream stm;
    stm << value;
    return stm.str();
}

/// @brief Transform a numeric value into a string.
/// @param value The value to turn into a string.
/// @return The resulting string.
template<typename ValueType>
std::string EnumToString(const ValueType & value)
{
    static_assert((std::is_enum<ValueType>::value), "template parameter is not an enum type");
    std::ostringstream stm;
    stm << static_cast<unsigned int>(value);
    return stm.str();
}

/// @brief Generate a random integral value between the defined range.
/// @param lowerBound The lower bound for the random value.
/// @param upperBound The upper bound for the random value.
/// @return The generated random value.
template<typename ValueType>
ValueType TRandInteger(const ValueType & lowerBound, const ValueType & upperBound)
{
    static_assert((
                      std::is_same<ValueType, bool>::value ||
                      std::is_same<ValueType, int>::value ||
                      std::is_same<ValueType, int64_t>::value ||
                      std::is_same<ValueType, unsigned int>::value ||
                      std::is_same<ValueType, uint64_t>::value ||
                      std::is_same<ValueType, double>::value), "template parameter is of the wrong type");

    std::random_device rng;
    std::uniform_int_distribution<ValueType> uid(lowerBound, upperBound);
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
/// @return The resulting compressed stream of data.
std::vector<uint8_t> DeflateStream(std::vector<uint8_t> & uncompressed);

/// @brief It decompress a compressed strem of data.
/// @param compressed   The input compressed stream of data.
/// @return The resulting non-compressed stream of data.
std::vector<uint8_t> InflateStream(std::vector<uint8_t> & compressed);

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
    return false;
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
