#############################################
# Include Directories
set(PROJECT_INC
        ${PROJECT_INC}
        ./src/item/subitem
        PARENT_SCOPE
        )

#############################################
# Source Files
set(PROJECT_SRC
        ${PROJECT_SRC}
        ./src/item/subitem/shopItem.cpp
        ./src/item/subitem/armorItem.cpp
        ./src/item/subitem/corpseItem.cpp
        ./src/item/subitem/currencyItem.cpp
        ./src/item/subitem/meleeWeaponItem.cpp
        ./src/item/subitem/rangedWeaponItem.cpp
        ./src/item/subitem/magazineItem.cpp
        PARENT_SCOPE
        )