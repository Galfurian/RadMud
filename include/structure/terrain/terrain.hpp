/// @file   terrain.hpp
/// @brief  Declaration of Terrain class.
/// @author Enrico Fraccaroli
/// @date   08/12/2016
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

#include <string>
#include <lua.hpp>
#include <vector>

class Liquid;

/// Used to determine the flag of the terrain.
enum class TerrainFlag {
	None = 0, ///< No flag.
	Indoor = 1, ///< The terrain is indoor.
	NaturalLight = 2 ///< There is natural light on this terrain.
};

/// Used to determine the generation flag of the terrain.
enum class TerrainGenerationFlag {
	None = 0, ///< No flag.
	CanHostLiquidSource = 2, ///< The terrain can host a liquid source.
	CanHostForest = 4, ///< The terrain can host a forest.
	//8
};

/// @brief Holds information about a type of terrain.
class Terrain {
protected:
	/// The list of liquids which can generate from this terrain.
	struct LiquidSource {
		/// The liquid.
		Liquid *liquid;
		/// The nominal probability.
		unsigned int assignedProbability;
		/// The complexive probability.
		unsigned int cumulativeProbability;
	};

public:
	/// The vnum.
	unsigned int vnum;
	/// The unique name.
	std::string name;
	/// The flags of the terrain.
	unsigned int flags;
	/// The flags used during map generation.
	unsigned int generationFlags;
	/// The available space inside the terrain.
	unsigned int space;
	/// The symbol describing the terrain.
	std::string symbol;
	/// The lua_State associated with this terrain.
	lua_State *L;
	/// The liquid which fills the terrain by default.
	std::pair<Liquid *, unsigned int> liquidContent;
	/// A list of liquid sources.
	std::vector<LiquidSource> liquidSources;

	/// @brief Constructor.
	Terrain();

	/// @brief Adds a liquid source to the terrain.
	void addLiquidSource(Liquid *_liquid, const unsigned int &_assignedProbability);

	/// @brief Provides a random liquid source based on their probabilities.
	Liquid *getRandomLiquidSource() const;
};
