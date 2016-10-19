/// @file   nodeModel.hpp
/// @brief  Define variables and methods of Node.
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

/// Used to determine the type of the node.
using NodeType = enum class NodeType_t
{
    NoType, ///< [0] No type.
    Metal,  ///< [1] Metal vein.
    Wood,   ///< [2] A tree.
    Stone   ///< [3] A monolith of stone.
};

/// @brief Model of a node of resources.
class NodeModel :
    public ItemModel
{
public:
    /// Type of node.
    NodeType nodeType;
    /// The vnum of the item provided during the extraction.
    //unsigned int provides;

    NodeModel();

    virtual ~NodeModel();

    ModelType getType() const override;

    std::string getTypeName() const override;

    bool setModel(const std::string & source) override;

    void getSheet(Table & sheet) const override;
};

/// @addtogroup EnumToString
/// @{

/// Return the string describing the type of a Node.
std::string GetNodeTypeName(NodeType type);

///@}

