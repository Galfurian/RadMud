/// @file   armorItem.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 04 2016
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

#include "item.hpp"

/// @brief Holds details about armors.
class ArmorItem: public Item
{
    public:
        ArmorItem();

        virtual ~ArmorItem();

        virtual ItemType getType() const;

        virtual std::string getTypeName() const;

        virtual void getSheet(Table & sheet) const;

        /// Provides the AC of the armor based on its quality, material and condition.
        /// @return the armor class.
        unsigned int getArmorClass() const;
};
