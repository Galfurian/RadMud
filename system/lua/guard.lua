local utils = require("utils")
require("mud")

-- Handle the actions when the character is created.
EventInit = function(self)
    -- Put event code here.
    local helm = mud.load_item(self, 1500, 10, mud.random(1, 5))
    if (helm ~= nil) then
        self:equipmentAdd(helm)
    end
    local cuirass = mud.load_item(self, 1501, 10, mud.random(1, 5))
    if (cuirass ~= nil) then
        self:equipmentAdd(cuirass)
    end
    local greave = mud.load_item(self, 1502, 10, mud.random(1, 5))
    if (greave ~= nil) then
        self:equipmentAdd(greave)
    end
    local boots = mud.load_item(self, 1503, 10, mud.random(1, 5))
    if (boots ~= nil) then
        self:equipmentAdd(boots)
    end
end

-- Handle the actions when in combat.
EventFight = function(self, character)
    -- Put event code here.
end

local enter_state_look = 0
local enter_state_speak = 1
local enter_state_again = 2
local enter_state_char = { }

-- Handle the actions when a character enters the room.
EventEnter = function(self, character)
    if (enter_state_char[character.name] == nil) then
        enter_state_char[character.name] = { }
        enter_state_char[character.name].state = enter_state_look
        enter_state_char[character.name].counter = 0
    end
    if (enter_state_char[character.name].state == enter_state_look) then
        self:doCommand("look " .. character.name)
        enter_state_char[character.name].state = enter_state_speak
    elseif (enter_state_char[character.name].state == enter_state_speak) then
        if (enter_state_char[character.name].counter >= 3) then
            self:doCommand("say " .. character.name .. " Stand aside, citizen!")
            enter_state_char[character.name].state = enter_state_again
            enter_state_char[character.name].counter = 0
        end
        enter_state_char[character.name].counter = enter_state_char[character.name].counter + 1
    elseif (enter_state_char[character.name].state == enter_state_again) then
        if (enter_state_char[character.name].counter >= 3) then
            self:doCommand("say " .. character.name .. " Are you still here?!?")
            enter_state_char[character.name].state = enter_state_look
            enter_state_char[character.name].counter = 0
            return true
        end
        enter_state_char[character.name].counter = enter_state_char[character.name].counter + 1
    end
    return false
end

-- Handle the actions when a character exits the room.
EventExit = function(self, character)
    print("EventExit")
    if (enter_state_char[character.name] ~= nil) then
        if (enter_state_char[character.name].state == enter_state_look) then
            self:doCommand("nod " .. character.name)
        elseif (enter_state_char[character.name].state == enter_state_speak) then
            self:doCommand("say " .. character.name .. " Wise choice...")
        end
    end
    return true
end

-- Handle conversation.
EventMessage = function(self, character, message)
    -- Put event code here.
end

-- Handle a random action.
EventRandom = function(self)
    -- Put event code here.
end

-- Handle the actions when is Morning.
EventMorning = function(self)
    -- Put event code here.
end

-- Handle the actions when is the middle of the day.
EventDay = function(self)
    -- Put event code here.
end

-- Handle the actions when is Evening.
EventDusk = function(self)
    -- Put event code here.
end

-- Handle the actions when is Night.
EventNight = function(self)
    -- Put event code here.
end

-- Handle the actions when the mobile dies.
EventDeath = function(self)
    -- Put event code here.
end

-- Handle the main behaviour of the character.
EventMain = function(self)
    -- Put event code here.
end