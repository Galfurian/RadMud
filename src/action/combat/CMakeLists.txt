#############################################
# Combat Action Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/action/combat
        PARENT_SCOPE
        )

#############################################
# Combat Action Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/action/combat/combatAction.cpp
        ./src/action/combat/basicAttack.cpp
        ./src/action/combat/chase.cpp
        ./src/action/combat/flee.cpp
        PARENT_SCOPE
        )