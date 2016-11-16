#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ./src/model/submodel
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ./src/model/submodel/armorModel.cpp
        ./src/model/submodel/bookModel.cpp
        ./src/model/submodel/containerModel.cpp
        ./src/model/submodel/corpseModel.cpp
        ./src/model/submodel/currencyModel.cpp
        ./src/model/submodel/foodModel.cpp
        ./src/model/submodel/furnitureModel.cpp
        ./src/model/submodel/keyModel.cpp
        ./src/model/submodel/lightModel.cpp
        ./src/model/submodel/liquidContainerModel.cpp
        ./src/model/submodel/mechanismModel.cpp
        ./src/model/submodel/nodeModel.cpp
        ./src/model/submodel/projectileModel.cpp
        ./src/model/submodel/resourceModel.cpp
        ./src/model/submodel/ropeModel.cpp
        ./src/model/submodel/seedModel.cpp
        ./src/model/submodel/shieldModel.cpp
        ./src/model/submodel/shopModel.cpp
        ./src/model/submodel/toolModel.cpp
        ./src/model/submodel/vehicleModel.cpp
        ./src/model/submodel/magazineModel.cpp
        ./src/model/submodel/meleeWeaponModel.cpp
        ./src/model/submodel/rangedWeaponModel.cpp
        PARENT_SCOPE
        )