#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/model/submodel
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/model/submodel/armorModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/bookModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/containerModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/corpseModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/currencyModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/foodModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/furnitureModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/keyModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/lightModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/liquidContainerModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/mechanismModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/nodeModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/projectileModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/resourceModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/ropeModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/seedModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/shieldModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/shopModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/toolModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/vehicleModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/magazineModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/meleeWeaponModel.cpp
        ${CMAKE_SOURCE_DIR}/src/model/submodel/rangedWeaponModel.cpp
        PARENT_SCOPE
        )