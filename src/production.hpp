/// @file   production.hpp
/// @brief  Define production variables and methods.
/// @author Enrico Fraccaroli
/// @date   Aug 11 2015
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

#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include "model.hpp"
class Profession;
class Character;

#include <string>
#include <map>

/// @brief Holds details about a production.
class Production
{
    public:
        /// The virtual number of the production.
        int vnum;
        /// The name of the production.
        std::string name;
        /// The profession needed to perform the production.
        Profession * profession;
        /// The difficulty of the production.
        unsigned int difficulty;
        /// The time required to perform the production.
        unsigned int time;
        /// A flag which indicates if the maker can be assisted by someone.
        bool assisted;
        /// The outcome and its quantity.
        std::pair<Model *, unsigned int> outcome;
        /// The list of needed type of tools.
        ToolSet tools;
        /// The list of needed type of ingredients and their quantity.
        IngredientMap ingredients;
        /// The type of the workbench.
        ToolType workbench;
        /// The ingredient from which the material of the production is choosen.
        ResourceType material;

        /// @brief Constructor.
        Production();

        /// @brief Disable Copy Construct.
        Production(Production const &) = delete;

        /// @brief Disable Move construct.
        Production(Production &&) = delete;

        /// @brief Disable Copy assign.
        Production & operator=(Production const &) = delete;

        /// @brief Disable Move assign.
        Production & operator=(Production &&) = delete;

        /// @brief Destructor.
        ~Production();

        /// @brief Given a predefined formatted string, the function sets the outcome.
        /// @param source The source string.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool setOutcome(const std::string & source);

        /// @brief Given a predefined formatted string, the function sets the tools.
        /// @param source The source string.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool setTool(const std::string & source);

        /// @brief Given a predefined formatted string, the function sets the ingredients.
        /// @param source The source string.
        /// @return <b>True</b> if the operation concluded successfuly,<br>
        ///         <b>False</b> otherwise.
        bool setIngredient(const std::string & source);

        /// @brief Check the correctness of the production.
        /// @return <b>True</b> if the profession has correct values,<br> <b>False</b> otherwise.
        bool check();

        /// @brief Return the name of the production.
        /// @return The name of the production.
        std::string getName();

        /// @brief Return the name of the production with the first letter capitalized.
        /// @return The name of the production capitalized.
        std::string getNameCapital();

};

/// Production map handler.
typedef std::map<int, Production *> ProductionMap;

#endif
