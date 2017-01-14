#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/character/modifiers
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/character/modifiers/abilityModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/character/modifiers/combatModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/character/modifiers/statusModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/character/modifiers/knowledge.cpp
        PARENT_SCOPE
        )