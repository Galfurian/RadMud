#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/model
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/model/itemModel.cpp
        ./src/model/modelFactory.cpp
        PARENT_SCOPE
        )