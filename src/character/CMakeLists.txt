#############################################
# Include Directories
set(RADMUD_CHARACTER_INC
        ./src/character
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_CHARACTER_SRC
        ./src/character/character.cpp
        ./src/character/mobile.cpp
        ./src/character/player.cpp
        ./src/character/characterContainer.cpp
        PARENT_SCOPE
        )