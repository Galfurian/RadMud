/// @file   bodyPart.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 08 2017
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

#pragma once

#include "table.hpp"
#include "material.hpp"
#include <string>
#include <vector>
#include <memory>

class ResourceModel;

/// The flags of a body part.
using BodyPartFlag = enum class BodyPartFlag_t
{
    None = 0,     ///< [0]   No flag.
    CanWear = 1,  ///< [1]   The equipment can be worn on the the body part.
    CanWield = 2, ///< [2]   The body part can be used to wield something.
    Internal = 4  ///< [4]   The body part is an internal part.
    ///< [8]
    ///< [16]
    ///< [32]
    ///< [64]
    ///< [128]
};

/// @brief An anatomic part of the body.
class BodyPart
{
public:
    struct BodyResource
    {
        std::shared_ptr<ResourceModel> resource;
        Material * material;
        int quantity;
    };

    /// The virtual number.
    unsigned int vnum;
    /// The name.
    std::string name;
    /// The description.
    std::string description;
    /// The flags associated with the body part.
    unsigned int flags;
    /// The resources which can be produced from this body part.
    std::vector<BodyResource> resources;

    /// @brief Constructor.
    BodyPart();

    /// @brief Destructor.
    virtual ~BodyPart();

    /// @brief Check the correctness of the body part.
    /// @return <b>True</b> if the body part has correct values,<br>
    ///         <b>False</b> otherwise.
    bool check();

    /// @brief Return the name of the body part with all lowercase characters.
    /// @param capital If true the first letters of the description are
    ///                 changed to upper case.
    /// @return The name of the character.
    std::string getDescription(bool capital = false) const;

    /// @brief Fills the provided table with the information concerning
    ///         the body part.
    void getSheet(Table & sheet) const;
};
