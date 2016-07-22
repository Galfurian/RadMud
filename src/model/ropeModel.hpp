/// @file   ropeModel.hpp
/// @brief  Define variables and methods of Rope.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
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

#include "itemModel.hpp"

/// @brief Model of a rope.
class RopeModel: public ItemModel
{
    public:
        /// The difficulty to untie.
        unsigned int difficulty;
        /// Type of rope
        unsigned int ropeType;

        RopeModel();

        virtual ~RopeModel();

        virtual ModelType getType() const;

        virtual std::string getTypeName() const;

        virtual bool setModel(const std::string & source);

        virtual void getSheet(Table & sheet);
};
