#############################################
add_subdirectory(combat)

#############################################
# Include Directories
set(RADMUD_ACTION_INC
        ./src/action
        ${RADMUD_COMBAT_ACTION_INC}
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_ACTION_SRC
        ./src/action/generalAction.cpp
        ./src/action/buildAction.cpp
        ./src/action/craftAction.cpp
        ./src/action/moveAction.cpp
        ./src/action/scoutAction.cpp
        ./src/action/reloadAction.cpp
        ./src/action/loadAction.cpp
        ./src/action/unloadAction.cpp
        ${RADMUD_COMBAT_ACTION_SRC}
        PARENT_SCOPE
        )