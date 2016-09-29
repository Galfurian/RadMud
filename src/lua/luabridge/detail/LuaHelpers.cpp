#include "LuaHelpers.h"

namespace luabridge
{
int get_length(lua_State * L, int idx)
{
    lua_len(L, idx);
    return int(luaL_checknumber(L, -1));
}

lua_State * get_main_thread(lua_State * thread)
{
    lua_rawgeti(thread, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
    lua_State * L = lua_tothread(thread, -1);
    lua_pop(thread, 1);
    return L;
}

void rawgetfield(lua_State * L, int index, char const * key)
{
    assert(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_rawget(L, index);
}

void rawsetfield(lua_State * L, int index, char const * key)
{
    assert(lua_istable(L, index));
    index = lua_absindex(L, index);
    lua_pushstring(L, key);
    lua_insert(L, -2);
    lua_rawset(L, index);
}

bool isfulluserdata(lua_State * L, int index)
{
    return lua_isuserdata(L, index) && !lua_islightuserdata(L, index);
}

bool equalstates(lua_State * L1, lua_State * L2)
{
    return lua_topointer(L1, LUA_REGISTRYINDEX) == lua_topointer(L2, LUA_REGISTRYINDEX);
}

}
