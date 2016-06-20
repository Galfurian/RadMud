/// @file   comm_gods.cpp
/// @brief  Implements the methods used by <b>gods</b>.
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

// Basic Include.
#include <istream>

#include "commands.hpp"
#include "constants.hpp"
#include "defines.hpp"
#include "mud.hpp"
// Other Include.
#include "utilities/table.hpp"

using namespace std;

void DoShutdown(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Send message to all the players.
    Mud::instance().broadcastMsg(0, character->getNameCapital() + " has shut down the game!");
    Mud::instance().shutDownSignal();
}

void DoGoTo(Character * character, std::istream & sArgs)
{
    std::string msgDepart, msgArrive, msgChar;
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You have to provide a room vnum.");
        return;
    }
    Room * destination = Mud::instance().findRoom(ToInt(arguments[0].first));
    if (destination == nullptr)
    {
        character->sendMsg("That room doesen't exists.\n");
        return;
    }

    // Check if the player it's already doing something.
    StopAction(character);
    // Prepare messages.
    msgDepart = character->getNameCapital();
    msgDepart += " disappears in a puff of smoke!\n";
    msgArrive = character->getNameCapital();
    msgArrive += " appears in a puff of smoke!\n";
    msgChar = "You go to room " + destination->name + ".\n";
    // Move player.
    character->moveTo(destination, msgDepart, msgArrive, msgChar);
}

void DoTransfer(Character * character, std::istream & sArgs)
{
    std::string msgDepart, msgArrive, msgChar;
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if ((arguments.size() != 1) && (arguments.size() != 2))
    {
        character->sendMsg("You have to select at least a target and optionally a destination.\n");
        return;
    }
    Character * target = nullptr;
    if (arguments.size() >= 1)
    {
        target = Mud::instance().findPlayer(arguments[0].first);
        if (target == nullptr)
        {
            target = Mud::instance().findMobile(arguments[0].first);
            if (target == nullptr)
            {
                character->sendMsg("Can't find the target character.\n");
                return;
            }
        }
    }
    Room * destination = character->room;
    if (arguments.size() == 2)
    {
        destination = character->room->area->getRoom(ToInt(arguments[1].first));
        if (destination == nullptr)
        {
            character->sendMsg("Can't find the target room, setting the current room.\n");
            return;
        }
    }
    // Check if the player it's already doing something.
    StopAction(target);
    // Prepare messages.
    msgDepart = target->getNameCapital() + " is yanked away by unseen forces!";
    msgArrive = target->getNameCapital() + " appears breathlessly!";
    msgChar = "\n" + character->getNameCapital();
    msgChar += " transfers you to another room!\n";
    // Move player.
    target->moveTo(destination, msgDepart, msgArrive, msgChar);
    character->sendMsg("You transfer %s to room %s.\n", target->getName(), destination->name);
}

void DoGodInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a valide player name.\n");
        return;
    }
    Character * target = Mud::instance().findPlayer(arguments[0].first);
    if (target == nullptr)
    {
        target = Mud::instance().findMobile(arguments[0].first);
        if (target == nullptr)
        {
            std::string msgFound;
            for (auto it : Mud::instance().mudMobiles)
            {
                if (BeginWith(it.first, arguments[0].first))
                {
                    if (msgFound.empty())
                    {
                        msgFound += "Maybe you mean:\n";
                    }
                    msgFound += "    " + it.first + "\n";
                }
            }
            character->sendMsg("Mobile not found.\n" + msgFound);
            return;
        }
    }
    std::string msg;
    msg += Formatter::green() + "# Chracter:\n" + Formatter::reset();
    msg += Formatter::yellow() + " Proper Noun  " + Formatter::reset() + ":" + target->name + "\n";
    msg += Formatter::yellow() + "    Desc    " + Formatter::reset() + ":" + target->description + "\n";
    msg += Formatter::yellow() + "    Gender  " + Formatter::reset() + ":" + GetGenderTypeName(target->gender) + "\n";
    msg += Formatter::yellow() + "    Weight  " + Formatter::reset() + ":" + ToString(target->weight) + "\n";
    msg += Formatter::yellow() + "    Level   " + Formatter::reset() + ":" + ToString(target->level) + "\n";
    msg += Formatter::yellow() + "    Flags   " + Formatter::reset() + ":" + ToString(target->flags) + "\n";
    msg += Formatter::yellow() + "    Race    " + Formatter::reset() + ":" + target->race->name + "\n";
    msg += Formatter::yellow() + "    Faction " + Formatter::reset() + ":" + target->faction->name + "\n";
    msg += Formatter::yellow() + "    Health  " + Formatter::reset() + ":" + ToString(target->getHealth()) + "\n";
    msg += Formatter::yellow() + "    Stamina " + Formatter::reset() + ":" + ToString(target->getStamina()) + "\n";
    msg += Formatter::yellow() + "    Hunger  " + Formatter::reset() + ":" + ToString(target->getHunger()) + "\n";
    msg += Formatter::yellow() + "    Thirst  " + Formatter::reset() + ":" + ToString(target->getThirst()) + "\n";
    msg += Formatter::yellow() + "    STR     " + Formatter::reset() + ":";
    msg += ToString(target->getAbility(Ability::Strength, false));
    msg += "[" + ToString(target->effects.getAbilityModifier(Ability::Strength)) + "]\n";
    msg += Formatter::yellow() + "    AGI     " + Formatter::reset() + ":";
    msg += ToString(target->getAbility(Ability::Agility, false));
    msg += "[" + ToString(target->effects.getAbilityModifier(Ability::Agility)) + "]\n";
    msg += Formatter::yellow() + "    PER     " + Formatter::reset() + ":";
    msg += ToString(target->getAbility(Ability::Perception, false));
    msg += "[" + ToString(target->effects.getAbilityModifier(Ability::Perception)) + "]\n";
    msg += Formatter::yellow() + "    CON     " + Formatter::reset() + ":";
    msg += ToString(target->getAbility(Ability::Constitution, false));
    msg += "[" + ToString(target->effects.getAbilityModifier(Ability::Constitution)) + "]\n";
    msg += Formatter::yellow() + "    INT     " + Formatter::reset() + ":";
    msg += ToString(target->getAbility(Ability::Intelligence, false));
    msg += "[" + ToString(target->effects.getAbilityModifier(Ability::Intelligence)) + "]\n";
    msg += Formatter::yellow() + "    Room    " + Formatter::reset() + ":" + ToString(target->room->vnum) + "\n";
    msg += Formatter::yellow() + "    Posture " + Formatter::reset() + ":" + GetPostureName(target->posture) + "\n";
    msg += Formatter::yellow() + "    Inv     " + Formatter::reset() + ":\n";
    for (auto it : target->inventory)
    {
        msg += "        " + it->getName() + "\n";
    }
    msg += Formatter::yellow() + "    Equip   " + Formatter::reset() + ":\n";
    for (auto it : target->equipment)
    {
        msg += "        " + it->getName() + "\n";
    }
    msg += Formatter::yellow() + "    Effects " + Formatter::reset() + ":\n";
    for (auto it : target->effects.activeEffects)
    {
        msg += "                [" + ToString(it.expires) + "] " + it.name + "\n";
    }
    msg += Formatter::yellow() + "    Action  " + Formatter::reset() + ":"
        + GetActionTypeName(target->getAction()->getType()) + "\n";

    if (target->isPlayer())
    {
        Player * player = target->toPlayer();
        msg += Formatter::yellow() + "    Age     " + Formatter::reset() + ":" + ToString(player->age) + "\n";
        msg += Formatter::yellow() + "    Exp     " + Formatter::reset() + ":" + ToString(player->experience) + "\n";
        msg += Formatter::yellow() + "    Prompt  " + Formatter::reset() + ":" + player->prompt + "\n";
        msg += Formatter::yellow() + "    Rent    " + Formatter::reset() + ":" + ToString(player->rent_room) + "\n";
        msg += Formatter::yellow() + "    Skills  " + Formatter::reset() + ":\n";
        for (auto iterator : Mud::instance().mudSkills)
        {
            msg += "        " + iterator.second->name + "[" + ToString(player->skills[iterator.first]) + "]\n";
        }
    }
    else if (target->isMobile())
    {
        Mobile * m = target->toMobile();
        msg += Formatter::yellow() + "    Id      " + Formatter::reset() + ":" + m->id + "\n";
        msg += Formatter::yellow() + "    Spawn   " + Formatter::reset() + ":" + ToString(m->respawnRoom->vnum) + "\n";
        msg += Formatter::yellow() + "    Keys    " + Formatter::reset() + ": {";
        for (auto it : m->keys)
        {
            msg += " " + it;
        }
        msg += "}\n";
        msg += Formatter::yellow() + "    Short   " + Formatter::reset() + ":" + m->shortdesc + "\n";
        msg += Formatter::yellow() + "    Static  " + Formatter::reset() + ":" + m->staticdesc + "\n";
        msg += Formatter::yellow() + "    Actions " + Formatter::reset() + ": {";
        for (auto it : m->actions)
        {
            msg += " " + it;
        }
        msg += "}\n";
        msg += Formatter::yellow() + "    Alive   " + Formatter::reset() + ":" + ToString(m->alive) + "\n";
        if (!m->alive)
        {
            msg += Formatter::yellow() + "    Respawn " + Formatter::reset() + ":" + ToString(m->getRespawnTime())
                + "\n";
        }
        if (m->controller != nullptr)
        {
            msg += Formatter::red() + "    Controller " + Formatter::reset() + ":" + m->controller->getName() + "\n";
        }
        msg += Formatter::red() + "    Lua Script   " + Formatter::reset() + ":" + m->lua_script + "\n";
    }
    character->sendMsg(msg);
}

