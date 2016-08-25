/*
 * argumentHandler.hpp
 *
 *  Created on: Aug 25, 2016
 *      Author: enrico
 */

#pragma once

#include <vector>
#include <string>

class Argument
{
    public:
        /// The original argument string.
        std::string original;
        /// The string with both the index and the multiplier removed.
        std::string content;
        /// The provided index.
        int index;
        /// The provided multiplier.
        unsigned int multiplier;

    public:
        /// @brief Constructor.
        Argument(const std::string & _original);

        /// @brief Destructor.
        virtual ~Argument();

        std::string getOriginal() const;

        std::string getContent() const;

        int & getIndex();

        unsigned int getMultiplier() const;

    private:
        void evaluateIndex();

        void evaluateMultiplier();
};

class ArgumentHandler
{
    private:
        /// The original string.
        std::string original;
        /// List of arguments.
        std::vector<Argument> arguments;

    public:
        /// @brief Constructor.
        ArgumentHandler(const std::string & _original);

        /// @brief Constructor.
        ArgumentHandler(std::istream & _original);

        /// @brief Destructor.
        virtual ~ArgumentHandler();

        std::string getOriginal();

        size_t size() const;

        bool empty() const;

        Argument & get(const size_t & position);

        Argument & operator[](const size_t & position);

        std::string substr(const size_t & startingArgument);

        void erase(const size_t & position);

    private:
        void evaluateArguments();
};
