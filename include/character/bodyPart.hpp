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

#include <string>
#include <vector>
#include <memory>

class ResourceModel;
class Table;
class Material;
class Race;

/// The flags of a body part.
enum class BodyPartFlag {
	None = 0, ///< [0]   No flag.
	CanWear = 1, ///< [1]   The equipment can be worn on the the body part.
	CanWield = 2, ///< [2]   The body part can be used to wield something.
	Internal = 4 ///< [4]   The body part is an internal part.
	///< [8]
	///< [16]
	///< [32]
	///< [64]
	///< [128]
};

/// @brief An anatomic part of the body.
class BodyPart {
public:
	/// @brief A structure to hold information about the resources provided
	/// by a body part.
	struct BodyResource {
		/// The provided resource.
		std::shared_ptr<ResourceModel> resource;
		/// The material of the resource.
		Material *material;
		/// The quantity.
		int quantity;
		/// The difficulty (DC) associated with the resources.
		int difficulty;
	};

	/// @brief A structure which holds information about the attack which can
	/// be performed with a body part.
	struct BodyWeapon {
		/// The name of the attack.
		std::string name;
		/// The article of the attack.
		std::string article;
		/// The minimum damage.
		unsigned int minDamage;
		/// The maximum damage.
		unsigned int maxDamage;
		/// The range of the natural weapon.
		int range;

		/// Returns the name of the attack.
		std::string getName(bool colored = false) const;

		/// Randomly roll the weapon damage.
		unsigned int rollDamage() const;
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
	/// The weapon ability of this body part.
	std::shared_ptr<BodyWeapon> weapon;

	/// @brief Constructor.
	BodyPart();

	/// @brief Destructor.
	virtual ~BodyPart();

	/// @brief Check the correctness of the body part.
	/// @return <b>True</b> if the body part has correct values,<br>
	///         <b>False</b> otherwise.
	bool check();

	/// @brief Return the name of the body part.
	/// @param capital If true, capitalize first letters.
	/// @return The name of the character.
	std::string getName(bool capital = false) const;

	/// @brief Return the description of the body part.
	/// @param capital If true, capitalize first letters.
	/// @return The description of the body part.
	std::string getDescription(Race *race, bool capital = false) const;

	/// @brief Fills the provided table with the information concerning
	///         the body part.
	void getSheet(Table &sheet) const;
};
