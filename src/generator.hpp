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

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <cstdio>

/// @brief Provides a method which generate a room description and name.
class Generator
{
    private:
        /// Name of the room.
        std::string name;
        /// Additional status for the room's name.
        std::string name_status;
        /// Room description.
        std::string description;
        /// An auxiliary vector.
        std::vector<std::string> auxiliary_vector;
        /// List of all the zones name.
        std::map<std::string, std::vector<std::string> > zones_names;
        /// List of all the name's modification for all the status.
        std::map<std::string, std::vector<std::string> > status_names;
        /// List of all the zones descriptions.
        std::map<std::string, std::vector<std::string> > zones_description;
        /// List of all the description's modification for all the status.
        std::map<std::string, std::vector<std::string> > status_description;

        /// @brief Generate the room name and description.
        /// @param phase  Phase of the generation process.
        /// @param zone   Zone of the desired room.
        /// @param status Status of the desired room.
        void generate(int phase, std::string zone, std::string status);

    public:

        /// @brief Constructor.
        Generator();

        /// @brief Destructor.
        ~Generator();

        /// @brief Generate the room name and description.
        /// @param d_zone   Zone of the desired room.
        /// @param d_status Status of the desired room.
        /// @return The name of the room and the description.
        std::pair<std::string, std::string> generateRoom(std::string d_zone, std::string d_status);
};
#endif
