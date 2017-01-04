#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/item
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/item/item.cpp
        ${CMAKE_SOURCE_DIR}/src/item/writing.cpp
        ${CMAKE_SOURCE_DIR}/src/item/itemFactory.cpp
        ${CMAKE_SOURCE_DIR}/src/item/itemVector.cpp
        PARENT_SCOPE
        )