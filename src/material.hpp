/// @file   material.hpp
/// @brief  Define material class.
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

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <map>

#include "defines.hpp"
#include "lua/lua_script.hpp"

/// @brief Holds details about a material.
class Material
{
    public:
        /// Virtual number.
        int vnum;
        /// Type of material.
        MaterialType type;
        /// Material's name.
        std::string name;
        /// Material's article.
        std::string article;
        /// Material's value on market.
        int worth;
        /// Material's hardness.
        int hardness;
        /// Material's lightness.
        int lightness;

        /// @brief Constructor.
        Material();

        /// @brief Copy Constructor.
        Material(const Material & source);

        /// @brief Destructor.
        ~Material();

        /// @brief Check the correctness of the material.
        /// @return <b>True</b> if the material has correct values,<br><b>False</b> otherwise.
        bool check();

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);
};

/// Material map handler.
typedef std::map<int, Material> MaterialMap;

/// MaterialMap iterator.
typedef MaterialMap::iterator MaterialMapIterator;

#endif