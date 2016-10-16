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
        ./src/action/combat/basicMeleeAttack.cpp
        ./src/action/combat/basicRangedAttack.cpp
        ./src/action/combat/flee.cpp
        PARENT_SCOPE
        )