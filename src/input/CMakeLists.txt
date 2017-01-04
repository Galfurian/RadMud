#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/input
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/input/argument.cpp
        ${CMAKE_SOURCE_DIR}/src/input/argumentHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/input/processInput.cpp
        PARENT_SCOPE
        )