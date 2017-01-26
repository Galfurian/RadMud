#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/character/effect
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/character/effect/effect.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effect/effectManager.cpp
        ${CMAKE_SOURCE_DIR}/src/character/effect/effectFactory.cpp
        PARENT_SCOPE
        )