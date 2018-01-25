/// @file  nameGenerator.hpp
/// @brief A fantasy name generator library.
/// @version 1.0.1
/// @copyright Public Domain
/// @author German M. Bravo (Kronuz)
/// This library is designed after the RinkWorks Fantasy Name Generator.
/// @see http://www.rinkworks.com/namegen/
///
/// @example
/// NameGen::Generator generator("sV'i");
/// generator.toString();  // Emits a new name on each call
/// // => "entheu'loaf"
///
/// ## Pattern Syntax
///
///   The compile() function creates a name generator based on an input
/// pattern. The letters s, v, V, c, B, C, i, m, M, D, and d represent
/// different types of random replacements. Everything else is emitted
/// literally.
///
///   s - generic syllable
///   v - vowel
///   V - vowel or vowel combination
///   c - consonant
///   B - consonant or consonant combination suitable for beginning a word
///   C - consonant or consonant combination suitable anywhere in a word
///   i - insult
///   m - mushy name
///   M - mushy name ending
///   D - consonant suited for a stupid person's name
///   d - syllable suited for a stupid person's name (begins with a vowel)
///
///   All characters between parenthesis () are emitted literally. For
/// example, the pattern "s(dim)", emits a random generic syllable
/// followed by "dim".
///
///   Characters between angle brackets <> emit patterns from the table
/// above. Imagine the entire pattern is wrapped in one of these.
///
///   In both types of groupings, a vertical bar | denotes a random
/// choice. Empty groups are allowed. For example, "(foo|bar)" emits
/// either "foo" or "bar". The pattern "<c|v|>" emits a constant,
/// vowel, or nothing at all.
///
///   An exclamation point ! means to capitalize the component that
/// follows it. For example, "!(foo)" will emit "Foo" and "v!s" will
/// emit a lowercase vowel followed by a capitalized syllable, like
/// "eRod".
///
///   A tilde ~ means to reverse the letters of the component that
/// follows it. For example, "~(foo)" will emit "oof". To reverse an
/// entire template, wrap it in brackets. For example, to reverse
/// "sV'i" as a whole use "~<sV'i>". The template "~sV'i" will only
/// reverse the initial syllable.
///
/// ## Internals
///
///   A name generator is anything with a toString() method, including,
/// importantly, strings themselves. The generator constructors
/// (Random, Sequence) perform additional optimizations when *not* used
/// with the `new` keyword: they may pass through a provided generator,
/// combine provided generators, or even return a simple string.
///
///   New pattern symbols added to Generator.symbols will automatically
/// be used by the compiler.

#pragma once

#include <stddef.h>       // for size_t
#include <iosfwd>         // for wstring
#include <memory>         // for unique_ptr
#include <stack>          // for stack
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

/// @brief Contains the classes used to generate a random name from a pattern.
namespace namegen
{

/// @brief Used to generate random names given a pattern.
class NameGenerator
{
public:
    /// @brief Constructor.
    NameGenerator();

    /// @brief Constructor.
    NameGenerator(const std::string & pattern, bool collapse_triples = true);

    /// @brief Constructor.
    NameGenerator(std::vector<std::unique_ptr<NameGenerator>> && _generators);

    /// @brief Destructor.
    virtual ~NameGenerator() = default;

    /// @brief Returns the total number of combinations.
    virtual size_t combinations();

    /// @brief Returns the minimum length of the generated name.
    virtual size_t min();

    /// @brief Returns the maximum length of the generated name.
    virtual size_t max();

    /// @brief Prints the generated name.
    virtual std::string toString();

    /// @brief Adds a new generator to the list of generators.
    void add(std::unique_ptr<NameGenerator> && g);

protected:
    /// The internal list of generators.
    std::vector<std::unique_ptr<NameGenerator>> generators;

    /// The type of the map of symbols.
    using SymbolMap = std::unordered_map<std::string, const std::vector<std::string>>;

    /// @brief Returns the symbol map.
    static const SymbolMap & getSymbolMap();

private:

    /// Allows to manipulate an element.
    using Manipulator = enum class Manipulators
    {
        None,       ///< No manipulation
        Capitalize, ///< Capitalize the next element.
        Reverse     ///< Reverse the next element.
    };

    /// Determines the type of a group of letters.
    using GroupType = enum class GroupTypes
    {
        None,       ///< No manipulation
        Symbol,     ///< Capitalize the next element.
        Literal     ///< Reverse the next element.
    };

    /// @brief Class used to group letters.
    class Group
    {
    public:
        /// @brief The type of the group.
        GroupType groupType;

        /// @brief Constructor.
        Group(const GroupType & _groupType);

        /// @brief Returns the content of the group.
        std::unique_ptr<NameGenerator> emit();

        /// @brief Split the group.
        void split();

        /// @brief Wrap the group with the given manipulator.
        void wrap(const Manipulator & manipulator);

        /// @brief Add a generator to the internal set.
        void add(std::unique_ptr<NameGenerator> && g);

        /// @brief Creates a new generator with a single Literal in it (i.e. c).
        virtual void add(char c);

    private:
        /// @brief List of manipulators which need to be applied.
        std::stack<Manipulator> manipulators;
        /// @brief A vectors of generators.
        std::vector<std::unique_ptr<NameGenerator>> set;
    };

    /// @brief A group of symbols.
    class GroupSymbol :
        public Group
    {
    public:
        /// @brief Constructor.
        GroupSymbol();

        /// @brief Retrieves the set of strings associated with the given
        /// symbol and add all of them as Literals to the list of generators.
        void add(char c);
    };

    /// @brief A group of literals.
    class GroupLiteral :
        public Group
    {
    public:
        /// @brief Constructor.
        GroupLiteral();
    };
};

/// @brief Used to pick a generator randomly from the internal list of
/// generator.
class Random :
    public NameGenerator
{
public:
    /// @brief Constructor.
    Random();

    /// @brief Constructor which takes a list of generators.
    Random(std::vector<std::unique_ptr<NameGenerator>> && generators_);

    size_t combinations() override;

    size_t min() override;

    size_t max() override;

    std::string toString() override;
};

/// @brief Class used to store a sequence of generators.
class Sequence :
    public NameGenerator
{
public:
    /// @brief Constructor.
    Sequence();

    /// @brief Constructor which takes a list of generators.
    Sequence(std::vector<std::unique_ptr<NameGenerator>> && generators_);
};

/// @brief Class used to store a literal.
class Literal :
    public NameGenerator
{
    /// The string containing the single literal.
    std::string value;

public:
    /// @brief Constructor which takes the string containing the literal.
    Literal(const std::string & value_);

    size_t combinations() override;

    size_t min() override;

    size_t max() override;

    std::string toString() override;
};

/// @brief Class used to reverse a string.
class Reverser :
    public NameGenerator
{
public:
    /// @brief Constructor which takes a generator.
    Reverser(std::unique_ptr<NameGenerator> && g);

    std::string toString() override;
};

/// @brief Class used to capitalize a string.
class Capitalizer :
    public NameGenerator
{
public:
    /// @brief Constructor which takes a generator.
    Capitalizer(std::unique_ptr<NameGenerator> && g);

    std::string toString() override;
};

/// @brief Class used to collapse a string.
class Collapser :
    public NameGenerator
{
public:
    /// @brief Constructor which takes a generator.
    Collapser(std::unique_ptr<NameGenerator> && g);

    std::string toString() override;
};

}
