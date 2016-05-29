/// @file   lua_script.cpp
/// @brief  Implements all the methods used by lua to interract and communicate with the mud.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
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

#include "lua_script.hpp"

#include "../mud.hpp"
#include "../logger.hpp"
#include "../commands.hpp"
#include "../constants.hpp"
#include "../luabridge/LuaBridge.h"

void LuaLog(std::string message)
{
    if (!message.empty())
    {
        Logger::log(LogLevel::Info, "[LUA]" + message);
    }
}

void LuaSleep(int sleepTime)
{
    if ((sleepTime > 0) && (sleepTime <= 20))
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }
}

int LuaRandom(int min, int max)
{
    return RandInteger(min, max);
}

Item * LuaLoadItem(Character * maker, int vnumModel, int vnumMaterial, int intQuality)
{
    if (maker == nullptr)
    {
        Logger::log(LogLevel::Error, "Received null maker.");
        return nullptr;
    }

    Model * model = Mud::instance().findModel(vnumModel);
    Material * material = Mud::instance().findMaterial(vnumMaterial);
    ItemQuality quality = static_cast<ItemQuality>(intQuality);

    if (model == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find model :" + ToString(vnumModel));
        return nullptr;
    }
    if (material == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find material :" + ToString(vnumMaterial));
        return nullptr;
    }

    // Instantiate the new item.
    Item * newItem = new Item();
    // Set the values of the new item.
    newItem->vnum = -1;
    newItem->model = model;
    newItem->maker = maker->getName();
    newItem->condition = model->condition;
    newItem->composition = material;
    newItem->quality = quality;
    newItem->room = nullptr;
    newItem->owner = nullptr;
    newItem->container = nullptr;
    newItem->currentSlot = model->slot;
    newItem->content = std::vector<Item *>();
    newItem->contentLiq = LiquidContent();

    return newItem;
}

void LuaRegisterUtils(lua_State * L)
{
    luabridge::getGlobalNamespace(L) //
    .beginNamespace("Mud") //
    .addFunction("Log", LuaLog) //
    .addFunction("Sleep", LuaSleep) //
    .addFunction("Random", LuaRandom) //
    .addFunction("LoadItem", LuaLoadItem) //
    .endNamespace();

    luabridge::getGlobalNamespace(L) //
    .beginClass<VectorHelper<std::string>>("StringVector") //
    .addFunction("size", &VectorHelper<std::string>::size) //
    .addFunction("back", &VectorHelper<std::string>::back) //
    .addFunction("front", &VectorHelper<std::string>::front) //
    .addFunction("at", &VectorHelper<std::string>::at) //
    .addFunction("push_back", &VectorHelper<Character *>::push_back) //
    .addFunction("pop_back", &VectorHelper<Character *>::pop_back) //
    .endClass();

    luabridge::getGlobalNamespace(L) //
    .beginClass<VectorHelper<Character *>>("CharacterVector") //
    .addFunction("size", &VectorHelper<Character *>::size) //
    .addFunction("back", &VectorHelper<Character *>::back) //
    .addFunction("front", &VectorHelper<Character *>::front) //
    .addFunction("at", &VectorHelper<Character *>::at) //
    .addFunction("push_back", &VectorHelper<Character *>::push_back) //
    .addFunction("pop_back", &VectorHelper<Character *>::pop_back) //
    .endClass();

    luabridge::getGlobalNamespace(L) //
    .beginClass<VectorHelper<Item *>>("ItemVector") //
    .addFunction("size", &VectorHelper<Item *>::size) //
    .addFunction("back", &VectorHelper<Item *>::back) //
    .addFunction("front", &VectorHelper<Item *>::front) //
    .addFunction("at", &VectorHelper<Item *>::at) //
    .addFunction("push_back", &VectorHelper<Item *>::push_back) //
    .addFunction("pop_back", &VectorHelper<Item *>::pop_back) //
    .endClass();

    luabridge::getGlobalNamespace(L) //
    .beginClass<VectorHelper<Exit *>>("ExitVector") //
    .addFunction("size", &VectorHelper<Exit *>::size) //
    .addFunction("back", &VectorHelper<Exit *>::back) //
    .addFunction("front", &VectorHelper<Exit *>::front) //
    .addFunction("at", &VectorHelper<Exit *>::at) //
    .addFunction("push_back", &VectorHelper<Exit *>::push_back) //
    .addFunction("pop_back", &VectorHelper<Exit *>::pop_back) //
    .endClass();
}
