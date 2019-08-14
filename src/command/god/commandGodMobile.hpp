/// @file   commandGodMobile.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 02 2017
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

#include "commandGod.hpp"

/// @addtogroup ComInterfaces
/// @{

/// List all the mobile models.
bool DoMobileModelList(Character *character, ArgumentHandler &args);

/// Allows to spawn new mobile.
bool DoMobileSpawn(Character *character, ArgumentHandler &args);

/// Allows to save the mobile and automatically spawn it in the same room at
/// the next boot.
bool DoMobileSave(Character *character, ArgumentHandler &args);

/// Kill the desired mobile, in the same room.
bool DoMobileKill(Character *character, ArgumentHandler &args);

/// List all the mobiles.
bool DoMobileList(Character *character, ArgumentHandler &args);

/// Reload the lua script for the target mobile, in the same room.
bool DoMobileReload(Character *character, ArgumentHandler &args);

/// Trigger the main behaviour of a mobile.
bool DoMobileTrigger(Character *character, ArgumentHandler &args);

/// Show the mobile's log.
bool DoMobileLog(Character *character, ArgumentHandler &args);

/// @}
