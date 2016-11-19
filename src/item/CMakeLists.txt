#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/item
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/item/item.cpp
        ./src/item/writing.cpp
        ./src/item/itemFactory.cpp
        ./src/item/itemContainer.cpp
        PARENT_SCOPE
        )