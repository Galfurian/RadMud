/// @file   material.hpp
/// @brief  Define material class.
/// @author Enrico Fraccaroli
/// @date   Aug 7 2015
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

#include "materialType.hpp"

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
    unsigned int worth;
    /// Material's hardness.
    unsigned int hardness;
    /// Material's lightness.
    unsigned int lightness;

    /// @brief Constructor.
    Material();

    /// @brief Disable Copy Construct.
    Material(Material const &) = delete;

    /// @brief Disable Move construct.
    Material(Material &&) = delete;

    /// @brief Disable Copy assign.
    Material & operator=(Material const &) = delete;

    /// @brief Disable Move assign.
    Material & operator=(Material &&) = delete;

    /// @brief Destructor.
    ~Material();

    /// @brief Check the correctness of the material.
    /// @return <b>True</b> if the material has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Provides the modifier w.r.t. the material worth value.
    double getWorthModifier() const;

    /// @brief Provides the modifier w.r.t. the material hardness value.
    double getHardnessModifier() const;

    /// @brief Provides the modifier w.r.t. the material lightness value.
    double getLightnessModifier() const;
};
