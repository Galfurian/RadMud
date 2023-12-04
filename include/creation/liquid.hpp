/// @file   liquid.hpp
/// @brief  Define liquid class.
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

#include "enumerators/liquidType.hpp"

/// Flags of a liquid.
using LiquidFlags = enum class LiquidFlags_t
{
    None,               ///< [0]   No flag.
    Inflammable = 1,    ///< [1]   The liquid is inflammable.
    AlcoholicBase = 2,  ///< [2]   The liquid can be used as base for mixtures.
    DealDamage = 4      ///< [4]   The liquid deals damage over time.
    ///< [8]
    ///< [16]
    ///< [32]
    ///< [64]
    ///< [128]
};

/// @brief Holds details about a liquid.
class Liquid
{
public:
    /// Virtual number.
    unsigned int vnum;
    /// The type of liquid.
    LiquidType type;
    /// Liquid's name.
    std::string name;
    /// Short description of the liquid.
    std::string description;
    /// The liquid's flags.
    unsigned int flags;
    /// Liquid's value on market.
    int worth;
    /// How much the liquid is able to quench the thirst.
    double quench;

    /// @brief Constructor.
    Liquid();

    /// @brief Disable Copy Construct.
    Liquid(Liquid const &) = delete;

    /// @brief Disable Move construct.
    Liquid(Liquid &&) = delete;

    /// @brief Disable Copy assign.
    Liquid & operator=(Liquid const &) = delete;

    /// @brief Disable Move assign.
    Liquid & operator=(Liquid &&) = delete;

    /// @brief Destructor.
    ~Liquid();

    /// @brief Check the correctness of the liquid.
    /// @return <b>True</b> if the liquid has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Return the name of the liquid.
    /// @return The name of the liquid.
    std::string getName();

    /// @brief Return the name of the liquid with the first letter capitalized.
    /// @return The name of the liquid.
    std::string getNameCapital();
};
