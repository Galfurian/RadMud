#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/action/object
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/action/object/dismemberAction.cpp
        PARENT_SCOPE
        )