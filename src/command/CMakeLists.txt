#############################################
# Sub-directories
add_subdirectory(${CMAKE_SOURCE_DIR}/src/command/object)

#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/command
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/command/command.cpp
        ${CMAKE_SOURCE_DIR}/src/command/combat.cpp
        ${CMAKE_SOURCE_DIR}/src/command/communication.cpp
        ${CMAKE_SOURCE_DIR}/src/command/crafting.cpp
        ${CMAKE_SOURCE_DIR}/src/command/manager.cpp
        ${CMAKE_SOURCE_DIR}/src/command/general.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god.cpp
        PARENT_SCOPE
        )