void DoSetFlag(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return;
    }
    Player * target = Mud::instance().findPlayer(arguments[0].first);
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '" + arguments[0].first + "'.\n");
        return;
    }
    CharacterFlag flag = static_cast<CharacterFlag>(ToInt(arguments[1].first));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag already set.\n");
        return;
    }
    // Set the flag.
    SetFlag(target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You set the flag '%s' for %s\n", GetCharacterFlagName(flag), target->getName());
}

void DoClearFlag(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 2)
    {
        character->sendMsg("You must provide a target and a flag.");
        return;
    }
    Player * target = Mud::instance().findPlayer(arguments[0].first);
    if (target == nullptr)
    {
        character->sendMsg("You can't find the player '" + arguments[0].first + "'.\n");
        return;
    }
    CharacterFlag flag = static_cast<CharacterFlag>(ToInt(arguments[1].first));
    if (flag == CharacterFlag::None)
    {
        character->sendMsg("You must insert a valid flag.\n");
        return;
    }
    // Check if the character already has the flag.
    if (HasFlag(target->flags, flag))
    {
        character->sendMsg("Flag not set.\n");
        return;
    }
    // Set the flag.
    ClearFlag(target->flags, flag);
    // Send confirmation to the player.
    character->sendMsg("You clear the flag '%s' for %s\n", GetCharacterFlagName(flag), target->getName());
}

void DoModelInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a model vnum.\n");
        return;
    }
    Model * model = Mud::instance().findModel(ToInt(arguments[0].first));
    if (model == nullptr)
    {
        character->sendMsg("Model not found.\n");
        return;
    }

    std::string msg;
    msg += Formatter::yellow() + "Model Vnum    " + Formatter::reset();
    msg += ": " + ToString(model->vnum) + "\n";
    msg += Formatter::yellow() + "Name          " + Formatter::reset();
    msg += ": " + model->name + "\n";
    msg += Formatter::yellow() + "Article       " + Formatter::reset();
    msg += ": " + model->article + "\n";
    msg += Formatter::yellow() + "Short Descr.  " + Formatter::reset();
    msg += ": " + model->shortdesc + "\n";
    msg += Formatter::yellow() + "Keys          " + Formatter::reset();
    msg += ": {";
    for (auto it : model->keys)
    {
        msg += " " + it;
    }
    msg += "}\n";
    msg += Formatter::yellow() + "Description   " + Formatter::reset();
    msg += ": " + model->description + "\n";
    msg += Formatter::yellow() + "Type          " + Formatter::reset();
    msg += ": " + GetModelTypeName(model->type) + "\n";
    msg += Formatter::yellow() + "Slot          " + Formatter::reset();
    msg += ": " + GetEquipmentSlotName(model->slot) + "\n";
    msg += Formatter::yellow() + "Flags         " + Formatter::reset();
    msg += ": " + GetModelFlagString(model->flags) + "\n";
    msg += Formatter::yellow() + "Weight        " + Formatter::reset();
    msg += ": " + ToString(model->weight) + " " + mud_measure + ".\n";
    msg += Formatter::yellow() + "Price         " + Formatter::reset();
    msg += ": " + ToString(model->price) + ".\n";
    msg += Formatter::yellow() + "Condition     " + Formatter::reset();
    msg += ": " + ToString(model->condition) + "\n";
    msg += Formatter::yellow() + "Decay         " + Formatter::reset();
    msg += ": " + ToString(model->decay) + "\n";
    msg += Formatter::yellow() + "Material      " + Formatter::reset();
    msg += ": " + GetMaterialTypeName(model->material) + "\n";
    msg += Formatter::yellow() + "Tile Set      " + Formatter::reset();
    msg += ": " + ToString(model->tileSet) + "\n";
    msg += Formatter::yellow() + "Tile Id       " + Formatter::reset();
    msg += ": " + ToString(model->tileId) + "\n";
    msg += Formatter::yellow() + "Functions     " + Formatter::reset();
    msg += ": {";
    for (auto it : model->functions)
    {
        msg += " " + ToString(it);
    }
    msg += "}\n";
    if (model->type == ModelType::Weapon)
    {
        WeaponFunc func = model->getWeaponFunc();
        msg += Formatter::brown() + "Type          " + Formatter::reset();
        msg += ": " + GetWeaponTypeName(func.type) + "\n";
        msg += Formatter::brown() + "Minimum Damage  " + Formatter::reset();
        msg += ": " + ToString(func.minDamage) + "\n";
        msg += Formatter::brown() + "Maximum Damage  " + Formatter::reset();
        msg += ": " + ToString(func.maxDamage) + "\n";
        msg += Formatter::brown() + "Range           " + Formatter::reset();
        msg += ": " + ToString(func.range) + "\n";
    }
    else if (model->type == ModelType::Armor)
    {
        ArmorFunc func = model->getArmorFunc();
        msg += Formatter::brown() + "Size          " + Formatter::reset();
        msg += ": " + GetArmorSizeName(func.size) + "\n";
        msg += Formatter::brown() + "Damage Abs.   " + Formatter::reset();
        msg += ": " + ToString(func.damageAbs) + "\n";
        msg += Formatter::brown() + "Allowed Anatom. " + Formatter::reset();
        msg += ": " + ToString(func.allowedAnatomy) + "\n";
    }
    else if (model->type == ModelType::Shield)
    {
        ShieldFunc func = model->getShieldFunc();
        msg += Formatter::brown() + "Size            " + Formatter::reset();
        msg += ": " + GetShieldSizeName(func.size) + "\n";
        msg += Formatter::brown() + "Parry Chance    " + Formatter::reset();
        msg += ": " + ToString(func.parryChance) + "\n";
    }
    else if (model->type == ModelType::Projectile)
    {
        ProjectileFunc func = model->getProjectileFunc();
        msg += Formatter::brown() + "Damage Bonus    " + Formatter::reset();
        msg += ": " + ToString(func.damageBonus) + "\n";
        msg += Formatter::brown() + "Range Bonus     " + Formatter::reset();
        msg += ": " + ToString(func.rangeBonus) + "\n";
    }
    else if (model->type == ModelType::Container)
    {
        ContainerFunc func = model->getContainerFunc();
        msg += Formatter::brown() + "Max Weight      " + Formatter::reset();
        msg += ": " + ToString(func.maxWeight) + "\n";
        msg += Formatter::brown() + "Flags           " + Formatter::reset();
        msg += ": {" + GetContainerFlagString(func.flags) + "}\n";
        msg += Formatter::brown() + "Key Vnum        " + Formatter::reset();
        msg += ": " + ToString(func.keyVnum) + "\n";
        msg += Formatter::brown() + "Lockpicking Lv. " + Formatter::reset();
        msg += ": " + ToString(func.difficulty) + "\n";
    }
    else if (model->type == ModelType::LiqContainer)
    {
        LiqContainerFunc func = model->getLiqContainerFunc();
        msg += Formatter::brown() + "Max Weight      " + Formatter::reset();
        msg += ": " + ToString(func.maxWeight) + "\n";
        msg += Formatter::brown() + "Flags           " + Formatter::reset();
        msg += ": {" + GetLiqContainerFlagString(func.flags) + "}\n";
    }
    else if (model->type == ModelType::Tool)
    {
        ToolFunc func = model->getToolFunc();
        msg += Formatter::brown() + "Type            " + Formatter::reset();
        msg += ": " + GetToolTypeName(func.type) + "\n";
    }
    else if (model->type == ModelType::Node)
    {
        NodeFunc func = model->getNodeFunc();
        msg += Formatter::brown() + "Type            " + Formatter::reset();
        msg += ": " + GetNodeTypeName(func.type) + "\n";
    }
    else if (model->type == ModelType::Resource)
    {
        ResourceFunc func = model->getResourceFunc();
        msg += Formatter::brown() + "Type            " + Formatter::reset();
        msg += ": " + GetResourceTypeName(func.type) + "\n";
    }
    else if (model->type == ModelType::Seed)
    {
        SeedFunc func = model->getSeedFunc();
        msg += Formatter::brown() + "Type            " + Formatter::reset();
        msg += ": " + GetSeedTypeName(func.type) + "\n";
    }
    else if (model->type == ModelType::Food)
    {
        FoodFunc func = model->getFoodFunc();
        msg += Formatter::brown() + "Feeding         " + Formatter::reset();
        msg += ": " + ToString(func.hours) + "\n";
        msg += Formatter::brown() + "Flags           " + Formatter::reset();
        msg += ": {" + GetFoodFlagString(func.flags) + "}\n";
    }
    else if (model->type == ModelType::Light)
    {
        LightFunc func = model->getLightFunc();
        msg += Formatter::brown() + "Autonomy        " + Formatter::reset();
        msg += ": " + ToString(func.maxHours) + "\n";
        msg += Formatter::brown() + "Rechargeable    " + Formatter::reset();
        msg += ": " + ToString(func.policy) + "\n";
    }
    else if (model->type == ModelType::Book)
    {
        BookFunc func = model->getBookFunc();
        msg += Formatter::brown() + "Capacity        " + Formatter::reset();
        msg += ": " + ToString(func.maxParchments) + "\n";
    }
    else if (model->type == ModelType::Rope)
    {
        RopeFunc func = model->getRopeFunc();
        msg += Formatter::brown() + "Difficulty      " + Formatter::reset();
        msg += ": " + ToString(func.difficulty) + "\n";
        msg += Formatter::brown() + "Type            " + Formatter::reset();
        msg += ": " + ToString(func.type) + "\n";
    }
    else if (model->type == ModelType::Mechanism)
    {
        MechanismFunc func = model->getMechanismFunc();
        msg += Formatter::brown() + "Type            " + Formatter::reset();
        msg += ": " + GetMechanismTypeName(func.type) + "\n";
        if ((func.type == MechanismType::Door) || (func.type == MechanismType::Lock))
        {
            msg += Formatter::brown() + "Key             " + Formatter::reset();
            msg += ": " + ToString(func.key) + "\n";
            msg += Formatter::brown() + "Difficulty      " + Formatter::reset();
            msg += ": " + ToString(func.difficulty) + "\n";
        }
        else if (func.type == MechanismType::Picklock)
        {
            msg += Formatter::brown() + "Efficency       " + Formatter::reset();
            msg += ": " + ToString(func.efficency) + "\n";
        }
        else if (func.type == MechanismType::Lever)
        {
            msg += Formatter::brown() + "Command         " + Formatter::reset();
            msg += ": " + ToString(func.command) + "\n";
            msg += Formatter::brown() + "Target          " + Formatter::reset();
            msg += ": " + ToString(func.target) + "\n";
        }
        else
        {
            msg += Formatter::brown() + "None" + Formatter::reset() + "\n";
        }
    }
    else
    {
        msg += Formatter::brown() + "No Type" + Formatter::reset() + "\n";
    }
    character->sendMsg(msg);
}

