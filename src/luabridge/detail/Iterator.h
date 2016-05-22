///----------------------------------------------------------------------------
/// @file   Iterator.h
/// @copyright
/// Copyright 2012, Vinnie Falco <vinnie.falco@gmail.com>
///
/// License: The MIT License (http://www.opensource.org/licenses/mit-license.php)
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///----------------------------------------------------------------------------

/** Allows table iteration.
 */
class Iterator
{
    private:
        lua_State* m_L;
        LuaRef m_table;
        LuaRef m_key;
        LuaRef m_value;

    public:
        explicit Iterator(LuaRef table) :
                m_L(table.state()),
                m_table(table),
                m_key(table.state()),
                m_value(table.state())
        {
            //m_table.push (
        }

        lua_State* state() const
        {
            return m_L;
        }

        LuaRef operator*() const
        {
            return m_value;
        }

        LuaRef operator->() const
        {
            return m_value;
        }

        Iterator& operator++()
        {
            return *this;
        }

        inline bool isNil() const
        {
            return m_key.isNil();
        }

        inline LuaRef key() const
        {
            return m_key;
        }

        inline LuaRef value() const
        {
            return m_value;
        }

    private:
        // Don't use postfix increment, it is less efficient
        Iterator operator++(int);
};

