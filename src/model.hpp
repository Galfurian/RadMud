/// @file   model.hpp
/// @brief  Define item model variables and methods.
/// @author Enrico Fraccaroli
/// @date   Mar 31 2015
/// @copyright
/// Copyright (c) 2015, 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <set>
#include <map>

#include "defines.hpp"
#include "lua/lua_script.hpp"

class Item;
class Player;
class Material;

/// @brief Holds details about a model of item.
class Model
{
    public:
        /// Model model vnum.
        int vnum;
        /// Model name.
        std::string name;
        /// Model article.
        std::string article;
        /// Model short description.
        std::string shortdesc;
        /// Model keys.
        std::vector<std::string> keys;
        /// Model description.
        std::string description;
        /// Model type.
        ModelType type;
        /// Store here the position where the model can be equipped.
        EquipmentSlot slot;
        /// Model flags.
        unsigned int flags;
        /// Model weight.
        unsigned int weight;
        /// Model price.
        int price;
        /// Model maximum condition.
        int condition;
        /// Model maximum condition.
        int decay;
        /// Model's material.
        MaterialType material;
        /// TileSet of the icon.
        int tileSet;
        /// TileId of the icon.
        int tileId;
        /// Model functions.
        std::vector<unsigned int> functions;

        /// @brief Constructor.
        Model();

        /// @brief Disable Copy Construct.
        Model(Model const &) = delete;

        /// @brief Disable Move construct.
        Model(Model &&) = delete;

        /// @brief Disable Copy assign.
        Model & operator=(Model const &) = delete;

        /// @brief Disable Move assign.
        Model & operator=(Model &&) = delete;

        /// @brief Destructor.
        ~Model();

        /// @brief Given a source string, this function parse the string
        ///         and sets the functions.
        /// @param source Contains the list of functions.
        /// @return <b>True</b> if the operation succeeded,<br>
        ///         <b>False</b> otherwise.
        bool setFunctions(std::string source);

        /// @brief Check the correctness of the model.
        /// @return <b>True</b> if the model has correct values,<br><b>False</b> otherwise.
        bool check();

        /// @brief Given the argumtens the function replace the symbols inside the source string
        ///         using the provided arguments.
        /// @param source       The source string.
        /// @param itemMaterial The material of which the model is made.
        /// @param itemQuality  The quality of the model.
        /// @return <b>True</b> if something changed inside the string,<br>
        ///         <b>False</b> otherwise.
        bool replaceSymbols(std::string & source, Material * itemMaterial = nullptr, ItemQuality itemQuality =
            ItemQuality::Normal);

        /// @brief Returns the name of the model depending on the passed arguments.
        /// @param itemMaterial The material of which the model is made.
        /// @param itemQuality  The quality of the model.
        /// @return The specific name of the model.
        std::string getName(Material * itemMaterial = nullptr, ItemQuality itemQuality = ItemQuality::Normal);

        /// @brief Returns the description of the model depending on the passed arguments.
        /// @param itemMaterial The material of which the model is made.
        /// @param itemQuality  The quality of the model.
        /// @return The specific description of the model.
        std::string getDescription(Material * itemMaterial = nullptr, ItemQuality itemQuality = ItemQuality::Normal);

        /// @brief Create a new item starting from this model.
        /// @param maker       The player that create the item.
        /// @param composition The composition of the item.
        /// @param itemQuality The quality of the item.
        /// @return The newly created item.
        Item * createItem(std::string maker, Material * composition, ItemQuality itemQuality);

        /// @brief Check if the item must be wielded.
        /// @return <b>True</b> if the item must be wielded,<br><b>False</b> Otherwise.
        bool mustBeWielded();

        /// @brief Returns the name of the type of model.
        /// @return The name of the model's type.
        std::string getSpecificTypeName();

        /// @defgroup Model functions Extraction Methods.
        /// @brief All the functions necessary to extract infomation regarding the functionality of a model.
        /// @{

        /// Parses the variable "functions" of a model and return information for a Weapon.
        WeaponFunc getWeaponFunc();
        /// Parses the variable "functions" of a model and return information for an Armor.
        ArmorFunc getArmorFunc();
        /// Parses the variable "functions" of a model and return information for a Shield.
        ShieldFunc getShieldFunc();
        /// Parses the variable "functions" of a model and return information for a Projectile.
        ProjectileFunc getProjectileFunc();
        /// Parses the variable "functions" of a model and return information for a Container.
        ContainerFunc getContainerFunc();
        /// Parses the variable "functions" of a model and return information for a Container of Liquids.
        LiqContainerFunc getLiqContainerFunc();
        /// Parses the variable "functions" of a model and return information for a Tool.
        ToolFunc getToolFunc();
        /// Parses the variable "functions" of a model and return information for a Node.
        NodeFunc getNodeFunc();
        /// Parses the variable "functions" of a model and return information for a Resource.
        ResourceFunc getResourceFunc();
        /// Parses the variable "functions" of a model and return information for a Seed.
        SeedFunc getSeedFunc();
        /// Parses the variable "functions" of a model and return information for a Food.
        FoodFunc getFoodFunc();
        /// Parses the variable "functions" of a model and return information for a Light.
        LightFunc getLightFunc();
        /// Parses the variable "functions" of a model and return information for a Book.
        BookFunc getBookFunc();
        /// Parses the variable "functions" of a model and return information for a Rope.
        RopeFunc getRopeFunc();
        /// Parses the variable "functions" of a model and return information for a Mechanism.
        MechanismFunc getMechanismFunc();
        ///@}

        /// @brief Function used to register inside the lua environment the class.
        /// @param L The lua environment.
        static void luaRegister(lua_State * L);

        /// @brief Returns the tile of the model.
        /// @return The string which contains the code of the tile.
        std::string getTile(int offset = 0);
};

/// Model vector handler.
typedef std::vector<Model *> ModelList;

/// Model map handler.
typedef std::map<int, Model *> ModelMap;

#endif
