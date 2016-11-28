--- File   : explorer.lua
--- Brief  : Implement a structure which allows to easely explore an area.
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
require 'queue'

Explorer = class(function(explorer)
    explorer.alreadyVisited = Queue("AlreadyVisited")
    explorer.notVisited = Queue("NotVisited")
    explorer.invalidRooms = Queue("InvalidRooms")
end)

--- Check if the given table contains the value.
-- @param tab The table.
-- @param roomToFind The room that has to be found.
-- @return If the table contains the value.
function Explorer:findRoomInTable(tab, roomToFind)
    for roomKey, room in pairs(tab) do
        if (room.vnum == roomToFind.vnum) then
            return true
        end
    end
    return false
end

function Explorer:removeRoomFromTable(tab, roomToRemove)
    for roomKey, room in pairs(tab) do
        if (room.vnum == roomToRemove.vnum) then
            tab[roomKey] = nil
            return true
        end
    end
    return false
end

--- Set the given room as visited.
function Explorer:setRoomAsVisited(room)
    if (not self:findRoomInTable(self.alreadyVisited.elements, room)) then
        -- Push the room inside the queue of already visited rooms.
        self.alreadyVisited:pushfirst(room)
        -- Remove the room from the the queue of not visited rooms.
        self:removeRoomFromTable(self.notVisited.elements, room)
        return true
    end
    return false
end

--- Retrieve the rooms which have to be visited.
function Explorer:updateNotVisitedRooms()
    for roomKey, room in pairs(self.alreadyVisited.elements) do
        for eKey, e in pairs(room:getExits()) do
            local destination = e.destination;
            if (not self:findRoomInTable(self.notVisited.elements, destination)) then
                if (not self:findRoomInTable(self.alreadyVisited.elements, destination)) then
                    if (not self:findRoomInTable(self.invalidRooms.elements, destination)) then
                        self.notVisited:pushfirst(destination);
                    end
                end
            end
        end
    end
end

function Explorer:reset()
    self.alreadyVisited:reset()
    self.notVisited:reset()
    self.invalidRooms:reset()
end

function Explorer:print()
    print("Already Visited:")
    for roomKey, room in pairs(self.alreadyVisited.elements) do
        print("    " .. room.vnum)
    end
    print("Not Visited    :")
    for roomKey, room in pairs(self.notVisited.elements) do
        print("    " .. room.vnum)
    end
    print("Invalid Rooms  :")
    for roomKey, room in pairs(self.invalidRooms.elements) do
        print("    " .. room.vnum)
    end
end

function Explorer:__tostring()
    return self.alreadyVisited:__tostring() .. "\n" .. self.notVisited:__tostring()
end

return Explorer