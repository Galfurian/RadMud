/// @file   item.hpp
/// @brief  Define item variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
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

#include <vector>
#include <list>
#include <map>
#include <mutex>

#include "updateInterface.hpp"
#include "itemVector.hpp"
#include "itemQuality.hpp"
#include "itemModel.hpp"
#include "nodeModel.hpp"
#include "liquid.hpp"
#include "table.hpp"

class Room;

class Character;

class Material;

/// Used to determine the flag of the item.
enum class ItemFlag {
	None = 0,
	Closed = 1,
	Locked = 2,
	Built = 4,
	Temporary = 8
};

/// @brief Holds details about items.
class Item : public UpdateInterface {
public:
	/// Item vnum.
	unsigned int vnum;
	/// Item model.
	std::shared_ptr<ItemModel> model;
	/// The number of stacked items.
	unsigned int quantity;
	/// The player that created the item.
	std::string maker;
	/// The item's condition.
	double condition;
	/// The composing material of the item.
	Material *composition;
	/// The quality of the item.
	ItemQuality quality;
	/// The item flags.
	unsigned int flags;
	/// Pointer to the room, where the item resides.
	Room *room;
	/// Pointer to the character who owns this item.
	Character *owner;
	/// Pointer to the item which contains this item.
	Item *container;
	/// The body parts occupied by the item.
	std::vector<std::shared_ptr<BodyPart> > occupiedBodyParts;
	/// List of items contained in this one.
	ItemVector content;
	/// Mutex used to protect the actions on the item.
	std::mutex itemMutex;

	/// @brief Constructor - Create a new empty item.
	Item();

	/// @brief Disable Copy Construct.
	Item(Item const &) = delete;

	/// @brief Disable Move construct.
	Item(Item &&) = delete;

	/// @brief Disable Copy assign.
	Item &operator=(Item const &) = delete;

	/// @brief Disable Move assign.
	Item &operator=(Item &&) = delete;

	/// @brief Destructor.
	virtual ~Item();

	/// @brief Check the correctness of the item.
	/// @return <b>True</b> if the item has correct values,<br>
	///         <b>False</b> otherwise.
	virtual bool check();

	/// @brief This function is used to remove the item from everywhere.
	virtual bool removeFromMud();

	/// @brief Create or Update the item entry on database.
	/// @return <b>True</b> if the execution goes well,<br>
	///         <b>False</b> otherwise.
	virtual bool updateOnDB();

	/// @brief Remove the item on database.
	/// @return <b>True</b> if the execution goes well,<br>
	///         <b>False</b> otherwise.
	virtual bool removeOnDB();

	/// @brief Fills the provided table with the information
	///         concerning the item.
	/// @param sheet The table that has to be filled.
	virtual void getSheet(Table &sheet) const;

	/// @brief Check if the item can be deconstructed.
	/// @param error In case the item cannot be deconstructed,
	///               error contains the reason.
	/// @return <b>True</b> if it can be deconstructed,<br>
	///         <b>False</b> otherwise.
	virtual bool canDeconstruct(std::string &error) const;

	/// Provides the type of item.
	ModelType getType() const;

	/// Provides a string representing the type of item.
	std::string getTypeName() const;

	/// @brief Check if the provided item can be stacked with this one.
	/// @param item The item to check.
	/// @return <b>True</b> if it can be stacked,<br>
	///         <b>False</b> otherwise.
	bool canStackWith(Item *item) const;

	/// @brief Allows to separate a new stack of this item.
	/// @param actor      The character which is separating the items.
	/// @param _quantity  The dimension of the new stack.
	/// @return The new stack of items.
	Item *removeFromStack(Character *actor, unsigned int &_quantity);

	/// @brief Check if the item has the desired key.
	/// @param key The key to search.
	/// @return <b>True</b> if the operations succeeded,<br>
	///         <b>False</b> Otherwise.
	bool hasKey(std::string key);

	/// @brief Provides the rate at which the item decays each TIC.
	virtual double getDecayRate() const;

	/// @brief Trigger a decay cycle.
	virtual void triggerDecay();

	/// @brief Provides the modifier due to the item's condition.
	double getConditionModifier() const;

	/// @brief Get the item current condition in text.
	/// @return The condition of the item.
	std::string getConditionDescription();

	/// @brief Provides the price of the item based on its quality,
	///         material and condition.
	/// @param entireStack If <b>true</b> this function returns the
	///                     price of the entire stack.
	/// @return The price of the item.
	virtual unsigned int getPrice(bool entireStack) const;

	/// @brief Get the item weight, plus eventually contained item weight.
	/// @param entireStack If <b>true</b> this function returns
	///                     the weight of the entire stack.
	/// @return The total weight of the item.
	virtual double getWeight(bool entireStack) const;

	/// @brief Get the item maximum condition.
	virtual unsigned int getMaxCondition() const;

	/// @brief Return the name of the item.
	/// @param colored If <b>true</b> the name also include formatting color.
	/// @return The name of the item.
	virtual std::string getName(bool colored = false) const;

	/// @brief Return the name of the item with the first letter capitalized.
	/// @param colored If <b>true</b> the name also include formatting color.
	/// @return The name of the item.
	virtual std::string getNameCapital(bool colored = false) const;

	/// @brief Return the description of the item.
	/// @return The description of the item.
	std::string getDescription();

	/// @brief Provide a detailed description of the item,
	///         including it's condition.
	/// @return A detailed description of the item.
	std::string getLook();

	/// @brief Return the description of the content.
	/// @return The string describing the content.
	virtual std::string lookContent();

	/// @brief Check if the item is a valid container.
	/// @return <b>True</b> if it is a valid container,<br>
	///         <b>False</b> otherwise.
	virtual bool isAContainer() const;

	/// @brief Check if the container is empty.
	/// @return <b>True</b> if the item is empty,<br>
	///         <b>False</b> otherwise.
	virtual bool isEmpty() const;

	/// @brief Return the total space of the container.
	/// @return The total space as an integer.
	virtual double getTotalSpace() const;

	/// @brief Return the used space of the container.
	/// @return The used space as an integer.
	virtual double getUsedSpace() const;

	/// @brief Return the free space inside the container.
	/// @return The free unit of space as an integer.
	virtual double getFreeSpace() const;

	/// @brief Check if this item can contain the passed one.
	/// @param item   The item to check.
	/// @param amount The amount to check.
	/// @return <b>True</b> if it can be contained,<br>
	///         <b>False</b> otherwise.
	bool canContain(Item *item, const unsigned int &amount) const;

	/// @brief Load an item inside the container and update the database.
	/// @param item     The item to load in.
	/// @param updateDB If the action has to be updated on the database.
	void putInside(Item *&item, bool updateDB = true);

	/// @brief Extract an item from the container.
	/// @param item The item to load in.
	/// @param updateDB If the action has to be updated on the database.
	/// @return <b>True</b> if the item has been taken out,<br>
	///         <b>False</b> otherwise.
	bool takeOut(Item *item, bool updateDB = true);

	/// @brief Search for the item inside the container.
	/// @param search_parameter The item to search.
	/// @param number           Position of the item we want to look for.
	/// @return The item, if it's in the container.
	Item *findContent(std::string search_parameter, int &number);

	/// @brief Set the body parts occupied by the item.
	/// @param _occupiedBodyParts The occupied body parts.
	void setOccupiedBodyParts(
		std::vector<std::shared_ptr<BodyPart> > _occupiedBodyParts);

	/// @brief Operator used to order the items based on their name.
	bool operator<(Item &rhs) const;

protected:
	void updateTicImpl() override;

	void updateHourImpl() override;
};
