/// @file   area.hpp
/// @brief  An area contains a 3D map of rooms, this file defines all the methods needed to interact with an area.
/// @author Enrico Fraccaroli
/// @date   Mar 10 2015
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
#include <vector>

#include "structure/coordinates.hpp"
#include "utilities/map3D.hpp"
#include "utilities/map2D.hpp"
#include "character/character.hpp"

class Room;

/// Used to determine the type of Zone.
enum class AreaType {
	Normal, ///< The area is quite normal.
	Cavern, ///< Identifies a cavern.
	Underdark ///< Identifies a region of the underdark.
};

/// Used to determine the status of a Zone.
enum class AreaStatus {
	Normal, ///< The area has no environmental status.
	Iced, ///< The area is freezing.
	Magmatic, ///< The area have volcanic activities.
	Vegetal ///< The area is overrun by the vegetation.
};

/// @brief This class contains all the information regarding an entire area.
class Area {
public:
	/// The virtual number of the area.
	unsigned int vnum;
	/// The name of the area.
	std::string name;
	/// The name of the original builder of this area.
	std::string builder;
	/// The 3D grid of the map.
	Map3D<Room *> map;
	/// The width of the area.
	int width;
	/// The height of the area.
	int height;
	/// The elevation of the area.
	int elevation;
	/// The tileset of the entire area.
	int tileSet;
	/// The type of area.
	AreaType type;
	/// The status of the area.
	AreaStatus status;

	/// Constructor.
	Area();

	/// Destructor.
	virtual ~Area();

	/// @brief Check the correctness of the area.
	/// @return <b>True</b> if the area has correct values,<br>
	///         <b>False</b> otherwise.
	bool check();

	/// @brief Check if the given coordinates is inside the boundaries.
	/// @param coordinates The coordinates to check.
	/// @return <b>True</b> if the coordinates are valid,<br>
	///         <b>False</b> otherwise.
	bool inBoundaries(const Coordinates &coordinates) const;

	/// @brief Checks if At the given coordinates there is a valid room
	///         through which a player Can See.
	/// @param coordinates The provided coordinates.
	/// @return <b>True</b> if there is a room at the given coordinates,<br>
	///         <b>False</b> otherwise.
	bool isValid(const Coordinates &coordinates);

	/// @brief Provides all the characters inside the room at
	///         the given coordinates.
	/// @param exceptions  The exceptions.
	/// @param coordinates The desired coordinates.
	/// @return The list of characters at the given coordinates.
	CharacterVector getCharactersAt(const CharacterVector &exceptions,
									const Coordinates &coordinates);

	/// @brief Provides all the items inside the room at the given coordinates.
	/// @param exceptions  The exceptions.
	/// @param coordinates The desired coordinates.
	/// @return The list of items at the given coordinates.
	ItemVector getItemsAt(const ItemVector &exceptions, const Coordinates &coordinates);

	/// @brief Add the passed room to its coordinates inside the area.
	/// @param room The room that has to be added.
	/// @return <b>True</b> if the room has been added,<br>
	///         <b>False</b> otherwise.
	bool addRoom(Room *room);

	/// @brief Remove the passed room.
	/// @param room The room that has to be removed.
	/// @return <b>True</b> if the room has been removed,<br>
	///         <b>False</b> otherwise.
	bool remRoom(Room *room);

	/// @brief Find a room  given its vnum.
	/// @param room_vnum The vnum of the room.
	/// @return The desired room.
	Room *getRoom(unsigned int room_vnum);

	/// @brief Find a room in a precise spot.
	/// @param coordinates The coordiantes where search the room.
	/// @return The room at the selected spot.
	Room *getRoom(const Coordinates &coordinates);

	/// @brief Draw the Filed of View for a character.
	/// @param centerRoom The room from where the algorithm has to
	///                     compute the Field of View.
	/// @param radius     The radius of visibility of the character.
	/// @return The map containing all the Information about the
	///          Field of View of a character.
	std::vector<std::string> drawFov(Room *centerRoom, const int &radius);

	/// @brief Draw the Filed of View for a character (ASCII).
	/// @param centerRoom The room from where the algorithm has to
	///                     compute the Field of View.
	/// @param radius     The radius of visibility of the character.
	/// @return The map containing all the Information about the
	///          Field of View of a character.
	std::string drawASCIIFov(Room *centerRoom, const int &radius, double const &origin_height);

	/// @brief Draw the Filed of View for a character (ASCII).
	/// @param centerRoom The room from where the algorithm has to
	///                     compute the Field of View.
	/// @param radius     The radius of visibility of the character.
	/// @return The map containing all the Information about the
	///          Field of View of a character.
	std::string getASCIICell(Room *room);

	/// @brief Provides a list of characters which are in sight.
	/// @param exceptions A list of exceptions.
	/// @param origin The coordinate of the central room.
	/// @param radius     The radius of visibility.
	/// @return The list containing the targets.
	CharacterVector getCharactersInSight(CharacterVector &exceptions, Coordinates &origin,
										 const int &radius);

	/// @brief Provides a list of items which are in sight.
	/// @param exceptions A list of excections.
	/// @param origin The coordinate of the central room.
	/// @param radius     The radius of visibility.
	/// @return The list containing the items.
	ItemVector getItemsInSight(ItemVector &exceptions, Coordinates &origin, const int &radius);
};