void DoItemCreate(Character * character, std::istream & sArgs)
{
    // Prevent mobiles to execute this command.
    NoMobile(character);
    // Stop any ongoing action.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if ((arguments.size() != 2) && (arguments.size() != 3))
    {
        character->sendMsg("What do you want to create?\n");
        return; // Skip the rest of the function.
    }
    Model * model = Mud::instance().findModel(ToInt(arguments[0].first));
    Material * material = Mud::instance().findMaterial(ToInt(arguments[1].first));
    ItemQuality quality = ItemQuality::Normal;
    if (model == nullptr)
    {
        character->sendMsg("Cannot find model '%s'.\n", arguments[0].first);
        return;
    }
    if (material == nullptr)
    {
        character->sendMsg("Cannot find material '%s'.\n", arguments[1].first);
        return;
    }
    if (arguments.size() == 3)
    {
        quality = static_cast<ItemQuality>(ToNumber<unsigned int>(arguments[2].first));
        if (quality == ItemQuality::None)
        {
            character->sendMsg("Not a valid quality.\n");
            return;
        }
    }
    // Create the item.
    Item * item = model->createItem(character->getName(), material, quality);
    if (item == nullptr)
    {
        character->sendMsg("Creation failed.\n");
        return;
    }
    character->addInventoryItem(item);
    character->sendMsg(
        "You produce '%s' out of your apparently empty top hat.\n",
        Formatter::yellow() + item->getName() + Formatter::reset());
}

void DoItemGet(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return;
    }
    int itemVnum = ToNumber<int>(arguments[0].first);
    Item * item = Mud::instance().findItem(itemVnum);
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum (%s).\n", ToString(itemVnum));
        return;
    }
    // Check if the God player can carry the item.
    if (!character->canCarry(item))
    {
        character->sendMsg("You can't carry anymore items.\n");
        return;
    }
    if (item->room != nullptr)
    {
        character->sendMsg("The item was inside the room '%s' (%s)\n", item->room->name, ToString(item->room->vnum));
        item->room->removeItem(item);
        item->updateOnDB();
    }
    else if (item->owner != nullptr)
    {
        character->sendMsg("The item was possessed by '%s'\n", item->owner->getName());
        FindErase(item->owner->inventory, item);
        FindErase(item->owner->equipment, item);
        item->owner = nullptr;
        item->updateOnDB();
    }
    else if (item->container != nullptr)
    {
        character->sendMsg("The item was inside the container '%s'\n", item->container->getName());
        item->container->takeOut(item);
        item->updateOnDB();
    }
    else
    {
        character->sendMsg("The item was nowhere!");
    }
    character->sendMsg("You materialize the desired object in your hands.\n");
    character->addInventoryItem(item);
    item->updateOnDB();
}

void DoItemDestroy(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert an item vnum.\n");
        return;
    }
    Item * item = Mud::instance().findItem(ToInt(arguments[0].first));
    if (item == nullptr)
    {
        character->sendMsg("Invalid vnum.\n");
        return;
    }
    item->destroy();
    character->sendMsg("You have tryed to destroy the desired object.\n");
}

void DoItemInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert the item vnum or the name of the item inside the room.\n");
        return;
    }
    Item * item = Mud::instance().findItem(ToInt(arguments[0].first));
    if (item == nullptr)
    {
        item = character->findNearbyItem(arguments[0].first, arguments[0].second);
        if (item == nullptr)
        {

            character->sendMsg("Cannot find the target item (provide vnum or item name).\n");
            return;
        }
    }
    std::string msg;
    msg += "Vnum         : " + ToString(item->vnum) + "\n";
    msg += "Model        : [" + ToString(item->model->vnum) + "] " + item->getName() + ".\n";
    msg += "Type         : " + GetModelTypeName(item->model->type) + "\n";
    msg += "Maker        : " + item->maker + "\n";
    msg += "Condition    : " + ToString(item->condition) + " of " + ToString(item->model->condition) + "\n";
    msg += "Composition  : " + item->composition->name + " [" + ToString(item->composition->vnum) + "]\n";
    msg += "Quality      : " + GetItemQualityName(item->quality) + "\n";
    if (item->room != nullptr)
    {
        msg += "Room         : " + item->room->name + " [" + ToString(item->room->vnum) + "]\n";
    }
    else if (item->owner != nullptr)
    {
        msg += "Owner        : " + item->owner->getName() + "\n";
    }
    else if (item->container != nullptr)
    {
        msg += "Inside       : " + item->container->getName() + " [" + ToString(item->container->vnum) + "]\n";
    }
    if (!item->content.empty())
    {
        msg += "Content      :\n";
        for (auto iterator : item->content)
        {
            msg += "             * " + iterator->getName() + " [" + ToString(iterator->vnum) + "]\n";
        }
    }
    if (item->contentLiq.first != nullptr)
    {
        msg += "Content Liq. : ";
        msg += item->contentLiq.first->getNameCapital() + " [" + ToString(item->contentLiq.second) + "]\n";
    }
    msg += "Current Slot : " + item->getCurrentSlotName() + "\n";
    // Send the formatted message.
    character->sendMsg(msg);
}

void DoAreaInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must instert an area vnum.\n");
        return;
    }
    Area * area = Mud::instance().findArea(ToInt(arguments[0].first));
    if (area == nullptr)
    {
        character->sendMsg("The selected area does not exist.");
        return;
    }
    std::string msg;
    msg += Formatter::green() + "# Area Informations:\n" + Formatter::reset();
    msg += Formatter::yellow() + " Vnum            " + Formatter::reset() + ":" + ToString(area->vnum) + "\n";
    msg += Formatter::yellow() + " Name            " + Formatter::reset() + ":" + area->name + "\n";
    msg += Formatter::yellow() + " Builder         " + Formatter::reset() + ":" + area->builder + "\n";
    msg += Formatter::yellow() + " Continent       " + Formatter::reset() + ":" + area->continent + "\n";
    msg += Formatter::yellow() + " Width           " + Formatter::reset() + ":" + ToString(area->width) + "\n";
    msg += Formatter::yellow() + " Height          " + Formatter::reset() + ":" + ToString(area->height) + "\n";
    msg += Formatter::yellow() + " Elevation       " + Formatter::reset() + ":" + ToString(area->elevation) + "\n";
    msg += Formatter::yellow() + " Number of Rooms " + Formatter::reset() + ":" + ToString(area->areaMap.size()) + "\n";
    character->sendMsg(msg);
}

void DoRoomInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    Room * room;
    if (arguments.empty())
    {
        room = character->room;
    }
    else if (arguments.size() == 1)
    {
        room = Mud::instance().findRoom(ToInt(arguments[0].first));
        if (room == nullptr)
        {
            character->sendMsg("Can't find the desired room.\n");
            return;
        }
    }
    else
    {
        character->sendMsg("CanYu can only provide nothing or a room vnum.\n");
        return;
    }
    std::string msg;
    msg += Formatter::green() + "# Room Informations:\n" + Formatter::reset();
    msg += Formatter::yellow() + " Vnum        " + Formatter::reset() + ":" + ToString(room->vnum) + "\n";
    msg += Formatter::yellow() + " X           " + Formatter::reset() + ":" + ToString(room->coord.x) + "\n";
    msg += Formatter::yellow() + " Y           " + Formatter::reset() + ":" + ToString(room->coord.y) + "\n";
    msg += Formatter::yellow() + " Z           " + Formatter::reset() + ":" + ToString(room->coord.z) + "\n";
    msg += Formatter::yellow() + " Name        " + Formatter::reset() + ":" + room->name + "\n";
    msg += Formatter::yellow() + " Description " + Formatter::reset() + ":" + room->description + "\n";
    msg += Formatter::yellow() + " Terrain     " + Formatter::reset() + ":" + room->terrain + "\n";
    msg += Formatter::yellow() + " Flags       " + Formatter::reset() + ":" + GetRoomFlagString(room->flags) + "\n";
    character->sendMsg(msg);
}

void DoRoomCreate(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return; // Skip the rest of the function.
    }
    Room * currentRoom = character->room;
    Area * currentArea = currentRoom->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return; // Skip the rest of the function.
    }
    // Check if it's a direction.
    Direction direction = Mud::instance().findDirection(arguments[0].first, false);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return; // Skip the rest of the function.
    }
    // Get the coordinate modifier.
    Coordinates<int> targetCoord = currentRoom->coord + GetCoordinates(direction);
    if (!currentArea->inBoundaries(targetCoord))
    {
        character->sendMsg("Sorry but in that direction you will go outside the boundaries.\n");
        return; // Skip the rest of the function.
    }
    // Find the room.
    Room * targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom)
    {
        character->sendMsg("Sorry but in that direction there is already a room.\n");
        return; // Skip the rest of the function.
    }
    if (!CreateRoom(targetCoord, currentRoom))
    {
        character->sendMsg("Sorry but you couldn't create the room.\n");
        return; // Skip the rest of the function.
    }
    character->sendMsg("You have created a room at: %s\n", targetCoord.toString());
}

void DoRoomDelete(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 1)
    {
        character->sendMsg("Usage: [direction]\n");
        return; // Skip the rest of the function.
    }
    Area * currentArea = character->room->area;
    if (currentArea == nullptr)
    {
        character->sendMsg("Your room's area has not been defined!\n");
        return; // Skip the rest of the function.
    }
    // Check if it's a direction.
    Direction direction = Mud::instance().findDirection(arguments[0].first, false);
    if (direction == Direction::None)
    {
        character->sendMsg("You must insert a valid direction!\n");
        return; // Skip the rest of the function.
    }
    // Get the coordinate modifier.
    Coordinates<int> targetCoord = character->room->coord + GetCoordinates(direction);
    if (!currentArea->inBoundaries(targetCoord))
    {
        character->sendMsg("Sorry but in that direction you will go outside the boundaries.\n");
        return; // Skip the rest of the function.
    }
    // Find the room.
    Room * targetRoom = currentArea->getRoom(targetCoord);
    if (targetRoom == nullptr)
    {
        character->sendMsg("Sorry but in that direction there is no room.\n");
        return; // Skip the rest of the function.
    }
    // Remove the room from the Database.
    if (!targetRoom->removeOnDB())
    {
        character->sendMsg("Sorry but you couldn't delete the selected room.\n");
        character->sendMsg("Probably there are items or characters in that room.\n");
        return; // Skip the rest of the function.
    }
    // Remove the room from the list of rooms.
    if (!Mud::instance().remRoom(targetRoom))
    {
        character->sendMsg("You cannot remove the room.\n");
        return; // Skip the rest of the function.
    }

    // Delete completely the room.
    delete (targetRoom);

    character->sendMsg("You have destroyed the room at coordinates :\n");
    std::string msg = "    [";
    msg += ToString(targetCoord.x) + ";";
    msg += ToString(targetCoord.y) + ";";
    msg += ToString(targetCoord.z) + "]\n";
    character->sendMsg(msg);
}

void DoRoomEdit(Character * character, std::istream & sArgs)
{
    NoMobile(character);
    Player * player = character->toPlayer();

    string input;
    Room * room = player->room;

    sArgs >> input >> ws;

    if (input == "des")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("You can't set an empty description.\n");
            return;
        }

        QueryList value =
        { std::make_pair("description", input) };
        QueryList where =
        { std::make_pair("vnum", ToString(room->vnum)) };

        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            player->sendMsg("Command gone wrong.\n");
            return;
        }

        room->description = input;
        player->sendMsg("Room description modified.\n");
    }
    else if (input == "ter")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("You can't set an empty terrain.\n");
            return;
        }
        QueryList value =
        { std::make_pair("terrain", input) };
        QueryList where =
        { std::make_pair("vnum", ToString(room->vnum)) };

        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            player->sendMsg("Command gone wrong.\n");
            return;
        }

        room->terrain = input;
        player->sendMsg("Room terrain modified.\n");
    }
    else if (input == "nam")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("You can't set an empty name.\n");
            return;
        }

        QueryList value =
        { std::make_pair("name", input) };
        QueryList where =
        { std::make_pair("vnum", ToString(room->vnum)) };
        if (!SQLiteDbms::instance().updateInto("Room", value, where))
        {
            player->sendMsg("Command gone wrong.\n");
            return;
        }

        room->name = input;
        player->sendMsg("Room name modified.\n");
    }
    else if (input == "sflag")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("Wrong flag.\n");
            return;
        }
        if (input == "R")
        {
            SetFlag(room->flags, RoomFlag::Rent);
        }
        else if (input == "P")
        {
            SetFlag(room->flags, RoomFlag::Peaceful);
        }
        else
        {
            player->sendMsg("Not a valid flag.\n");
        }
    }
    else if (input == "cflag")
    {
        getline(sArgs, input);
        if (input.empty())
        {
            player->sendMsg("Wrong flag.\n");
            return;
        }
        if (input == "R")
        {
            ClearFlag(room->flags, RoomFlag::Rent);
        }
        else if (input == "P")
        {
            ClearFlag(room->flags, RoomFlag::Peaceful);
        }
        else
        {
            player->sendMsg("Not a valid flag.\n");
        }
    }
    else
    {
        player->sendMsg("Usage:                           \n");
        player->sendMsg("|  Argument  |  Text            |\n");
        player->sendMsg("| ---------- | ---------------- |\n");
        player->sendMsg("|  nam       |  [name]          |\n");
        player->sendMsg("|  ter       |  [terrain]       |\n");
        player->sendMsg("|  des       |  [description]   |\n");
        player->sendMsg("|  sflag     |  [Set Flag]      |\n");
        player->sendMsg("|            |   R -> kRent     |\n");
        player->sendMsg("|            |   P -> kPeaceful |\n");
        player->sendMsg("|  cflag     |  [Clear Flag]    |\n");
        player->sendMsg("|            |   R -> kRent     |\n");
        player->sendMsg("|            |   P -> kPeaceful |\n");
        player->sendMsg("| ---------- | ---------------- |\n");
        return;
    }
}

