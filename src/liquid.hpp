/// @file   liquid.hpp
/// @brief  Define liquid class.
/// @author Enrico Fraccaroli
/// @date   Aug 7 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#ifndef LIQUID_HPP
#define LIQUID_HPP

#include <map>
#include "defines.hpp"

/// @brief Holds details about a liquid.
class Liquid
{
    public:
        /// Virtual number.
        int vnum;
        /// Liquid's name.
        std::string name;
        /// Liquid's value on market.
        int worth;

        /// @brief Constructor.
        Liquid();

        /// @brief Copy Constructor.
        Liquid(const Liquid & source);

        /// @brief Destructor.
        ~Liquid();

        /// @brief Check the correctness of the liquid.
        /// @return <b>True</b> if the liquid has correct values,<br><b>False</b> otherwise.
        bool check();

        /// @brief Return the name of the liquid.
        /// @return The name of the liquid.
        std::string getName();

        /// @brief Return the name of the liquid with the first letter capitalized.
        /// @return The name of the liquid.
        std::string getNameCapital();
};

/// Liquid list for containers.
typedef std::pair<Liquid *, unsigned int> LiquidContent;

/// Liquid map handler.
typedef std::map<int, Liquid> LiquidMap;

/// LiquidMap iterator.
typedef LiquidMap::iterator LiquidMapIterator;

#endif
