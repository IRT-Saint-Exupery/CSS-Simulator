------------------------------------------------------------------
--
-- Cortex modem (Safran) protocol dissector
-- TC  protocol
--
-- Written by Pierre Bacquet, Thales Alenia Space
--
-- To Do:
-- * Handle the case of Cortex messag split over several TCP segments
--   or TCP segments containing several Cortex messages
--
------------------------------------------------------------------

--
-- Get common definitions and helper functions
--
require("Cortex_common")

local ifType = {
    PHYSICAL = 0,
    CODING   = 1
}

local default_settings =
{
    enabled      = true,           	-- whether this dissector is enabled or not
    port         = CORTEX_TC_PORT, 	-- default TCP port number for Cortex TC messages
    max_msg_len  = 1024,           	-- max length of Cortex TC message
	if_type		 = ifType.PHYSICAL,	-- Interface type : physical layer or coding layer
    subdissect   = true,           	-- whether to call sub-dissector or not
	subdissector = "tc_cltu_02",   	-- sub-dissector to be called (TC_cltu_02 if randomisation in use)
    debug_level  = debugLevel.DISABLED,	
}

local scramblingModeValues = {
	[0] = "Clear TC",
	[1] = "Scrambled TC"
	}
	
local tc_debug = debug.new(default_settings.debug_level) 
	
--
-- TC protocol
--
	
cortex_tc_protocol = Proto("Cortex_TC",  "Cortex TC Protocol")

-- a table of all of our Protocol's fields
local tc_fields =
{	
	-- Header
	preamble = ProtoField.int32("cortex.preamble", "Preamble", base.DEC),
	messageLength = ProtoField.int32("cortex.msgLength", "Message length (bytes)", base.DEC),
	flowId = ProtoField.int32("cortex.flowId", "Flow Id", base.DEC),
	-- Message
	reqCode = ProtoField.int32("cortex.requestCode", "Request code", base.DEC),
	loggingWord = ProtoField.uint32("cortex.loggingWord", "Command tag", base.DEC),
	TCmsgLength = ProtoField.int32("cortex.TCmsgLength", "TC message length (bits)", base.DEC),
	radTime = ProtoField.absolute_time("cortex.tcRadTime", "TC latest radiation time", base.UTC),
	scramblingEnable = ProtoField.uint32("cortex.scramblingEnable", "Scrambling enable", base.DEC, scramblingModeValues),
	unused = ProtoField.none("cortex.unused", "Unused", base.NONE),
	firstScrambledBit = ProtoField.uint32("cortex.1stScrambledBit", "Position of the 1st bit to scramble", base.DEC),
	acquisitionSeq = ProtoField.none("cortex.acquisitionSequence", "Acquisition sequence", base.NONE),
	theTC = ProtoField.none("cortex.TeleCommand", "CLTU", base.NONE),
	idleSeq = ProtoField.none("cortex.idleSequence", "Idle sequence", base.NONE),
	padding = ProtoField.none("cortex.TC_padding", "Cortext TC padding", base.NONE),
	verificationTO = ProtoField.uint32("cortex.verifTimeOut", "Verification time-out (ms)", base.DEC),
	maxRetries = ProtoField.uint32("cortex.maxRetries", "Maximum number of retries", base.DEC),
	numTCWords = ProtoField.uint32("cortex.numTCWords", "Number of TM words to verify", base.DEC),
	wordOffset = ProtoField.uint32("cortex.wordOffset", "Word offset (bytes)", base.DEC),
	wordMask = ProtoField.uint32("cortex.wordMask", "Mask for word", base.HEX),
	wordValue = ProtoField.uint32("cortex.wordValue", "Expected value for word", base.HEX),
	numPulses = ProtoField.uint32("cortex.numPulses", "Number of pulses", base.DEC),
	pulseWidth = ProtoField.uint32("cortex.pulseWidth", "Pulse width", base.DEC),
	pulsePeriod = ProtoField.uint32("cortex.pulsePeriod", "Pulse period", base.DEC),
	pausePeriod = ProtoField.uint32("cortex.pausePeriod", "Pause period", base.DEC),
	waitUntil = ProtoField.absolute_time("cortex.tcWaitUntil", "Next operation time", base.UTC),
	numTC = ProtoField.uint32("cortex.numTC", "Number of TCs", base.DEC),
	dataField = ProtoField.uint32("cortex.dataField", "Data field ", base.HEX),
	maskField = ProtoField.uint32("cortex.maskField", "Mask field ", base.HEX),
	-- For acknowledgments
	respData = ProtoField.uint32("cortex.tcRespData", "Response data", base.DEC),
	respTime = ProtoField.absolute_time("cortex.tcRespTime", "Acknowledgement time", base.UTC),
	respStatus = ProtoField.uint32("cortex.tcRespStatus", "Response status", base.DEC),
	-- Common to TC messages and their acknowledgments
	checkSum = ProtoField.uint32("cortex.TCcheckSum", "TC checksum", base.HEX),
	-- Trailer
	postamble = ProtoField.int32("cortex.postamble", "Postamble", base.DEC),
}

