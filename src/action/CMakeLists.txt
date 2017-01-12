#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/action
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/action/generalAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/buildAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/craftAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/moveAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/scoutAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/reloadAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/loadAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/unloadAction.cpp
        ${CMAKE_SOURCE_DIR}/src/action/aimAction.cpp
        PARENT_SCOPE
        )