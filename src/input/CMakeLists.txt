#############################################
# Include Directories
set(RADMUD_INPUT_INC
        ./src/input
        ./src/input/initialization
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_INPUT_SRC
        ./src/input/argument.cpp
        ./src/input/argumentHandler.cpp
        ./src/input/processInput.cpp
        ./src/input/processTelnetCommand.cpp
        ./src/input/initialization/processInitialization.cpp
        ./src/input/initialization/processNewAge.cpp
        ./src/input/initialization/processNewAttributes.cpp
        ./src/input/initialization/processNewConfirm.cpp
        ./src/input/initialization/processNewDescription.cpp
        ./src/input/initialization/processNewGender.cpp
        ./src/input/initialization/processNewName.cpp
        ./src/input/initialization/processNewPassword.cpp
        ./src/input/initialization/processNewPasswordConfirm.cpp
        ./src/input/initialization/processNewRace.cpp
        ./src/input/initialization/processNewStory.cpp
        ./src/input/initialization/processNewWeight.cpp
        ./src/input/initialization/processPlayerName.cpp
        ./src/input/initialization/processPlayerPassword.cpp
        PARENT_SCOPE
        )