void DoMobileKill(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return;
    }
    Mobile * mobile = character->room->findMobile(arguments[0].first, arguments[0].second);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    // Kill the mob.
    mobile->triggerDeath();
    // Notify the death.
    character->sendMsg("You snap your fingers.\n");
    character->room->sendToAll("%s fall to the ground dead.", CharacterVector(), mobile->getNameCapital());
}

void DoMobileReload(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a target mobile.\n");
        return;
    }
    Mobile * mobile = character->room->findMobile(arguments[0].first, arguments[0].second);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    // Reload the mob.
    mobile->reloadLua();
    // Notify.
    character->sendMsg("Target(%s) Script(%s)\n", mobile->getName(), mobile->lua_script);
}
void DoMobileLog(Character * character, std::istream &sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a mobile id.\n");
        return;
    }
    Mobile * mobile = Mud::instance().findMobile(arguments[0].first);
    if (mobile == nullptr)
    {
        character->sendMsg("Mobile not found.\n");
        return;
    }
    // Notify.
    character->sendMsg("Target(%s)\n", mobile->getName());
    character->sendMsg("Log:\n%s\n", mobile->message_buffer);
}

void DoHurt(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("Who do you want to hurt?\n");
        return;
    }
    Character * target = character->room->findCharacter(arguments[0].first, arguments[0].second);
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return;
    }
    // Set health to 1.
    target->setHealth(1);
    // Notify.
    character->sendMsg("You point your finger, %s cry in pain.\n", target->getName());
    target->sendMsg("%s points the finger towards you, you cry in pain.\n", character->getNameCapital());
    // Set the list of exceptions.
    CharacterVector exceptions;
    exceptions.push_back(character);
    exceptions.push_back(target);
    // Send the message inside the room.
    target->room->sendToAll(
        "%s points the finger towards %s, %s cries in pain.\n",
        exceptions,
        character->getNameCapital(),
        target->getName(),
        target->getPronoun());
}

void DoInvisibility(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Check if already invisibile.
    if (HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("Don't worry, you are already invisible.\n");
        return;
    }
    // Set the character invisible.
    SetFlag(character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are invisible now.\n");
}

void DoVisible(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    // Check if already visible.
    if (!HasFlag(character->flags, CharacterFlag::Invisible))
    {
        character->sendMsg("You are not invisible.\n");
        return;
    }
    // Set the character visible.
    ClearFlag(character->flags, CharacterFlag::Invisible);
    character->sendMsg("You are no more invisible.\n");
}

void DoModSkill(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 3)
    {
        character->sendMsg("Usage: [target] [#skill] [+/-VALUE]\n");
        return;
    }
    Player * target = character->room->findPlayer(arguments[0].first, arguments[0].second);
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return;
    }
    Skill * skill = Mud::instance().findSkill(ToInt(arguments[1].first));
    if (skill == nullptr)
    {
        character->sendMsg("Cannot find the desired skill.\n");
        return;
    }
    int modifier = ToNumber<int>(arguments[2].first);
    if ((modifier == 0) || (modifier < -100) || (modifier > 100))
    {
        character->sendMsg("You must insert a valid modifier.\n");
        return;
    }
    int modified = static_cast<int>(target->skills[skill->vnum]) + modifier;
    if (modified <= 0)
    {
        character->sendMsg("You cannot reduce the skill level to 0 or below.\n");
        return;
    }
    if (modified >= 100)
    {
        character->sendMsg("You cannot increase the skill to 100 or above.\n");
        return;
    }
    // Change the skill value.
    target->skills[skill->vnum] = static_cast<unsigned int>(modified);
    // Notify.
    std::string msg;
    character->sendMsg(
        "You have successfully %s by %s the \"%s\" skill, the new level is %s.\n",
        ((modifier > 0) ? "increased " : "decreased"),
        ToString(modifier),
        skill->name,
        ToString(target->skills[skill->vnum]));
}

void DoModAttr(Character * character, std::istream & sArgs)
{
    // Stop any action the character is executing.
    StopAction(character);
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    // Check the number of arguments.
    if (arguments.size() != 3)
    {
        character->sendMsg("Usage: [target] [attribute] [+/-VALUE]\n");
        return; // Skip the rest of the function.
    }
    Character * target = character->room->findCharacter(arguments[0].first, arguments[0].second);
    if (target == nullptr)
    {
        character->sendMsg("Target not found.\n");
        return; // Skip the rest of the function.
    }
    int modifier = ToInt(arguments[2].first);
    if (modifier == 0)
    {
        character->sendMsg("You must insert a valid value.\n");
        return; // Skip the rest of the function.
    }
    Ability ability;
    if (arguments[1].first == "str")
    {
        ability = Ability::Strength;
    }
    else if (arguments[1].first == "agi")
    {
        ability = Ability::Agility;
    }
    else if (arguments[1].first == "per")
    {
        ability = Ability::Perception;
    }
    else if (arguments[1].first == "con")
    {
        ability = Ability::Constitution;
    }
    else if (arguments[1].first == "int")
    {
        ability = Ability::Intelligence;
    }
    else
    {
        character->sendMsg("Bad attribute name, accepted : (str, agi, per, con, int).\n");
        return;
    }
    // Get the resulting ability value.
    int result = static_cast<int>(target->getAbility(ability)) + modifier;
    if (result < 0)
    {
        character->sendMsg("Attribute cannot go below 0.");
        return; // Skip the rest of the function.
    }
    else if (!target->setAbility(ability, static_cast<unsigned int>(result)))
    {
        character->sendMsg("Attribute cannot go above 60.");
        return; // Skip the rest of the function.
    }
    character->sendMsg(
        "You have successfully %s by %s the %s of the target.",
        std::string((modifier > 0) ? "increased" : "decreased"),
        ToString(modifier),
        GetAbilityName(ability));
}

void DoAggroList(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a valide character name.\n");
        return;
    }
    Character * targer = Mud::instance().findPlayer(arguments[0].first);
    if (targer == nullptr)
    {
        targer = Mud::instance().findMobile(arguments[0].first);
        if (targer == nullptr)
        {
            character->sendMsg("Character not found.\n");
            return;
        }
    }
    for (auto aggressor : targer->opponents)
    {
        if (aggressor.aggressor != nullptr)
        {
            character->sendMsg("%s\t%s", ToString(aggressor.aggression), aggressor.aggressor->getNameCapital());
        }
    }
}

void DoMaterialInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must insert a valide material vnum.\n");
        return;
    }
    Material * material = Mud::instance().findMaterial(ToInt(arguments[0].first));
    if (material == nullptr)
    {
        character->sendMsg("Can't find the desire material.\n");
        return;
    }
    std::string msg;
    msg += Formatter::yellow() + "Vnum            " + Formatter::reset() + ": " + ToString(material->vnum) + "\n";
    msg += Formatter::yellow() + "Type            " + Formatter::reset() + ": " + GetMaterialTypeName(material->type)
        + "\n";
    msg += Formatter::yellow() + "Name            " + Formatter::reset() + ": " + material->name + "\n";
    msg += Formatter::yellow() + "Worth           " + Formatter::reset() + ": " + ToString(material->worth) + "\n";
    msg += Formatter::yellow() + "Hardness        " + Formatter::reset() + ": " + ToString(material->hardness) + "\n";
    msg += Formatter::yellow() + "Lightness       " + Formatter::reset() + ": " + ToString(material->lightness) + "\n";
    character->sendMsg(msg);
}

void DoLiquidCreate(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 3)
    {
        character->sendMsg("Usage: [Item] [Liquid] [Quantity].\n");
        return;
    }
    Item * item = character->findNearbyItem(arguments[0].first, arguments[0].second);
    if (item == nullptr)
    {
        character->sendMsg("Can't find the desire item.\n");
        return;
    }
    if (item->model->type != ModelType::LiqContainer)
    {
        character->sendMsg("The item is not a container of liquids.\n");
        return;
    }
    Liquid * liquid = Mud::instance().findLiquid(ToInt(arguments[1].first));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
        return;
    }
    int quantity = ToInt(arguments[2].first);
    if ((quantity <= 0) || (quantity >= 100))
    {
        character->sendMsg("You must insert a valid quantity of liquids between 1 and 99.\n");
        return;
    }
    if (!item->pourIn(liquid, static_cast<unsigned int>(quantity)))
    {
        character->sendMsg("The selected item can't contain that quantity of liquid.\n");
        return;
    }
    character->sendMsg(
        "You materialise %s units of %s inside %s.\n",
        ToString(quantity),
        liquid->getName(),
        item->getName());
}

void DoLiquidInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a liquid vnum.\n");
        return;
    }
    Liquid * liquid = Mud::instance().findLiquid(ToInt(arguments[0].first));
    if (liquid == nullptr)
    {
        character->sendMsg("Can't find the desire liquid.\n");
        return;
    }
    std::string msg;
    msg += Formatter::yellow() + "Vnum            " + Formatter::reset() + ": " + ToString(liquid->vnum) + "\n";
    msg += Formatter::yellow() + "Name            " + Formatter::reset() + ": " + liquid->getNameCapital() + "\n";
    msg += Formatter::yellow() + "Worth           " + Formatter::reset() + ": " + ToString(liquid->worth) + "\n";
    character->sendMsg(msg);
}

void DoProductionInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a production vnum.\n");
        return;
    }
    Production * production = Mud::instance().findProduction(ToInt(arguments[0].first));
    if (production == nullptr)
    {
        character->sendMsg("Can't find the desire production.\n");
        return;
    }
    std::string msg;
    msg += "Vnum            : " + ToString(production->vnum) + "\n";
    msg += "Name            : " + production->name + "\n";
    msg += "Profession      : " + production->profession->command + "\n";
    msg += "Difficulty      : " + ToString(production->difficulty) + "\n";
    msg += "Time            : " + ToString(production->time) + "\n";
    msg += "Assisted        : " + ToString(production->assisted) + "\n";
    msg += "Outcome         : " + production->outcome.first->name + "[" + ToString(production->outcome.second) + "\n";
    msg += "Tools           :\n";
    for (auto iterator : production->tools)
    {
        msg += "                  " + GetToolTypeName(iterator) + "\n";
    }
    msg += "Ingredients     :\n";
    for (auto iterator : production->ingredients)
    {
        msg += "                  " + GetResourceTypeName(iterator.first) + "(" + ToString(iterator.second) + ")\n";
    }
    character->sendMsg(msg);
}

void DoProfessionInfo(Character * character, std::istream & sArgs)
{
    // Get the arguments of the command.
    ArgumentList arguments = ParseArgs(sArgs);
    if (arguments.size() != 1)
    {
        character->sendMsg("You must provide a profession name.\n");
        return;
    }
    Profession * profession = Mud::instance().findProfession(arguments[0].first);
    if (profession == nullptr)
    {
        character->sendMsg("Can't find the desire profession.\n");
        return;
    }
    std::string msg;
    msg += Formatter::yellow() + "Name          " + Formatter::reset() + ": " + profession->name + "\n";
    msg += Formatter::yellow() + "Description   " + Formatter::reset() + ": " + profession->description + "\n";
    msg += Formatter::yellow() + "Command       " + Formatter::reset() + ": " + profession->command + "\n";
    msg += Formatter::yellow() + "Posture       " + Formatter::reset() + ": " + GetPostureName(profession->posture)
        + "\n";
    msg += Formatter::yellow() + "Action        " + Formatter::reset() + ": " + profession->action + "\n";
    msg += Formatter::yellow() + "    Start     " + Formatter::reset() + ": " + profession->startMessage + "\n";
    msg += Formatter::yellow() + "    Finish    " + Formatter::reset() + ": " + profession->finishMessage + "\n";
    msg += Formatter::yellow() + "    Success   " + Formatter::reset() + ": " + profession->successMessage + "\n";
    msg += Formatter::yellow() + "    Failure   " + Formatter::reset() + ": " + profession->failureMessage + "\n";
    msg += Formatter::yellow() + "    Interrupt " + Formatter::reset() + ": " + profession->interruptMessage + "\n";
    msg += Formatter::yellow() + "    Not Found " + Formatter::reset() + ": " + profession->notFoundMessage + "\n";
    character->sendMsg(msg);
}

