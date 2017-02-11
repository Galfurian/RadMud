/// @file   lua_script.cpp
/// @brief  Implements all the methods used by lua to interact and communicate
///          with the mud.
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

void LuaStopScript()
{
    throw std::logic_error("Stopped Lua Script");
}

void LuaRegisterUtils(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Mud")
        .addFunction("log", LuaLog)
        .addFunction("sleep", LuaSleep)
        .addFunction("random", LuaRandom)
        .addFunction("stop", LuaStopScript)
        .endNamespace();
}

void LoadLuaEnvironmet(lua_State * L, const std::string & scriptFile)
{
    // -------------------------------------------------------------------------
    // Open lua libraries.
    luaL_openlibs(L);
    // Register utility functions.
    LuaRegisterUtils(L);

    // -------------------------------------------------------------------------
    // Register all the base classes.
    Character::luaRegister(L);
    Area::luaRegister(L);
    Faction::luaRegister(L);
    ItemModel::luaRegister(L);
    Item::luaRegister(L);
    Material::luaRegister(L);
    Race::luaRegister(L);
    Coordinates::luaRegister(L);
    Exit::luaRegister(L);
    terrain::Terrain::luaRegister(L);
    Room::luaRegister(L);

    // -------------------------------------------------------------------------
    // The enumerators.
    Direction::luaRegister(L);
    ModelType::luaRegister(L);
    ToolType::luaRegister(L);
    ResourceType::luaRegister(L);

    // -------------------------------------------------------------------------
    // Load the script.
    auto path = Mud::instance().getMudSystemDirectory() + "lua/" + scriptFile;
    if (luaL_dofile(L, path.c_str()) != LUABRIDGE_LUA_OK)
    {
        Logger::log(LogLevel::Error, "Can't open script %s.", scriptFile);
        Logger::log(LogLevel::Error, "Error :%s",
                    std::string(lua_tostring(L, -1)));
    }
}