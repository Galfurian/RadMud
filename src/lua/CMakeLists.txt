#############################################
# Directories
set(RADMUD_LUA_INC
        ./src/lua/luabridge/detail
        ./src/lua/luabridge
        ./src/lua
        PARENT_SCOPE
        )

#############################################
# Files
set(RADMUD_LUA_SRC
        ./src/lua/lua_script.cpp
        ./src/lua/luabridge/detail/LuaHelpers.cpp
        PARENT_SCOPE
        )