-- Register our fields
cortex_tc_protocol.fields = tc_fields

local tc_subdissector = nil

local function enableTC_SubDissector()
	tc_subdissector = Dissector.get(default_settings.subdissector)

	if tc_subdissector == nil then
		tc_debug:show(debugLevel.LEVEL_1, default_settings.subdissector .. " subdissector not found")
	end

end

local function get_TC_codeName(tc_code)
  local opcode_name = "Unknown"

      if tc_code ==  1 then opcode_name = "Clear TC"
  elseif tc_code == 21 then opcode_name = "Clear TC with latest radiation time"
  elseif tc_code == 14 then opcode_name = "Scrambled TC"
  elseif tc_code ==  3 then opcode_name = "Wait and verify"
  elseif tc_code ==  4 then opcode_name = "Execute"
  elseif tc_code ==  5 then opcode_name = "Pause"
  elseif tc_code ==  6 then opcode_name = "Wait for absolute time"
  elseif tc_code ==  7 then opcode_name = "TCU unlock"
  elseif tc_code ==  8 then opcode_name = "NOP"
  elseif tc_code ==  9 then opcode_name = "Stop idling"
  elseif tc_code == 10 then opcode_name = "Group"
  elseif tc_code == 18 then opcode_name = "Data+Execute" end

  return opcode_name
end

local function get_RespStatus(code, status)
  local retStatusName = "Unknown"

  --  To Do: "switchify" me if possible
      if status ==  0 then retStatusName = "Successful"
  elseif status ==  1 then retStatusName = "TC encoder locked out"
  elseif status ==  2 then retStatusName = "Invalid check-sum or invalid syntax"
  elseif status ==  3 then 
    if code == 10 then
	  retStatusName = "Corrupted data in the group of satellite TC requests"
	else
	  retStatusName = "Verification failed after retries"
    end
  elseif status ==  4 then
    if code == 10 then
	  retStatusName = "New « Group » instruction received"
	else
	  retStatusName = "TC Scrambler mounted & not responding or CMM1 checking failed (time-out)"
    end
  elseif status ==  5 then retStatusName = "CMM2 checking failed"
  elseif status ==  6 then retStatusName = "Group rejected"
  elseif status ==  7 then retStatusName = "TCU failure"
  elseif status ==  8 then retStatusName = "Bad TC demodulation"
  elseif status == 11 then retStatusName = "Bad TC chain configuration (TCU not selected on IFM)"
  elseif status == 12 then retStatusName = "Latest radiation time reached"
  end
 
  return retStatusName
end

local function dissectTC_checksum(buffer, payloadSubtree, msgLength)
  -- Display and verify checksum
  local checkSum = 0
  local i = 0
  local buffOffset = 0
  local numWords = msgLength/4
  for i = 1,numWords,1 do
    checkSum = checkSum + buffer(buffOffset, 4):uint()
	buffOffset = buffOffset + 4
  end
  checkSum = bit.band(checkSum, 0xFFFFFFFF)
  local checkSumOK = "OK"
  if checkSum ~= 0 then checkSumOK = "NOK" end
  payloadSubtree:add(tc_fields.checkSum, buffer(msgLength - 8, 4)):append_text(" (" .. checkSumOK .. ")")
