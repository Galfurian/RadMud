/// @file   lua_script.cpp
/// @brief  Implements all the methods used by lua to interract and communicate with the mud.
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

#include "lua_script.hpp"

#include "logger.hpp"
#include "mud.hpp"

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
    return TRandInteger<int>(min, max);
}

Item * LuaLoadItem(Character * maker, int vnumModel, int vnumMaterial, unsigned int qualityValue)
{
    if (maker == nullptr)
    {
        Logger::log(LogLevel::Error, "Received null maker.");
        return nullptr;
    }
    auto model = Mud::instance().findItemModel(vnumModel);
    if (model == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find model :" + ToString(vnumModel));
        return nullptr;
    }
    auto composition = Mud::instance().findMaterial(vnumMaterial);
    if (composition == nullptr)
    {
        Logger::log(LogLevel::Error, "Can't find material :" + ToString(vnumMaterial));
        return nullptr;
    }
    ItemQuality quality = ItemQuality::Normal;
    if (ItemQuality::isValid(qualityValue))
    {
        quality = ItemQuality(qualityValue);
    }
    return model->createItem(maker->getName(), composition, true, quality);
}

void LuaRegisterUtils(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Mud")
        .addFunction("Log", LuaLog)
        .addFunction("Sleep", LuaSleep)
        .addFunction("Random", LuaRandom)
        .addFunction("LoadItem", LuaLoadItem)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<VectorHelper<std::string>>("StringVector")
        .addFunction("size", &VectorHelper<std::string>::size)
        .addFunction("back", &VectorHelper<std::string>::back)
        .addFunction("front", &VectorHelper<std::string>::front)
        .addFunction("at", &VectorHelper<std::string>::at)
        .addFunction("push_back", &VectorHelper<std::string>::push_back)
        .addFunction("pop_back", &VectorHelper<std::string>::pop_back)
        .addFunction("empty", &VectorHelper<std::string>::empty)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<VectorHelper<Character *>>("CharacterVector")
        .addFunction("size", &VectorHelper<Character *>::size)
        .addFunction("back", &VectorHelper<Character *>::back)
        .addFunction("front", &VectorHelper<Character *>::front)
        .addFunction("at", &VectorHelper<Character *>::at)
        .addFunction("push_back", &VectorHelper<Character *>::push_back)
        .addFunction("pop_back", &VectorHelper<Character *>::pop_back)
        .addFunction("empty", &VectorHelper<Character *>::empty)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<VectorHelper<Item *>>("ItemVector")
        .addFunction("size", &VectorHelper<Item *>::size)
        .addFunction("back", &VectorHelper<Item *>::back)
        .addFunction("front", &VectorHelper<Item *>::front)
        .addFunction("at", &VectorHelper<Item *>::at)
        .addFunction("push_back", &VectorHelper<Item *>::push_back)
        .addFunction("pop_back", &VectorHelper<Item *>::pop_back)
        .addFunction("empty", &VectorHelper<Item *>::empty)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<VectorHelper<Exit *> >("ExitVector")
        .addFunction("size", &VectorHelper<Exit *>::size)
        .addFunction("back", &VectorHelper<Exit *>::back)
        .addFunction("front", &VectorHelper<Exit *>::front)
        .addFunction("at", &VectorHelper<Exit *>::at)
        .addFunction("push_back", &VectorHelper<Exit *>::push_back)
        .addFunction("pop_back", &VectorHelper<Exit *>::pop_back)
        .addFunction("empty", &VectorHelper<Exit *>::empty)
        .endClass();
}
