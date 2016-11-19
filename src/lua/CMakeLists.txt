#############################################
# Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/lua/luabridge/detail
        ./src/lua/luabridge
        ./src/lua
        PARENT_SCOPE
        )

#############################################
# Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/lua/lua_script.cpp
        ./src/lua/luabridge/detail/LuaHelpers.cpp
        PARENT_SCOPE
        )