end

local function dissectTC_response(buffer, payloadSubtree, tc_code_number)

  payloadSubtree:add(tc_fields.respData, buffer(16, 4))
  payloadSubtree:add(tc_fields.respTime, buffer(20, 8), UTC_with_msec(buffer(20, 8)))

  local code = buffer(12, 4):uint()
  local status = buffer(28, 4):uint()
  payloadSubtree:add(tc_fields.respStatus, buffer(28, 4)):append_text(" (" .. get_RespStatus(code, status) .. ")")

  -- Verify checksum
  dissectTC_checksum(buffer, payloadSubtree, 40)
end

local function dissectTC_request(buffer, payloadSubtree, tc_code_number, pinfo, msgLength)
  local trailerOffset = 16
  
  if tc_code_number == 1 or tc_code_number == 14 or tc_code_number == 21 then
	-- Requests including TC data
	payloadSubtree:add(tc_fields.loggingWord, buffer(trailerOffset, 4))
	trailerOffset = trailerOffset + 4
	-- Decode TC message lenght in bits
	payloadSubtree:add(tc_fields.TCmsgLength, buffer(trailerOffset, 4))
	local TC_length = buffer(trailerOffset, 4):uint()		-- Cortex uses bits
	TC_length = TC_length / 8								-- Now we have bytes
	trailerOffset = trailerOffset + 4
	if tc_code_number == 21 then
	  -- Clear TC with latest radiation time, just add the said radiation time to the tree
	  payloadSubtree:add(tc_fields.radTime, buffer(trailerOffset, 8), UTC_with_msec(buffer(trailerOffset, 8)))
	  trailerOffset = trailerOffset + 8
	elseif tc_code_number == 14 then
	  -- Scrambled TC
	  local tc_mode = buffer(trailerOffset, 4):uint()
	  payloadSubtree:add(tc_fields.scramblingEnable, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.unused, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.firstScrambledBit, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4	end
	-- Common dissection for TC whether they are scrambled or not
	-- Take care of Cortex padding TC on 32 bytes words
	local TC_paddedLength = msgLength - trailerOffset - 8 
	if default_settings.if_type == ifType.PHYSICAL then
		-- Display acquisition sequence (22 times 0x55) 
		payloadSubtree:add(tc_fields.acquisitionSeq, buffer(trailerOffset, 22))
		trailerOffset = trailerOffset + 22
		TC_length = TC_length - 24		-- 22 bytes of acquisition sequence and 2 bytes of idle sequence
	end
	-- Subdissect TC CLTU if enabled
	if default_settings.subdissect then
		if tc_subdissector == nil then
		  enableTC_SubDissector()
		end
		-- CCSDS dissector starts at coding layer with a start sequence = 0xeb90
		local tcFrameTree = payloadSubtree:add(cortex_tc_protocol, buffer(trailerOffset, TC_length), "CLTU")
		tc_subdissector(buffer(trailerOffset, TC_length):tvb(), pinfo, tcFrameTree)
	else
		payloadSubtree:add(tc_fields.theTC, buffer(trailerOffset, TC_length))
	end
	trailerOffset = trailerOffset + TC_length
	if default_settings.if_type == ifType.PHYSICAL then
		-- Display idle sequence (2 x 0x55) 
		payloadSubtree:add(tc_fields.idleSeq, buffer(trailerOffset, 2))
		trailerOffset = trailerOffset + 2
	end
  elseif tc_code_number == 3 then
	  -- Wait and verify instruction
	  payloadSubtree:add(tc_fields.verificationTO, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.maxRetries, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  local numWords = buffer(trailerOffset, 4):uint()
	  local i
 	  payloadSubtree:add(tc_fields.numTCWords, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  for i = 0,numWords-1,1 do
	    local wordName = "Word " .. tostring(i)
		local groupSubtree = payloadSubtree:add(cortex_tc_protocol, buffer(trailerOffset,12), wordName) 	    
		groupSubtree:add(tc_fields.wordOffset, buffer(trailerOffset, 4))
	    trailerOffset = trailerOffset + 4
		groupSubtree:add(tc_fields.wordMask, buffer(trailerOffset, 4))
	    trailerOffset = trailerOffset + 4
		groupSubtree:add(tc_fields.wordValue, buffer(trailerOffset, 4))
	    trailerOffset = trailerOffset + 4
	  end
  elseif tc_code_number == 4 then
	  -- Execute instruction
	  payloadSubtree:add(tc_fields.numPulses, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.pulseWidth, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.pulsePeriod, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
  elseif tc_code_number == 5 then
	  -- Pause instruction
	  payloadSubtree:add(tc_fields.pausePeriod, buffer(trailerOffset, 4))
  	  trailerOffset = trailerOffset + 4
  elseif tc_code_number == 6 then
	  -- Wait for absolute time instruction
	  payloadSubtree:add(tc_fields.waitUntil, buffer(trailerOffset, 8), UTC_with_msec(buffer(trailerOffset, 8)))
  	  trailerOffset = trailerOffset + 8
  elseif tc_code_number == 10 then
	  -- Group instruction
	  payloadSubtree:add(tc_fields.numTC, buffer(trailerOffset, 4))
  	  trailerOffset = trailerOffset + 4
  elseif tc_code_number == 18 then
	  -- Data+Exec instruction
	  payloadSubtree:add(tc_fields.loggingWord, buffer(trailerOffset, 4))
  	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.TCmsgLength, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.pulseWidth, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  payloadSubtree:add(tc_fields.pulsePeriod, buffer(trailerOffset, 4))
	  trailerOffset = trailerOffset + 4
	  -- Determine number of words
	  local numWords = (buffer:len() - trailerOffset - 8)/8
	  local dataFieldsSubtree = payloadSubtree:add(cortex_tc_protocol, buffer(trailerOffset,4 * numWords), "Data fields")
	  for i = 0,numWords-1,1 do
	    dataFieldsSubtree:add(tc_fields.dataField, buffer(trailerOffset, 4)):prepend_text("[" .. tostring(i) .. "] ")
	    trailerOffset = trailerOffset + 4
	  end
	  
	  local maskFieldsSubtree = payloadSubtree:add(cortex_tc_protocol, buffer(trailerOffset,4 * numWords), "Mask fields")
	  for i = 0,numWords-1,1 do
	    maskFieldsSubtree:add(tc_fields.maskField, buffer(trailerOffset, 4)):prepend_text("[" .. tostring(i) .. "] ")
	    trailerOffset = trailerOffset + 4
	  end
  end  
  -- Display and verify checksum
  dissectTC_checksum(buffer, payloadSubtree, msgLength)

end

function cortex_tc_protocol.dissector(buffer, pinfo, tree)
  length = buffer:len()
  if length == 0 then return end

  pinfo.cols.protocol = cortex_tc_protocol.name

  local subtree = tree:add(cortex_tc_protocol, buffer(), "Cortex TC Protocol Data")
  local headerSubtree = subtree:add(cortex_tc_protocol, buffer(0,12), "Header")
  local payloadSubtree = subtree:add(cortex_tc_protocol, buffer(12,length-16), "Payload")
  local trailerSubtree = subtree:add(cortex_tc_protocol, buffer(length-4,4), "Trailer") 
 
  -- Display preamble information
  local preambleCode = buffer(0,4):int()
  local preambleOK
  if preambleCode == 1234567890 then preambleOK = "OK" else preambleOK = "KO" end
 
  headerSubtree:add(tc_fields.preamble, buffer(0,4)):append_text(" (" .. preambleOK .. ")")
  local msgLength = buffer(4,4):uint()
  headerSubtree:add(tc_fields.messageLength, buffer(4,4))  
  headerSubtree:add(tc_fields.flowId, buffer(8,4))
  
  -- Decode and display TC code
  local tc_code_number = buffer(12,4):uint()
  local tc_code_name = get_TC_codeName(tc_code_number)
  
  -- Simple heuristic to determine if it is a request or response without considering
  -- the source / destination port. Responses are always 40 bytes and any request doesn't have
  -- this size
  local isResponse
  local xtraDescription = ""
  if msgLength == 40 then
    isResponse = 1
	xtraDescription = " acknowledgment"
  else
    isResponse = 0
  end  
  local fieldTitle = " (" .. tc_code_name .. xtraDescription
  payloadSubtree:append_text(fieldTitle .. " message)")
  payloadSubtree:add(tc_fields.reqCode, buffer(12,4)):append_text(fieldTitle ..")")

  if isResponse == 1 then
    dissectTC_response(buffer, payloadSubtree, tc_code_number)
  else
    dissectTC_request(buffer, payloadSubtree, tc_code_number, pinfo, msgLength)
  end
	
  -- Decode postamble  
  local postambleCode = buffer(length-4,4):int()
  local postambleOK
  if postambleCode == -1234567890 then postambleOK = "OK" else postambleOK = "KO" end
  trailerSubtree:add(tc_fields.postamble, buffer(length-4,4)):append_text(" (" .. postambleOK .. ")")
end

local function enableTC_Dissector()
    -- Using DissectorTable:set() removes existing dissector(s), whereas the
    -- DissectorTable:add() one adds ours before any existing ones, but
    -- leaves the other ones alone, which is better
    DissectorTable.get("tcp.port"):add(default_settings.port, cortex_tc_protocol)
end
-- Call it now, because we're enabled by default
enableTC_Dissector()

local function disableTC_Dissector()
    DissectorTable.get("tcp.port"):remove(default_settings.port, cortex_tc_protocol)
end

local debug_pref_enum = {
    { 1,  "Disabled", debugLevel.DISABLED },
    { 2,  "Level 1",  debugLevel.LEVEL_1  },
    { 3,  "Level 2",  debugLevel.LEVEL_2  },
}

local if_type_enum = {
    { 1,  "Physical layer", ifType.PHYSICAL },
    { 2,  "Coding layer",  ifType.CODING  }
}

-- Register our preferences
cortex_tc_protocol.prefs.enabled     = Pref.bool("Dissector enabled", default_settings.enabled,
                                        "Whether the FPM dissector is enabled or not")

cortex_tc_protocol.prefs.port     = Pref.uint("TC port", default_settings.port,
                                        "Define the TCP port used for TeleCommands")

cortex_tc_protocol.prefs.if_type     = Pref.enum("I/F type", default_settings.if_type,
                                        "Define if TCs are sent at physical or coding layer", if_type_enum)
										
cortex_tc_protocol.prefs.subdissect  = Pref.bool("Enable sub-dissectors", default_settings.subdissect,
                                        "Whether the Cortex TC packet's content" ..
                                        " should be dissected or not using CCSDS dissector")

cortex_tc_protocol.prefs.subdissector  = Pref.string("CCSDS TC dissector", default_settings.subdissector,
                                        "CCSDS dissector to call to dissect TC")
										
cortex_tc_protocol.prefs.debug       = Pref.enum("Debug", default_settings.debug_level,
                                        "The debug printing level", debug_pref_enum)
										
function cortex_tc_protocol.prefs_changed()

    default_settings.subdissect  = cortex_tc_protocol.prefs.subdissect
	default_settings.debug_level = cortex_tc_protocol.prefs.debug
	default_settings.if_type = cortex_tc_protocol.prefs.if_type
	tc_debug:setLevel(default_settings.debug_level)	

    if default_settings.enabled ~= cortex_tc_protocol.prefs.enabled then
        default_settings.enabled = cortex_tc_protocol.prefs.enabled
        if default_settings.enabled then
            enableTC_Dissector()
        else
            disableTC_Dissector()
        end
        -- have to reload the capture file for this type of change
        reload()
    end

	if default_settings.port ~= cortex_tc_protocol.prefs.port then
		if default_settings.enabled then
			disableTC_Dissector()
			default_settings.port = cortex_tc_protocol.prefs.port
			enableTC_Dissector()
			-- have to reload the capture file for this type of change
			reload()
		else
			default_settings.port = cortex_tc_protocol.prefs.port
		end
    end
	
	if default_settings.subdissector ~= cortex_tc_protocol.prefs.subdissector then
		default_settings.subdissector = cortex_tc_protocol.prefs.subdissector
		enableTC_SubDissector()
		-- have to reload the capture file for this type of change
		reload()
    end
	
end
