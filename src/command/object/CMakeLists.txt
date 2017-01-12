#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/command/object
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectLightSource.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectManagement.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectContainer.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectCharacter.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectLiquids.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectShop.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObjectFood.cpp
        ${CMAKE_SOURCE_DIR}/src/command/object/commandObject.cpp
        PARENT_SCOPE
        )