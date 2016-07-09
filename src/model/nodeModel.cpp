/// @file   nodeModel.cpp
/// @brief  Iplement the methods for Node.
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

#include "nodeModel.hpp"

NodeModel::NodeModel() :
        nodeType(),
        provides()
{
    // Nothing to do.
}

NodeModel::~NodeModel()
{
    // Nothing to do.
}

ModelType NodeModel::getType() const
{
    return ModelType::Node;
}

std::string NodeModel::getTypeName() const
{
    return "Node";
}

bool NodeModel::setModel(const std::string & source)
{
    if (source.empty())
    {
        Logger::log(LogLevel::Error, "Function list is empty (%s).", this->name);
        return false;
    }
    std::vector<std::string> functionList = SplitString(source, " ");
    if (functionList.size() != 2)
    {
        Logger::log(LogLevel::Error, "Wrong number of parameters for Node Model (%s).", this->name);
        return false;
    }
    this->nodeType = static_cast<NodeType>(ToNumber<unsigned int>(functionList[0]));
    this->provides = ToNumber<unsigned int>(functionList[1]);
    return true;
}

std::string GetNodeTypeName(NodeType type)
{
    if (type == NodeType::Metal) return "Metal Node";
    if (type == NodeType::Wood) return "Wood Node";
    if (type == NodeType::Stone) return "Stone Node";
    if (type == NodeType::Metal) return "Metal Node";
    return "No Node Type";
}