void DoModelList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("TYPE", StringAlign::Left);
    table.addColumn("SPECIFIC", StringAlign::Left);
    table.addColumn("SLOT", StringAlign::Left);
    table.addColumn("FLAGS", StringAlign::Right);
    table.addColumn("WEIGHT", StringAlign::Right);
    table.addColumn("PRICE", StringAlign::Right);
    for (auto iterator : Mud::instance().mudModels)
    {
        Model * model = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(model->vnum));
        row.push_back(model->name);
        row.push_back(GetModelTypeName(model->type));
        row.push_back(model->getSpecificTypeName());
        row.push_back(GetEquipmentSlotName(model->slot));
        row.push_back(ToString(model->flags));
        row.push_back(ToString(model->weight));
        row.push_back(ToString(model->price));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoItemList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("TYPE", StringAlign::Left);
    table.addColumn("LOCATION", StringAlign::Left);
    for (auto item : Mud::instance().mudItems)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(item->vnum));
        row.push_back(item->getNameCapital());
        row.push_back(GetModelTypeName(item->model->type));
        if (item->owner != nullptr)
        {
            row.push_back(" Owner  : " + item->owner->getName());
        }
        else if (item->room != nullptr)
        {
            row.push_back(" Room   : " + ToString(item->room->vnum));
        }
        else if (item->container != nullptr)
        {
            row.push_back(" Inside : " + ToString(item->container->vnum));
        }
        else
        {
            row.push_back(" Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoMobileList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("ALIVE", StringAlign::Center);
    table.addColumn("ID", StringAlign::Left);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("LOCATION", StringAlign::Right);
    for (auto iterator : Mud::instance().mudMobiles)
    {
        Mobile * mobile = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back((mobile->alive) ? "Yes" : "No");
        row.push_back(mobile->id);
        row.push_back(mobile->getName());
        if (mobile->room != nullptr)
        {
            row.push_back("Room : " + ToString(mobile->room->vnum));
        }
        else
        {
            row.push_back("Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoPlayerList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ROOM", StringAlign::Left);
    for (auto iterator : Mud::instance().mudPlayers)
    {
        // If the player is not playing, continue.
        if (!iterator->isPlaying())
        {
            continue;
        }
        // Prepare the row.
        TableRow row;
        row.push_back(iterator->name);
        row.push_back(iterator->room->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoAreaList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("BUILDER", StringAlign::Left);
    table.addColumn("CONTINENT", StringAlign::Center);
    table.addColumn("ROOMS", StringAlign::Center);
    for (auto iterator : Mud::instance().mudAreas)
    {
        Area * area = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(area->vnum));
        row.push_back(area->name);
        row.push_back(area->builder);
        row.push_back(area->continent);
        row.push_back(ToString(area->areaMap.size()));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoRoomList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("AREA", StringAlign::Left);
    table.addColumn("COORD", StringAlign::Center);
    table.addColumn("TERRAIN", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    for (auto iterator : Mud::instance().mudRooms)
    {
        Room * room = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(room->vnum));
        if (room->area != nullptr)
        {
            row.push_back(room->area->name);
        }
        else
        {
            row.push_back("None");
        }
        row.push_back(ToString(room->coord.x) + ' ' + ToString(room->coord.y) + ' ' + ToString(room->coord.z));
        row.push_back(room->terrain);
        row.push_back(room->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoRaceList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ALLOWED", StringAlign::Left);
    table.addColumn("STRENGTH", StringAlign::Right);
    table.addColumn("AGILITY", StringAlign::Right);
    table.addColumn("PERCEPTION", StringAlign::Right);
    table.addColumn("CONSTITUTION", StringAlign::Right);
    table.addColumn("INTELLIGENCE", StringAlign::Right);
    for (auto iterator : Mud::instance().mudRaces)
    {
        Race * race = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(race->vnum));
        row.push_back(race->name);
        row.push_back(BoolToString(race->player_allow));
        row.push_back(ToString(race->strength));
        row.push_back(ToString(race->agility));
        row.push_back(ToString(race->perception));
        row.push_back(ToString(race->constitution));
        row.push_back(ToString(race->intelligence));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoFactionList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    for (auto iterator : Mud::instance().mudFactions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoSkillList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("ATTRIBUTE", StringAlign::Left);
    for (auto iterator : Mud::instance().mudSkills)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        row.push_back(GetAttributeName(iterator.second->attribute, true));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoWritingList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("AUTHOR", StringAlign::Left);
    table.addColumn("TITLE", StringAlign::Left);
    for (auto iterator : Mud::instance().mudWritings)
    {
        Writing * writing = iterator.second;
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(writing->vnum));
        row.push_back(writing->author);
        row.push_back(writing->title);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoCorpseList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("LOCATION", StringAlign::Left);
    for (auto item : Mud::instance().mudCorpses)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(item->vnum));
        row.push_back(item->getNameCapital());
        if (item->owner != nullptr)
        {
            row.push_back(" Owner  : " + item->owner->getName());
        }
        else if (item->room != nullptr)
        {
            row.push_back(" Room   : " + ToString(item->room->vnum));
        }
        else if (item->container != nullptr)
        {
            row.push_back(" Inside : " + ToString(item->container->vnum));
        }
        else
        {
            row.push_back(" Is nowhere.");
        }
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoContinentList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
}

void DoMaterialList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Right);
    table.addColumn("TYPE", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Left);
    table.addColumn("WORTH", StringAlign::Right);
    table.addColumn("HARDNESS", StringAlign::Right);
    table.addColumn("LIGHTNESS", StringAlign::Right);
    for (auto iterator : Mud::instance().mudMaterials)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(GetMaterialTypeName(iterator.second->type));
        row.push_back(iterator.second->name);
        row.push_back(ToString(iterator.second->worth));
        row.push_back(ToString(iterator.second->hardness));
        row.push_back(ToString(iterator.second->lightness));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoProfessionList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("COMMAND", StringAlign::Center);
    table.addColumn("POSTURE", StringAlign::Center);
    table.addColumn("ACTION", StringAlign::Center);
    for (auto iterator : Mud::instance().mudProfessions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(iterator.second->name);
        row.push_back(iterator.second->command);
        row.push_back(GetPostureName(iterator.second->posture));
        row.push_back(iterator.second->action);
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoProductionList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("PROFESSION", StringAlign::Center);
    table.addColumn("DIFFICULTY", StringAlign::Left);
    for (auto iterator : Mud::instance().mudProductions)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->name);
        row.push_back(iterator.second->profession->command);
        row.push_back(ToString(iterator.second->difficulty));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoLiquidList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Right);
    table.addColumn("WORTH", StringAlign::Right);
    for (auto iterator : Mud::instance().mudLiquids)
    {
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(iterator.second->vnum));
        row.push_back(iterator.second->getNameCapital());
        row.push_back(ToString(iterator.second->worth));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}

void DoBuildingList(Character * character, std::istream & sArgs)
{
    // Check no more input.
    NoMore(character, sArgs);
    Table table;
    table.addColumn("VNUM", StringAlign::Center);
    table.addColumn("NAME", StringAlign::Center);
    table.addColumn("DIFFICULTY", StringAlign::Left);
    table.addColumn("TIME", StringAlign::Center);
    table.addColumn("UNIQUE", StringAlign::Center);
    for (auto iterator : Mud::instance().mudBuildings)
    {
        Building * building = &(iterator.second);
        // Prepare the row.
        TableRow row;
        row.push_back(ToString(building->vnum));
        row.push_back(building->name);
        row.push_back(ToString(building->difficulty));
        row.push_back(ToString(building->time));
        row.push_back(ToString(building->unique));
        // Add the row to the table.
        table.addRow(row);
    }
    character->sendMsg(table.getTable());
}
