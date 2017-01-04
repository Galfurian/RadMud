#############################################
# Include Directories
set(PROJECT_INC ${PROJECT_INC}
        ${CMAKE_SOURCE_DIR}/src/item/subitem
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC ${PROJECT_SRC}
        ${CMAKE_SOURCE_DIR}/src/item/subitem/shopItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/lightItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/armorItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/corpseItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/currencyItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/resourceItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/magazineItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/meleeWeaponItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/rangedWeaponItem.cpp
        ${CMAKE_SOURCE_DIR}/src/item/subitem/liquidContainerItem.cpp
        PARENT_SCOPE
        )