--- File   : queue.lua
--- Brief  : Implement a simple queue.
--- Author : Enrico Fraccaroli
--- Date   : 27/11/2016
--- Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
--- Permission is hereby granted, free of charge, to any person obtaining a
--- copy of this software and associated documentation files (the "Software"),
--- to deal in the Software without restriction, including without limitation
--- the rights to use, copy, modify, merge, publish, distribute, sublicense,
--- and/or sell copies of the Software, and to permit persons to whom the
--- Software is furnished to do so, subject to the following conditions:
--- The above copyright notice and this permission notice shall be included
--- in all copies or substantial portions of the Software.
--- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
--- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
--- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
--- THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
--- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
--- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
--- DEALINGS IN THE SOFTWARE.
require 'class'

Queue = class(function(queue, name)
    queue.name = name
    queue.first = 0
    queue.last = -1
    queue.elements = {}
end)

function Queue:empty()
    return (self.first > self.last);
end

function Queue:contains(value)
    for key, val in pairs(self.elements) do
        if val == value then
            return true
        end
    end
    return false
end

function Queue:pushfirst(value)
    local first = self.first - 1
    self.first = first
    self.elements[first] = value
end

function Queue:pushlast(value)
    local last = self.last + 1
    self.last = last
    self.elements[last] = value
end

function Queue:popfirst()
    local first = self.first
    if (first > self.last) then
        error("self is empty")
    end
    local value = self.elements[first]
    self.elements[first] = nil -- to allow garbage collection
    self.first = first + 1
    return value
end

function Queue:poplast()
    local last = self.last
    if (self.first > last) then
        error("self is empty")
    end
    local value = self.elements[last]
    self.elements[last] = nil -- to allow garbage collection
    self.last = last - 1
    return value
end

function Queue:reset()
    for key in pairs(self.elements) do
        self.elements[key] = nil
    end
end

function Queue:__tostring()
    --    local content = {}
    --    for k, v in pairs(self.elements) do
    --        content[#content + 1] = v
    --    end
    --    return self.name .. "\t [" .. self.first .. " : " .. self.last .. "]" .. "\t {" .. table.concat(content, ",") .. "}"
    return self.name .. "\t [" .. self.first .. " : " .. self.last .. "]"
end

return Queue