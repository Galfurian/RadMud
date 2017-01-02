#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/command/god
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGod.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodMud.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodItem.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodMobile.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodCreation.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodCharacter.cpp
        ${CMAKE_SOURCE_DIR}/src/command/god/commandGodStructure.cpp
        PARENT_SCOPE
        )