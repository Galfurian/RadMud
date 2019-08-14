/// @file   resourceType.hpp
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

#pragma once

#include "baseEnumerator.hpp"

/// Represents a type of resource.
class ResourceType : public BaseEnumerator {
public:
	/// The possible type of resources.
	enum Enum {
		None,
		Coal, ///< [1] COAL
		Ore, ///< [2] ORE
		Bar, ///< [3] BAR
		Log, ///< [4] LOG
		Plank, ///< [5] PLANK
		Tree, ///< [6] TREE
		Fastener, ///< [7] FASTENER
		Leather, ///< [8] LEATHER
		Cloth, ///< [9] CLOTH
		StoneBlock, ///< [10] STONE_BLOCK
		MetalVein, ///< [11] METAL_VEIN
		StoneMonolith, ///< [12] STONE_MONOLITH
		Pen, ///< [13] Pen
		Trash, ///< [14] Trash (Anything which has no specific use)
		Meat, ///< [15] MEAT
		Bone, ///< [16] BONE
		Skull, ///< [17] SKULL
		Nail ///< [18] NAIL
	};

	/// @brief Constructor.
	ResourceType() : BaseEnumerator()
	{
		// Nothing to do.
	}

	/// @brief Constructor from unsigned int.
	explicit ResourceType(const unsigned int &_value) : BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Constructor from enum.
	ResourceType(const Enum &_value) : BaseEnumerator(_value)
	{
		// Nothing to do.
	}

	/// @brief Returns the resource as string.
	std::string toString() const override;
};
