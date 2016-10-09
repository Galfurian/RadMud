#############################################
# Include Directories
set(RADMUD_ITEM_INC
        ./src/item
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_ITEM_SRC
        ./src/item/item.cpp
        ./src/item/shopItem.cpp
        ./src/item/armorItem.cpp
        ./src/item/corpseItem.cpp
        ./src/item/currencyItem.cpp
        ./src/item/itemContainer.cpp
        ./src/item/meleeWeaponItem.cpp
        ./src/item/rangedWeaponItem.cpp
        PARENT_SCOPE
        )