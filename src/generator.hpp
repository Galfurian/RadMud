/// @file   generator.hpp
/// @brief  Define methods used to generate objects.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
/// @copyright
/// Copyright (c) 2014, 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "defines.hpp"

/// @brief Provides a method which generate a room description and name.
class Generator
{
    private:
        /// @brief Constructor.
        Generator();

        /// @brief Destructor.
        ~Generator();

        /// Struct used to characterize a zone.
        struct ZoneBaseNaming
        {
                /// The name of the zone.
                std::string name;
                /// The list of names for rooms inside this kind of zone.
                std::vector<std::string> names;
                /// The list of descriptions for rooms inside this kind of zone.
                std::vector<std::string> descriptions;
        };
        /// Struct used to characterize a zone status.
        struct ZoneStatusNaming
        {
                /// The name of the status.
                std::string name;
                /// The list of status for rooms inside this kind of zone.
                std::vector<std::string> namesStatus;
                /// The list of modified descriptions for rooms inside this kind of zone.
                std::vector<std::string> descriptionStatus;
        };
        /// Map between a type of area and the corresponding naming struct.
        std::map<AreaType, ZoneBaseNaming> mapBase;
        /// Map between the status of an area and the corresponding status naming struct.
        std::map<AreaStatus, ZoneStatusNaming> mapStatus;

        /// @brief Generate the room name and description.
        /// @param phase  Phase of the generation process.
        /// @param zone   Zone of the desired room.
        /// @param status Status of the desired room.
        void generate(int phase, std::string zone, std::string status);

    public:
        /// @brief Disable Copy Construct.
        Generator(Generator const &) = delete;

        /// @brief Disable Move construct.
        Generator(Generator &&) = delete;

        /// @brief Disable Copy assign.
        Generator & operator=(Generator const &) = delete;

        /// @brief Disable Move assign.
        Generator & operator=(Generator &&) = delete;

        /// @brief Get the singleton istance of the Generator.
        /// @return The static and uniquie Generator variable.
        static Generator & instance();

        /// @brief Generate the room name.
        /// @param type   Type of zone.
        /// @param status Status of the zone.
        /// @return The name of the room.
        std::string generateName(const AreaType & type, const AreaStatus & status);

        /// @brief Generate the room description.
        /// @param type     Type of zone.
        /// @param status   Status of the zone.
        /// @param roomName The name of the room.
        /// @return The description of the room.
        std::string generateDescription(
            const AreaType & type,
            const AreaStatus & status,
            const std::string & roomName);
};
