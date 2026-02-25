------------------------------------------------------------------
--
-- Cortex modem (Safran) protocol dissector
-- Common definitions
--
-- Written by Pierre Bacquet, Thales Alenia Space
--
------------------------------------------------------------------

--
-- Comon definitions and helper functions
--

-- TCP PORTS per Safran's ICD "STI 100013_CRT (TC = 3020, TM=3070)"
CORTEX_TC_PORT = 3020
CORTEX_TM_PORT = 3070
-- TCP PORTS used by TAS DSS"
--CORTEX_TC_PORT = 18051
--CORTEX_TM_PORT = 18050


OnOffValues = {
	[1] = "On",
	[2] = "Off"
	}
	
-- Representation of UTC time with msec as a 2nd 32 bits word

function UTC_with_msec(subBuffer)
	--Time is encoded as UTC on the 1st 32 bits word  and then msecs on the next 32 bits word
	local secs = subBuffer(0,4):uint()
	local nsecs = subBuffer(4,4):uint()
	
	nsecs = nsecs * 1000000
	
	return NSTime.new(secs, nsecs)
end

-- Debug print class

debugLevel = {
    DISABLED = 0,
    LEVEL_1  = 1,
    LEVEL_2  = 2
}

debug = {}

debug.__index = debug

function debug.new(level)
  local myClass = setmetatable({}, debug)
  
  debug.level_ = level

  return myClass
end

function debug:setLevel(level)	
	self.level_ = level
end

function debug:getLevel()	
	return self.level
end

function debug:show(level, text)
	if level <= self.level_ then
	    print(text)
	end
end
