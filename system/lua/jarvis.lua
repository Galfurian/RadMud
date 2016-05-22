function MessageContains(message, what)
	return string.match(' '..message..' ', '%W'..what..'%W') ~= nil
end

-- Handle the actions when the character is created.
EventInit=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventInit)");
end

-- Handle the actions when in combat.
EventFight=function(self,character)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventFight) ["..character.name.."]");
end

-- Handle the actions when a character enters the room.
EventEnter=function(self, character)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventEnter) ["..character.name.."]");
	if (Mud.Random(1,10) > 5)
	then
		self:doCommand("look "..character.name);
	end
end

-- Handle the actions when a character exits the room.
EventExit=function(self, character)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventExit) ["..character.name.."]");
end

-- Handle conversation.
EventMessage=function(self, character, message)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventMessage) ["..character.name.."] ["..message.."]");
	if(MessageContains(message, "hi"))
	then
		Mud.Sleep(1);
		self:doCommand("say "..character.name.." hi to you!");
	end
end

-- Handle a random action.
EventRandom=function(self)
	-- Put event code here.
	-- Get the possible directions.
	--local availables = Mud.getDirections(self);
	--if(availables:size() > 0)
	--then
	--	-- Get a random next direction.
	--	local direction = Mud.Random(0, (availables:size()-1));
	--	-- Go to the next direction.
	--	self:doCommand(availables:at(direction));
	--end
end

-- Handle the actions when is Morning.
EventMorning=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventMorning)");
end

-- Handle the actions when is the middle of the day.
EventDay=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventDay)");
end

-- Handle the actions when is Evening.
EventDusk=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventDusk)");
end

-- Handle the actions when is Night.
EventNight=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventNight)");
end

-- Handle the actions when the mobile dies.
EventDeath=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventDeath)");
end

-- Handle the main behaviour of the character.
EventMain=function(self)
	-- Put event code here.
	Mud.Log("["..self.name.."] (EventMain)");
end