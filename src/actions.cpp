/// @file   actions.cpp
/// @brief  Implements all the actions that a player can do and which takes more than one turn to conclude.
/// @author Enrico Fraccaroli
/// @date   Feb 28 2015
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

// Basic Include.
#include "actions.hpp"

// Other Include.
#include "mud.hpp"
#include "logger.hpp"
#include "constants.hpp"

#include "model/nodeModel.hpp"
#include "model/weaponModel.hpp"

Action::Action(Character * _character) :
        type(ActionType::Wait),
        actor(_character),
        target(),
        itemTarget(),
        destination(),
        direction(Direction::None),
        production(),
        schematics(),
        craftMaterial(),
        usedTools(),
        usedIngredients(),
        actionCooldown(),
        combatAction()
{
    // Nothing to do.
}

Action::~Action()
{
    // Nothing to do.
}

ActionType Action::getType() const
{
    return this->type;
}

CombatAction Action::getCombatAction() const
{
    return this->combatAction;
}

std::string Action::getDescription() const
{
    if (this->type == ActionType::Move)
    {
        return "moving";
    }
    else if (this->type == ActionType::Crafting)
    {
        if (this->production != nullptr)
        {
            if (this->production->profession != nullptr)
            {
                return this->production->profession->action;
            }
        }
    }
    else if (this->type == ActionType::Building)
    {
        return "moving";
    }
    else if (this->type == ActionType::Combat)
    {
        return "fighting";
    }
    return "";
}

std::string Action::stop()
{
    std::string msg;
    if (this->type == ActionType::Wait)
    {
        msg = "Excuse me sir, but you are doing nothing.";
    }
    else if (this->type == ActionType::Move)
    {
        msg = "You stop moving.";
    }
    else if (this->type == ActionType::Crafting)
    {
        if (this->production != nullptr)
        {
            if (this->production->profession != nullptr)
            {
                msg = this->production->profession->interruptMessage + ".";
            }
        }
    }
    else if (this->type == ActionType::Building)
    {
        msg = "You stop building.";
    }
    else if (this->type == ActionType::Combat)
    {
        msg = "You stop fighting.";
    }
    else
    {
        msg = "You stop whatever you were doing.";
    }
    this->reset();
    return msg;
}

void Action::perform()
{
    if (this->type == ActionType::Move)
    {
        this->performMove();
    }
    else if (this->type == ActionType::Crafting)
    {
        this->performCraft();
    }
    else if (this->type == ActionType::Building)
    {
        this->performBuild();
    }
    else if (this->type == ActionType::Combat)
    {
        this->performComb();
    }
}

