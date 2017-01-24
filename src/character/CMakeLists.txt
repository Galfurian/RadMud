#############################################
# Sub-directories
add_subdirectory(${CMAKE_SOURCE_DIR}/src/character/effect)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/character/skill)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/character/race)

#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/character
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/character/bodyPart.cpp
        ${CMAKE_SOURCE_DIR}/src/character/character.cpp
        ${CMAKE_SOURCE_DIR}/src/character/mobile.cpp
        ${CMAKE_SOURCE_DIR}/src/character/player.cpp
        ${CMAKE_SOURCE_DIR}/src/character/characterContainer.cpp
        ${CMAKE_SOURCE_DIR}/src/character/faction.cpp
        ${CMAKE_SOURCE_DIR}/src/character/combatHandler.cpp
        PARENT_SCOPE
        )