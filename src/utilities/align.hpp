/// @file   align.hpp
/// @author Enrico Fraccaroli
/// @date   Feb 13 2017

#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace align
{

/// Kinds of string alignment.
enum align_t
{
    left,
    center,
    right,
};

/// @brief Helper class to center strings.
template<typename charT, typename traits = std::char_traits<charT> >
class center_helper
{
private:
    std::basic_string<charT, traits> str_;

public:
    explicit center_helper(std::basic_string<charT, traits> str) :
        str_(str)
    {
        // Nothing to do.
    }

    template<typename a, typename b>
    friend std::basic_ostream<a, b> & operator<<(std::basic_ostream<a, b> & s,
                                                 const center_helper<a, b> & c);
};

template<typename charT, typename traits = std::char_traits<charT> >
inline center_helper<charT, traits> centtered(
    std::basic_string<charT, traits> str)
{
    return center_helper<charT, traits>(str);
}

// redeclare for std::string directly so we can support anything that
// implicitly converts to std::string.
inline center_helper<std::string::value_type, std::string::traits_type>
centtered(const std::string & str)
{
    return center_helper<std::string::value_type,
                         std::string::traits_type>(str);
}

template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
inline center_helper<std::string::value_type, std::string::traits_type>
centtered(T value)
{
    return centtered(std::to_string(value));
}

template<typename charT, typename traits>
inline std::basic_ostream<charT, traits> & operator<<(
    std::basic_ostream<charT, traits> & s,
    const center_helper<charT, traits> & c)
{
    std::streamsize w = s.width();
    auto cl = static_cast<long int>(c.str_.length());
    if (w > cl)
    {
        std::streamsize left = (w + cl) / 2;
        s.width(left);
        s << c.str_;
        s.width(w - left);
        s << "";
    }
    else
    {
        s << c.str_;
    }
    return s;
}

}

/// @brief Align the given string.
/// @param source    The source string.
/// @param alignment The kind of alignment.
/// @param width     The total width of the string.
/// @return The aligned string.
template<typename ValueType>
inline std::string Align(const ValueType & s,
                         const align::align_t & a,
                         const size_t & w)
{
    // Create a string stream.
    std::stringstream ss;
    // Set the width
    ss << std::setw(static_cast<int>(w));
    // Align the string.
    if (a == align::left) ss << std::left << s;
    else if (a == align::center) ss << align::centtered(s);
    else if (a == align::right) ss << std::right << s;
    return ss.str();
}
