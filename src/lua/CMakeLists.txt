#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/lua
        ./src/lua/luabridge
        ./src/lua/luabridge/detail
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/lua/lua_script.cpp
        PARENT_SCOPE
        )