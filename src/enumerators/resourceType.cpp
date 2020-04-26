/// @file   resourceType.cpp
/// @brief
/// @author Enrico Fraccaroli
/// @date   05/12/2016
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

#include "enumerators/resourceType.hpp"

std::string ResourceType::toString() const
{
	if (value == ResourceType::Coal)
		return "Natural Coal";
	if (value == ResourceType::Ore)
		return "Metal Ore";
	if (value == ResourceType::Bar)
		return "Metal Bar";
	if (value == ResourceType::Log)
		return "Wood Log";
	if (value == ResourceType::Plank)
		return "Wood Plank";
	if (value == ResourceType::Tree)
		return "Tree";
	if (value == ResourceType::Fastener)
		return "Fastener";
	if (value == ResourceType::Leather)
		return "Leather";
	if (value == ResourceType::Cloth)
		return "Cloth";
	if (value == ResourceType::StoneBlock)
		return "Stone Block";
	if (value == ResourceType::MetalVein)
		return "Metal Vein";
	if (value == ResourceType::StoneMonolith)
		return "Stone Monolith";
	if (value == ResourceType::Pen)
		return "Pen";
	if (value == ResourceType::Trash)
		return "Trash";
	if (value == ResourceType::Meat)
		return "Meat";
	if (value == ResourceType::Bone)
		return "Bone";
	if (value == ResourceType::Skull)
		return "Skull";
	if (value == ResourceType::Nail)
		return "Nail";
	return "None";
}
