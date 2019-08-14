/// @file   generator.hpp
/// @brief  Define methods used to generate objects.
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "area.hpp"

/// @brief Provides a method which generate a room description and name.
class Generator {
private:
	/// @brief Constructor.
	Generator();

	/// @brief Destructor.
	~Generator();

	/// Struct used to characterize a zone.
	struct ZoneBaseNaming {
		/// The name of the zone.
		std::string name;
		/// The list of names for rooms inside this kind of zone.
		std::vector<std::string> names;
		/// The list of descriptions for rooms inside this kind of zone.
		std::vector<std::string> descriptions;
	};
	/// Struct used to characterize a zone status.
	struct ZoneStatusNaming {
		/// The name of the status.
		std::string name;
		/// The list of status for rooms inside this kind of zone.
		std::vector<std::string> namesStatus;
		/// The list of modified descriptions for rooms inside this
		///  kind of zone.
		std::vector<std::string> descriptionStatus;
	};
	/// Map between a type of area and the corresponding naming struct.
	std::map<AreaType, ZoneBaseNaming> mapBase;
	/// Map between the status of an area and the corresponding status
	///  naming struct.
	std::map<AreaStatus, ZoneStatusNaming> mapStatus;

public:
	/// @brief Disable Copy Construct.
	Generator(Generator const &) = delete;

	/// @brief Disable Move construct.
	Generator(Generator &&) = delete;

	/// @brief Disable Copy assign.
	Generator &operator=(Generator const &) = delete;

	/// @brief Disable Move assign.
	Generator &operator=(Generator &&) = delete;

	/// @brief Get the singleton istance of the Generator.
	/// @return The static and uniquie Generator variable.
	static Generator &instance();

	/// @brief Generate the room name.
	/// @param type   Type of zone.
	/// @param status Status of the zone.
	/// @return The name of the room.
	std::string generateName(const AreaType &type, const AreaStatus &status);

	/// @brief Generate the room description.
	/// @param type     Type of zone.
	/// @param status   Status of the zone.
	/// @param roomName The name of the room.
	/// @return The description of the room.
	std::string generateDescription(const AreaType &type,
									const AreaStatus &status,
									const std::string &roomName);
};
