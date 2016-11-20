#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/support
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/support/ability.cpp
        ./src/support/direction.cpp
        ./src/support/telnetChar.cpp
        ./src/support/itemQuality.cpp
        ./src/support/equipmentSlot.cpp
        ./src/support/characterPosture.cpp
        PARENT_SCOPE
        )