bool Action::setMove(Room * _destination, Direction _direction, unsigned int _cooldown)
{
    bool correct = true;
    if (_destination == nullptr)
    {
        Logger::log(LogLevel::Error, "No destination has been set.");
        correct = false;
    }
    if (_direction == Direction::None)
    {
        Logger::log(LogLevel::Error, "No direction has been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        Logger::log(LogLevel::Error, "No cooldown has been set.");
        correct = false;
    }
    if (correct)
    {
        type = ActionType::Move;
        destination = _destination;
        direction = _direction;
        actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(_cooldown);
    }
    return correct;
}

bool Action::setCraft(
    Production * _production,
    ItemVector & _usedTools,
    ItemVector & _usedIngredients,
    Material * _craftMaterial,
    unsigned int _cooldown)
{
    bool correct = true;
    if (_production == nullptr)
    {
        Logger::log(LogLevel::Error, "No profession has been set.");
        correct = false;
    }
    if (_usedTools.empty())
    {
        Logger::log(LogLevel::Error, "No used tools have been set.");
        correct = false;
    }
    if (_usedIngredients.empty())
    {
        Logger::log(LogLevel::Error, "No used ingredients have been set.");
        correct = false;
    }
    if (_craftMaterial == nullptr)
    {
        Logger::log(LogLevel::Error, "No crafting material has been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        Logger::log(LogLevel::Error, "No cooldown has been set.");
        correct = false;
    }
    if (correct)
    {
        type = ActionType::Crafting;
        production = _production;
        usedTools = _usedTools;
        usedIngredients = _usedIngredients;
        craftMaterial = _craftMaterial;
        actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(_cooldown);
    }
    return correct;
}

bool Action::setBuild(
    Building * _schematics,
    Item * _itemTarget,
    ItemVector & _usedTools,
    ItemVector & _usedIngredients,
    unsigned int _cooldown)
{
    bool correct = true;
    if (_schematics == nullptr)
    {
        Logger::log(LogLevel::Error, "No schematics have been set.");
        correct = false;
    }
    if (_itemTarget == nullptr)
    {
        Logger::log(LogLevel::Error, "No item is set for the building.");
        correct = false;
    }
    if (_usedTools.empty())
    {
        Logger::log(LogLevel::Error, "No used tools have been set.");
        correct = false;
    }
    if (_cooldown == 0)
    {
        Logger::log(LogLevel::Error, "No cooldown has been set.");
        correct = false;
    }
    if (correct)
    {
        type = ActionType::Building;
        schematics = _schematics;
        itemTarget = _itemTarget;
        usedTools = _usedTools;
        usedIngredients = _usedIngredients;
        actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(_cooldown);
    }
    return correct;
}

bool Action::setInCombat()
{
    if (this->type != ActionType::Wait)
    {
        return false;
    }
    else
    {
        this->type = ActionType::Combat;
        return true;
    }
}

bool Action::setNextCombatAction(const CombatAction & nextAction)
{
    if (!actor->opponents.hasOpponents())
    {
        Logger::log(LogLevel::Error, "The list of opponents is empty.");
        return false;
    }
    if (type != ActionType::Combat)
    {
        Logger::log(LogLevel::Error, "The character is not fighting.");
        return false;
    }
    // Set the next combat action.
    combatAction = nextAction;
    // Set the action cooldown.
    unsigned int cooldown = actor->getCooldown(combatAction);
    actionCooldown = std::chrono::system_clock::now() + std::chrono::seconds(cooldown);
    Logger::log(
        LogLevel::Debug,
        "[%s] Next action in %s.",
        actor->getNameCapital(),
        ToString(cooldown));
    return true;
}

void Action::reset()
{
    type = ActionType::Wait;
    target = nullptr;
    itemTarget = nullptr;
    destination = nullptr;
    direction = Direction::None;
    production = nullptr;
    schematics = nullptr;
    craftMaterial = nullptr;
    usedTools.clear();
    usedIngredients.clear();
    actionCooldown = std::chrono::system_clock::now();
    combatAction = CombatAction::NoAction;
}

bool Action::checkElapsed()
{
    TimeClock currentTime = std::chrono::system_clock::now();
    return (std::chrono::duration_cast<std::chrono::seconds>(actionCooldown - currentTime).count()
        <= 0);
}

void Action::performMove()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }
    // Create a variable which will contain the ammount of consumed stamina.
    unsigned int consumedStamina;
    // Check if the actor has enough stamina to execute the action.
    if (actor->hasStaminaFor(consumedStamina, ActionType::Move))
    {
        // Consume the stamina.
        actor->remStamina(consumedStamina);
        // Move character.
        actor->moveTo(
            destination,
            actor->getNameCapital() + " goes " + GetDirectionName(direction) + ".\n",
            actor->getNameCapital() + " arives from " + GetDirectionName(InverDirection(direction))
                + ".\n");
    }
    else
    {
        // Notify that the actor can't move because too tired.
        actor->sendMsg("You are too tired right now.\n");
        // Debugging log.
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            actor->getName(),
            ToString(actor->getStamina()),
            ToString(consumedStamina));
    }
    // Return the character in waiting status.
    this->reset();
}

