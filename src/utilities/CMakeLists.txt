#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/utilities/
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/utilities/CMacroWrapper.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/formatter.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/table.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/logger.cpp
        ${CMAKE_SOURCE_DIR}/src/utilities/utils.cpp
        PARENT_SCOPE
        )