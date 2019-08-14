/// @file   mobileModel.cpp
/// @brief  Define the model of a Mobile OBject.
/// @author Enrico Fraccaroli
/// @date   24 02 2018
/// @copyright
/// Copyright (c) 2018 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "mobileModel.hpp"
#include "mobile.hpp"
#include "mud.hpp"

Mobile *MobileModel::spawn(Room *spawnRoom, unsigned int mobileVnum)
{
	(void)spawnRoom;
	(void)mobileVnum;
	if (spawnRoom == nullptr)
		return nullptr;
	if (mobileVnum == 0) {
		mobileVnum = Mud::instance().getMaxVnumMobile() + 1;
	}
	// Initialize the mobile.
	auto mob = new Mobile(shared_from_this());
	mob->vnum = mobileVnum;
	mob->respawnRoom = spawnRoom;
	mob->room = mob->respawnRoom;
	mob->name = propnoun;
	mob->keys = keys;
	mob->shortdesc = shortdesc;
	mob->staticdesc = staticdesc;
	mob->description = description;
	mob->race = race;
	mob->faction = faction;
	mob->gender = gender;
	mob->weight = weight;
	mob->actions = actions;
	mob->flags = flags;
	mob->level = level;
	mob->abilities = abilities;
	mob->lua_script = lua_script;
	mob->setHealth(mob->getMaxHealth(), true);
	mob->setStamina(mob->getMaxStamina(), true);
	// Check the correctness.
	if (!mob->check()) {
		delete (mob);
		Logger::log(LogLevel::Error, "Error during error checking on mob.");
		return nullptr;
	}
	if (!Mud::instance().addMobile(mob)) {
		delete (mob);
		Logger::log(LogLevel::Error, "Error during mob insertion.");
		return nullptr;
	}
	// Respawn the mob.
	mob->respawn();
	return mob;
}