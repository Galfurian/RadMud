#############################################
# Include Directories
set(RADMUD_ITEM_INC
        ./src/item
        ./src/item/subtype
        PARENT_SCOPE
        )

#############################################
# Source Files
set(RADMUD_ITEM_SRC
        ./src/item/item.cpp
        ./src/item/writing.cpp
        ./src/item/itemContainer.cpp
        ./src/item/subtype/shopItem.cpp
        ./src/item/subtype/armorItem.cpp
        ./src/item/subtype/corpseItem.cpp
        ./src/item/subtype/currencyItem.cpp
        ./src/item/subtype/meleeWeaponItem.cpp
        ./src/item/subtype/rangedWeaponItem.cpp
        ./src/item/subtype/magazineItem.cpp
        PARENT_SCOPE
        )