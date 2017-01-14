#############################################
# Sub-directories
add_subdirectory(${CMAKE_SOURCE_DIR}/src/character/modifiers)

#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/character
        ${CMAKE_SOURCE_DIR}/src/character/skill
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
        ${CMAKE_SOURCE_DIR}/src/character/race.cpp
        ${CMAKE_SOURCE_DIR}/src/character/skill/skill.cpp
        ${CMAKE_SOURCE_DIR}/src/character/combatHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effect.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effectList.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effectFactory.cpp
        PARENT_SCOPE
        )