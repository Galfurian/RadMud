/// @file   generalAction.hpp
/// @brief  Defines the base class for any kind of action.
/// @author Enrico Fraccaroli
/// @date   Jul 13 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include <memory>
#include <chrono>

#include "luaBridge.hpp"

/// The list of possible behaviours.
using BehaviourStatus = enum class BehaviourStatus_t
{
    Finished,   ///< The behaviour is finished.
    Running,    ///< The behaviour is still running.
    Error       ///< The behaviour has encountered an error.
};

/// @brief A general behaviour. Performing this behaviour will result in doing nothing.
class GeneralBehaviour
{
protected:
    /// @brief The name of the funciton.
    std::string functionName;
    /// @brief A reference to the function inside Lua.
    luabridge::LuaRef functionRef;

public:
    /// @brief Constructor.
    explicit GeneralBehaviour(const std::string & _functionName,
                              const luabridge::LuaRef & _functionRef) :
        functionName(_functionName),
        functionRef(_functionRef)
    {
        // Nothing to do.
    }

    inline std::string getFunctionName() const
    {
        return functionName;
    }

    /// @brief Destructor.
    virtual ~GeneralBehaviour() = default;

    /// @brief Performs the behaviour.
    /// @return
    virtual BehaviourStatus perform() = 0;

protected:
    inline BehaviourStatus handleReturn(const luabridge::LuaRef & result) const
    {
        if (result.isBool())
        {
            if (result.cast<bool>())
            {
                return BehaviourStatus::Finished;
            }
            else
            {
                return BehaviourStatus::Running;
            }
        }
        return BehaviourStatus::Error;
    }
};

template<class P1>
class BehaviourP1 :
    public GeneralBehaviour
{
private:
    P1 p1;

public:
    BehaviourP1(const std::string & _functionName,
                const luabridge::LuaRef & _func, P1 _p1) :
        GeneralBehaviour(_functionName, _func),
        p1(_p1)
    {
        // Nothing to do.
    }

    inline BehaviourStatus perform() override
    {
        if (functionRef.isFunction())
        {
            return this->handleReturn(functionRef(p1));
        }
        return BehaviourStatus::Error;
    }
};

template<typename P1, typename P2>
class BehaviourP2 :
    public GeneralBehaviour
{
private:
    P1 p1;
    P2 p2;

public:
    BehaviourP2(const std::string & _functionName,
                const luabridge::LuaRef & _func, P1 _p1, P2 _p2) :
        GeneralBehaviour(_functionName, _func),
        p1(_p1),
        p2(_p2)
    {
        // Nothing to do.
    }

    inline BehaviourStatus perform() override
    {
        if (functionRef.isFunction())
        {
            return this->handleReturn(functionRef(p1, p2));
        }
        return BehaviourStatus::Error;
    }
};

template<typename P1, typename P2, typename P3>
class BehaviourP3 :
    public GeneralBehaviour
{
private:
    P1 p1;
    P2 p2;
    P3 p3;

public:
    BehaviourP3(const std::string & _functionName,
                const luabridge::LuaRef & _func, P1 _p1, P2 _p2, P3 _p3) :
        GeneralBehaviour(_functionName, _func),
        p1(_p1),
        p2(_p2),
        p3(_p3)
    {
        // Nothing to do.
    }

    inline BehaviourStatus perform() override
    {
        if (functionRef.isFunction())
        {
            return this->handleReturn(functionRef(p1, p2, p3));
        }
        return BehaviourStatus::Error;
    }
};