void Action::performMine()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    // Check if the presence of the tool.
    if (usedTools.empty())
    {
        // Notify the character.
        actor->sendMsg("You don't have any tool.\n");
        return;
    }
    else if (usedTools.front() == nullptr)
    {
        // Notify the character.
        actor->sendMsg("You don't have the right tool.\n");
        return;
    }
    // Check if the vein is still there.
    if (itemTarget == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = Vein is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    if (itemTarget->model->getType() != ModelType::Node)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, "%s:mine = Vein is node a node.", actor->getName());
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    NodeModel * node = itemTarget->model->toNode();
    // Retrieve the product.
    ItemModel * product = Mud::instance().findItemModel(static_cast<int>(node->provides));
    // Check if the product exists.
    if (product == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = Product is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Retrieve the material of the resources node.
    Material * material = itemTarget->composition;
    // Check the existance of the material.
    if (material == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":mine = Material is a null pointer.");
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    // Create the product.
    SQLiteDbms::instance().beginTransaction();
    Item * newItem = product->createItem(actor->getName(), material, ItemQuality::Normal);
    // Check if the creation is not null.
    if (newItem == nullptr)
    {
        // Log a warning.
        Logger::log(
            LogLevel::Warning,
            actor->getName() + ":mine = The new item is a null pointer.");
        // Rollback the database.
        SQLiteDbms::instance().rollbackTransection();
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Delete the item.
        delete (newItem);
        return;
    }
    SQLiteDbms::instance().endTransaction();
    // Check if the character can carry the item.
    if (!actor->canCarry(newItem))
    {
        actor->sendMsg("%s is too heavy, it will be left on the ground.\n", newItem->getName());
        actor->room->addItem(newItem);
    }
    else
    {
        actor->addInventoryItem(newItem);
    }
    // Update the condition of the involved objects.
    if (itemTarget->editCondition(-10))
    {
        actor->sendMsg("With your last blow, %s crumbles into pieces.\n", itemTarget->getName());
        // Set the list of exceptions.
        CharacterVector exceptions;
        exceptions.push_back(actor);
        // Send the message inside the room.
        actor->room->sendToAll(
            "%s crumbles into pieces.\n",
            exceptions,
            itemTarget->getNameCapital());
        // Destroy the vein.
        itemTarget->destroy();
    }
    // Check if the tool is destroyed.
    Item * tool = dynamic_cast<Item *>(usedTools.front());
    if (tool->editCondition(-1))
    {
        actor->sendMsg("%s falls into pieces.\n", tool->getName());
        tool->destroy();
    }

    // Advise character that he has ended the action.
    actor->sendMsg("You have successfully extracted some minerals.\n\n");

    // Return the character in waiting status.
    this->reset();
}

