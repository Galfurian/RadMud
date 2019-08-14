/// @file   nodeModel.cpp
/// @brief  Iplement the methods for Node.
/// @author Enrico Fraccaroli
/// @date   Jul 6 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#include "nodeModel.hpp"

#include "logger.hpp"

NodeModel::NodeModel() : nodeType()
//,provides()
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

bool NodeModel::setModel(const std::string &source)
{
	if (source.empty()) {
		Logger::log(LogLevel::Error, "Function list is empty (%s).",
					this->name);
		return false;
	}
	std::vector<std::string> functionList = SplitString(source, " ");
	if (functionList.size() != 1) {
		Logger::log(LogLevel::Error,
					"Wrong number of parameters for Node Model (%s).",
					this->name);
		return false;
	}
	this->nodeType =
		static_cast<NodeType>(ToNumber<unsigned int>(functionList[0]));
	//this->provides = ToNumber<unsigned int>(functionList[1]);
	return true;
}

void NodeModel::getSheet(Table &sheet) const
{
	// Call the function of the father class.
	ItemModel::getSheet(sheet);
	// Add a divider.
	sheet.addDivider();
	// Set the values.
	sheet.addRow({ "Node Type", GetNodeTypeName(this->nodeType) });
	//sheet.addRow({"Provides", ToString(this->provides)});
}

std::string GetNodeTypeName(NodeType type)
{
	if (type == NodeType::Metal)
		return "Metal Node";
	if (type == NodeType::Wood)
		return "Wood Node";
	if (type == NodeType::Stone)
		return "Stone Node";
	if (type == NodeType::Metal)
		return "Metal Node";
	return "No Node Type";
}
