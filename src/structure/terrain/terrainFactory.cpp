/// @file   terrainFactory.cpp
/// @author Enrico Fraccaroli
/// @date   feb 07 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "terrainFactory.hpp"
#include "formatter.hpp"
#include "terrain.hpp"

std::shared_ptr<Terrain> TerrainFactory::getAir()
{
    static std::shared_ptr<Terrain> terrain;
    if (terrain == nullptr)
    {
        terrain = std::make_shared<Terrain>();
        terrain->name = "Air";
        terrain->flags = 0;
        terrain->generationFlags = 0;
        terrain->space = 1000;
        terrain->symbol = " ";
    }
    return terrain;
}

/// @brief Produces a terrain representing a cave.
std::shared_ptr<Terrain> TerrainFactory::getCave()
{
    static std::shared_ptr<Terrain> terrain;
    if (terrain == nullptr)
    {
        terrain = std::make_shared<Terrain>();
        terrain->name = "Cave";
        terrain->flags = static_cast<unsigned int>(TerrainFlag::Indoor);
        terrain->generationFlags = 0;
        terrain->space = 500;
        terrain->symbol = Formatter::darkGray(".");
    }
    return terrain;
}

/// @brief Produces a terrain representing soil.
std::shared_ptr<Terrain> TerrainFactory::getSoil()
{
    static std::shared_ptr<Terrain> terrain;
    if (terrain == nullptr)
    {
        terrain = std::make_shared<Terrain>();
        terrain->name = "Soil";
        terrain->flags = static_cast<unsigned int>(TerrainFlag::Indoor);
        terrain->generationFlags = 0;
        terrain->space = 0;
        terrain->symbol = Formatter::darkGray(" ");
    }
    return terrain;
}