void Action::performChop()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    // Check if the presence of the tool.
    if (usedTools.empty())
    {
        // Notify the character.
        actor->sendMsg("\nYou don't have any tool.\n");
        return;
    }
    else if (usedTools.front() == nullptr)
    {
        // Notify the character.
        actor->sendMsg("\nYou don't have the right tool.\n");
        return;
    }
    // Check if the vein is still there.
    if (itemTarget == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = Tree is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    if (itemTarget->model->getType() != ModelType::Node)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, "%s:chop = Vein is node a node.", actor->getName());
        // Notify the character.
        actor->sendMsg("You have failed your action.\n");
        return;
    }
    NodeModel * node = itemTarget->model->toNode();
    // Retrieve the product.
    ItemModel * product = Mud::instance().findItemModel(static_cast<int>(node->provides));
    // Check if the product exists.
    if (product == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = Product is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Retrieve the material of the resources node.
    Material * material = itemTarget->composition;
    // Check the existance of the material.
    if (material == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":chop = Material is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Create the product.
    SQLiteDbms::instance().beginTransaction();
    Item * newItem = product->createItem(actor->getName(), material, ItemQuality::Normal);
    // Check if the creation is not null.
    if (newItem == nullptr)
    {
        // Log a warning.
        Logger::log(
            LogLevel::Warning,
            actor->getName() + ":chop = The new item is a null pointer.");
        // Rollback the database.
        SQLiteDbms::instance().rollbackTransection();
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Delete the item.
        delete (newItem);
        return;
    }
    SQLiteDbms::instance().endTransaction();

    // Check if the character can carry the item.
    if (!actor->canCarry(newItem))
    {
        actor->sendMsg("%s is too heavy, it will be left on the ground.\n", newItem->getName());
        actor->room->addItem(newItem);
    }
    else
    {
        actor->addInventoryItem(newItem);
    }
    // Update the condition of the involved objects.
    if (itemTarget->editCondition(-10))
    {
        actor->sendMsg("\nWith your last blow, %s falls down.\n", itemTarget->getName());
        // Set the list of exceptions.
        CharacterVector exceptions;
        exceptions.push_back(actor);
        // Send the message inside the room.
        actor->room->sendToAll("%s falls down.", exceptions, itemTarget->getNameCapital());
        // Destroy the vein.
        itemTarget->destroy();
    }
    // Check if the tool is destroyed.
    Item * tool = dynamic_cast<Item *>(usedTools.front());
    if (tool->editCondition(-1))
    {
        actor->sendMsg("%s falls into pieces.\n", tool->getName());
        tool->destroy();
    }

    // Advise character that he has ended the action.
    actor->sendMsg("\nYou have successfully cut some branches.\n\n");

    // Return the character in waiting status.
    this->reset();
}

void Action::performCraft()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    unsigned int consumedStamina;
    // Check if the actor has enough stamina to execute the action.
    if (!actor->hasStaminaFor(consumedStamina, ActionType::Crafting))
    {
        actor->sendMsg("You are too tired right now.\n");
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            actor->getName(),
            ToString(actor->getStamina()),
            ToString(consumedStamina));
        return;
    }
    // Consume the stamina.
    actor->remStamina(consumedStamina, true);

    // Check if the production is not a null pointer.
    if (production == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = Production is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Check the number of ingredients.
    if (usedIngredients.empty())
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = No ingredients.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    // Check the ingredients and the used tools.
    for (auto iterator : usedIngredients)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(
                LogLevel::Warning,
                actor->getName() + ":craft = One ingredient is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
    }
    for (auto iterator : usedTools)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(
                LogLevel::Warning,
                actor->getName() + ":craft = One tool is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
    }

    // Check the crafring material.
    if (craftMaterial == nullptr)
    {
        // Log a warning.
        Logger::log(
            LogLevel::Warning,
            actor->getName() + ":craft = Crafting material is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        return;
    }
    ItemVector toDestroy;
    for (auto it : usedTools)
    {
        // Update the condition of the involved objects.
        if (it->editCondition(-1))
        {
            actor->sendMsg("%s falls into pieces.\n", it->getName());
            toDestroy.push_back(it);
        }
    }
    for (auto it : usedIngredients)
    {
        toDestroy.push_back(it);
    }
    for (auto it : toDestroy)
    {
        it->destroy();
    }

    SQLiteDbms::instance().beginTransaction();
    ItemVector createdItems;
    for (unsigned int it = 0; it < production->outcome.second; ++it)
    {
        ItemModel * outcomeModel = production->outcome.first;
        // Create the item.
        Item * newItem = outcomeModel->createItem(
            actor->getName(),
            craftMaterial,
            ItemQuality::Normal);
        if (newItem == nullptr)
        {
            // Log a warning.
            Logger::log(
                LogLevel::Warning,
                actor->getName() + ":craft = New item is a null pointer.");
            // Rollback the database.
            SQLiteDbms::instance().rollbackTransection();
            // Delete all the items created so far.
            for (auto createdItem : createdItems)
            {
                delete (createdItem);
            }
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            return;
        }
        createdItems.push_back(newItem);
    }
    SQLiteDbms::instance().endTransaction();

    // Add the created items into the character inventory.
    bool dropped = false;
    for (auto createdItem : createdItems)
    {
        if (!actor->canCarry(createdItem))
        {
            actor->room->addItem(createdItem);
            dropped = true;
        }
        else
        {
            actor->addInventoryItem(createdItem);
        }
    }

    // Send conclusion message.
    actor->sendMsg(
        "%s %s.\n\n",
        production->profession->finishMessage,
        Formatter::yellow() + createdItems.back()->getName() + Formatter::reset());

    if (dropped)
    {
        actor->sendMsg(
            "Since you can't carry them, some of the items have been placed on the ground.\n\n");
    }

    // Return the character in waiting status.
    this->reset();
}

void Action::performBuild()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }

    unsigned int consumedStamina;
    // Check if the actor has enough stamina to execute the action.
    if (!actor->hasStaminaFor(consumedStamina, ActionType::Building))
    {
        actor->sendMsg("You are too tired right now.\n");
        Logger::log(
            LogLevel::Debug,
            "[%s] Has %s stamina and needs %s.",
            actor->getName(),
            ToString(actor->getStamina()),
            ToString(consumedStamina));
        return;
    }
    // Consume the stamina.
    actor->remStamina(consumedStamina, true);

    // Check the schematics.
    if (schematics == nullptr)
    {
        // Log a warning.
        Logger::log(LogLevel::Warning, actor->getName() + ":craft = Building is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Reset the action values.
        this->reset();
        return;
    }

    // Check the ingredients and used tools.
    for (auto iterator : usedIngredients)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(
                LogLevel::Warning,
                actor->getName() + ":craft = One ingredient is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            // Reset the action values.
            this->reset();
            return;
        }
    }
    for (auto iterator : usedTools)
    {
        if (iterator == nullptr)
        {
            // Log a warning.
            Logger::log(
                LogLevel::Warning,
                actor->getName() + ":craft = One tool is a null pointer.");
            // Notify the character.
            actor->sendMsg("\nYou have failed your action.\n");
            // Reset the action values.
            this->reset();
            return;
        }
    }

    ItemVector toDestroy;
    for (auto iterator : usedTools)
    {
        // Update the condition of the involved objects.
        if (iterator->editCondition(-1))
        {
            actor->sendMsg(iterator->getName() + " falls into pieces.");
            toDestroy.push_back(iterator);
        }
    }
    for (auto it : usedIngredients)
    {
        toDestroy.push_back(it);
    }
    for (auto it : toDestroy)
    {
        it->destroy();
    }

    if (schematics->buildingModel == nullptr)
    {
        // Log a warning.
        Logger::log(
            LogLevel::Warning,
            actor->getName() + ":craft = The building model is a null pointer.");
        // Notify the character.
        actor->sendMsg("\nYou have failed your action.\n");
        // Reset the action values.
        this->reset();
        return;
    }
    SQLiteDbms::instance().beginTransaction();
    actor->remInventoryItem(itemTarget);
    actor->room->addBuilding(itemTarget);
    itemTarget->updateOnDB();
    SQLiteDbms::instance().endTransaction();

    // Send conclusion message.
    actor->sendMsg(
        "You have finished building %s.\n\n",
        Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());

    // Return the character in waiting status.
    this->reset();
}

