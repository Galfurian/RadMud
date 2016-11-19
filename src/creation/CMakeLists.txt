#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/creation/
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/creation/building.cpp
        ./src/creation/production.cpp
        ./src/creation/liquid.cpp
        ./src/creation/material.cpp
        ./src/creation/profession.cpp
        PARENT_SCOPE
        )