#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/generation
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/generation/mapGenerator.cpp
        ${CMAKE_SOURCE_DIR}/src/generation/heightMapper.cpp
        PARENT_SCOPE
        )