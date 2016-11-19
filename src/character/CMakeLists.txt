#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/character
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/character/character.cpp
        ./src/character/mobile.cpp
        ./src/character/player.cpp
        ./src/character/characterContainer.cpp
        ./src/character/faction.cpp
        ./src/character/race.cpp
        ./src/character/skill.cpp
        ./src/character/combatHandler.cpp
        ./src/character/effect.cpp
        ./src/character/effectList.cpp
        ./src/character/effectFactory.cpp
        PARENT_SCOPE
        )