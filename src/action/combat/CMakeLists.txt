#############################################
# Combat Action Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/action/combat
        PARENT_SCOPE
        )

#############################################
# Combat Action Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/action/combat/combatAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/basicAttack.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/chase.cpp
        ${CMAKE_SOURCE_DIR}/src/action/combat/flee.cpp
        PARENT_SCOPE
        )