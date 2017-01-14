#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/character/modifier
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/character/modifier/abilityModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/character/modifier/combatModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/character/modifier/statusModifier.cpp
        ${CMAKE_SOURCE_DIR}/src/character/modifier/knowledge.cpp
        PARENT_SCOPE
        )