void Action::performComb()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return;
    }
    // The sequence is the following:
    //  1. Check the list of opponents.
    //  2. Get the target. It could eighter be the top aggro or an allied.
    //  3. Check if the target is at range (depending on the action).
    //  4. If not return to 2.
    //  5. Otherwise perform the action.

    // Check the list of opponents.
    actor->opponents.checkList();
    if (combatAction == CombatAction::BasicAttack)
    {
        // Perform the combat action.
        this->performCombatAction(combatAction);
    }
    else if (combatAction == CombatAction::Flee)
    {
        // Perform the combat action.
        this->performCombatAction(combatAction);
    }
    else
    {
        actor->sendMsg(this->stop());
    }
}

void Action::performCombatAction(const CombatAction & move)
{
    // Retrive once and for all the name of the actor.
    std::string nam = actor->getNameCapital();
    if (move == CombatAction::BasicAttack)
    {
        Logger::log(LogLevel::Debug, "[%s] Perform a BasicAttack.", nam);
        ItemVector activeWeapons = actor->getActiveWeapons();
        if (activeWeapons.empty())
        {
            actor->sendMsg("You do not have a valid weapon equipped.\n");
        }
        else
        {
            for (auto iterator : activeWeapons)
            {
                WeaponModel * weapon = iterator->model->toWeapon();
                // Get the top aggro enemy at range.
                Character * enemy = actor->getNextOpponentAtRange(weapon->range);
                if (enemy == nullptr)
                {
                    actor->sendMsg(
                        "You do not have opponents at reange for %s.\n",
                        iterator->getName());
                    continue;
                }
                // Will contain the required stamina.
                unsigned int consumedStamina;
                // Check if the actor has enough stamina to execute the action.
                if (!actor->hasStaminaFor(
                    consumedStamina,
                    ActionType::Combat,
                    CombatAction::BasicAttack,
                    iterator->getCurrentSlot()))
                {
                    actor->sendMsg("You are too tired to attack with %s.\n", iterator->getName());
                    Logger::log(
                        LogLevel::Debug,
                        "[%s] Has %s stamina and needs %s.",
                        nam,
                        ToString(actor->getStamina()),
                        ToString(consumedStamina));
                    continue;
                }
                // Consume the stamina.
                actor->remStamina(consumedStamina, true);
                // Natural roll for the attack.
                unsigned int ATK = TRandInteger<unsigned int>(1, 20);
                // Log the rolled value.
                Logger::log(LogLevel::Debug, "[%s] Natural roll of %s.", nam, ToString(ATK));
                // Check if:
                //  1. The number of active weapons is more than 1, then we have to apply
                //      a penality to the attack roll.
                //  2. The value is NOT a natural 20 (hit).
                if ((activeWeapons.size() > 1) && (ATK != 20))
                {
                    // Evaluate the penality to the attack roll.
                    unsigned int penality = 0;
                    // On the main hand the penality is 6.
                    if (iterator->currentSlot == EquipmentSlot::RightHand)
                    {
                        penality = 6;
                    }
                    // On the off hand the penality is 10.
                    else if (iterator->currentSlot == EquipmentSlot::LeftHand)
                    {
                        penality = 10;
                    }
                    // Log that we have applied a penality.
                    Logger::log(
                        LogLevel::Debug,
                        "[%s] Suffer a %s penalty with its %s.",
                        nam,
                        ToString(penality),
                        GetEquipmentSlotName(iterator->currentSlot));
                    // Safely apply the penality.
                    if (ATK < penality)
                    {
                        ATK = 0;
                    }
                    else
                    {
                        ATK -= penality;
                    }
                }
                // Evaluate the armor class of the enemy.
                unsigned int AC = enemy->getArmorClass();
                // Log the overall attack roll.
                Logger::log(
                    LogLevel::Debug,
                    "[%s] Attack roll %s vs %s.",
                    nam,
                    ToString(ATK),
                    ToString(AC));
                // Check if:
                //  1. The attack roll is lesser than the armor class of the opponent.
                //  2. The attack roll is not a natural 20.
                if ((ATK < AC) && (ATK != 20))
                {
                    actor->sendMsg(
                        "You miss %s with %s.\n\n",
                        enemy->getName(),
                        iterator->getName());
                    enemy->sendMsg("%s misses you with %s.\n\n", nam, iterator->getName());
                }
                else
                {
                    // Store the type of attack.
                    std::string critical;
                    // Natural roll for the damage.
                    unsigned int DMG = TRandInteger<unsigned int>(
                        weapon->minDamage,
                        weapon->maxDamage);
                    // Log the damage roll.
                    Logger::log(LogLevel::Debug, "[%s] Rolls a damage of %s.", nam, ToString(DMG));
                    // Check if the character is wielding a two-handed weapon.
                    if (activeWeapons.size() == 1)
                    {
                        if (HasFlag(iterator->model->modelFlags, ModelFlag::TwoHand))
                        {
                            // Get the strenth modifier.
                            unsigned int STR = actor->getAbilityModifier(Ability::Strength);
                            // Add to the damage rool one and half the strenth value.
                            DMG += STR + (STR / 2);
                            // Log the additional damage.
                            Logger::log(LogLevel::Debug, "[%s] Add 1-1/2 times its Strength.", nam);
                        }
                    }
                    // If the character has rolled a natural 20, then multiply the damage by two.
                    if (ATK == 20)
                    {
                        DMG *= 2;
                        critical = "critically";
                    }
                    // Log the damage.
                    Logger::log(
                        LogLevel::Debug,
                        "[%s] Hits %s for with %s.",
                        nam,
                        enemy->getName(),
                        ToString(DMG),
                        iterator->getName());
                    // Procede and remove the damage from the health of the target.
                    if (!enemy->remHealth(DMG))
                    {
                        actor->sendMsg(
                            "You %s hit %s with %s and kill %s.\n\n",
                            critical,
                            enemy->getName(),
                            iterator->getName(),
                            enemy->getObjectPronoun());
                        enemy->sendMsg(
                            "%s %s hits you with %s and kill you.\n\n",
                            nam,
                            critical,
                            iterator->getName());
                        // The enemy has received the damage and now it is dead.
                        enemy->kill();
                        continue;
                    }
                    else
                    {
                        // The enemy has received the damage but it is still alive.
                        actor->sendMsg(
                            "You %s hit %s with %s for %s.\n\n",
                            critical,
                            enemy->getName(),
                            iterator->getName(),
                            ToString(DMG));
                        enemy->sendMsg(
                            "%s %s hits you with %s for %s.\n\n",
                            nam,
                            critical,
                            iterator->getName(),
                            ToString(DMG));
                    }
                }
            }
        }
    }
    else if (move == CombatAction::Flee)
    {
        // Get the character chance of fleeing (D20).
        unsigned int fleeChance = TRandInteger<unsigned int>(0, 20);
        fleeChance += actor->getAbilityModifier(Ability::Agility);
        // Will contain the required stamina.
        unsigned int consumedStamina;
        // Base the escape level on how many enemies are surrounding the character.
        if (fleeChance < static_cast<unsigned int>(actor->opponents.getSize()))
        {
            actor->sendMsg("You were not able to escape from your attackers.\n");
        }
        // Check if the actor has enough stamina to execute the action.
        else if (!actor->hasStaminaFor(consumedStamina, ActionType::Combat, CombatAction::Flee))
        {
            actor->sendMsg("You are too tired to flee.\n");
            Logger::log(
                LogLevel::Debug,
                "[%s] Has %s stamina and needs %s.",
                nam,
                ToString(actor->getStamina()),
                ToString(consumedStamina));
        }
        else
        {
            // Consume the stamina.
            actor->remStamina(consumedStamina, true);
            // Get the list of available directions.
            std::vector<Direction> directions = actor->room->getAvailableDirections();
            // Check if there are some directions.
            if (!directions.empty())
            {
                // Pick a random direction, from the poll of the available ones.
                Direction randomDirection = directions.at(
                    TRandInteger<size_t>(0, directions.size() - 1));
                // Get the selected exit.
                std::shared_ptr<Exit> selected = actor->room->findExit(randomDirection);
                // Check that the picked exit is not a null pointer.
                if (selected == nullptr)
                {
                    Logger::log(LogLevel::Error, "Selected null exit during action Flee.");
                    actor->sendMsg("You were not able to escape from your attackers.\n");
                }
                else
                {
                    // Stop the current action.
                    actor->sendMsg(this->stop());
                    // Move the actor to the random direction.
                    actor->moveTo(
                        selected->destination,
                        nam + " flees from the battlefield.\n\n",
                        nam + " arives fleeing.\n\n",
                        "You flee from the battlefield.\n");
                    return;
                }
            }
        }
    }
    // By default set the next combat action to basic attack.
    if (!this->setNextCombatAction(CombatAction::BasicAttack))
    {
        actor->sendMsg(this->stop() + "\n\n");
    }
}

std::string GetActionTypeName(ActionType type)
{
    if (type == ActionType::Wait) return "Waiting";
    if (type == ActionType::Move) return "Moving";
    if (type == ActionType::Crafting) return "Crafting";
    if (type == ActionType::Building) return "Building";
    if (type == ActionType::Combat) return "Fighting";
    return "No Action";
}
