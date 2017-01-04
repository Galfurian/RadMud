#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/creation/
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/creation/building.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/production.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/liquid.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/material.cpp
        ${CMAKE_SOURCE_DIR}/src/creation/profession.cpp
        PARENT_SCOPE
        )