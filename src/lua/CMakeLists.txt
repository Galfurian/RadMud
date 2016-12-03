#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/lua/luabridge/detail
        ${CMAKE_SOURCE_DIR}/src/lua/luabridge
        ${CMAKE_SOURCE_DIR}/src/lua
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/lua/lua_script.cpp
        PARENT_SCOPE
        )