#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/lua/luabridge/detail
        ./src/lua/luabridge
        ./src/lua
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/lua/lua_script.cpp
        PARENT_SCOPE
        )