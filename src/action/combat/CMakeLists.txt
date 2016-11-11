#############################################
# Combat Action Include Directories
set(RADMUD_COMBAT_ACTION_INC
        ./src/action/combat
        PARENT_SCOPE
        )

#############################################
# Combat Action Source Files
set(RADMUD_COMBAT_ACTION_SRC
        ./src/action/combat/combatAction.cpp
        ./src/action/combat/basicAttack.cpp
        ./src/action/combat/chase.cpp
        ./src/action/combat/flee.cpp
        PARENT_SCOPE
        )