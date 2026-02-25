------------------------------------------------------------------
--
-- Cortex modem (Safran) protocol dissector
-- TM protocol
--
-- Written by Pierre Bacquet, Thales Alenia Space
--
-- To Do:
-- * Handle the case of Cortex messag split over several TCP segments
--   or TCP segments containing several Cortex messages
--
------------------------------------------------------------------

----------------------------------------------------------------------
-- Uncomment next line if bit is not available in your LUA version
--local bit=bit32
----------------------------------------------------------------------

--
-- Get common definitions and helper functions
--
require("Cortex_common")

local default_settings =
{
    enabled      = true,    	   -- whether this dissector is enabled or not
    port         = CORTEX_TM_PORT, -- default TCP port number for Cortex TM messages
    max_msg_len  = 1024,           -- max length of Cortex TM message (bytes)
    subdissect   = true,           -- whether to call sub-dissector or not
	subdissector = "tm_frame_02",	   -- sub-dissector to be called
    debug_level  = debugLevel.LEVEL_1,	
	}

local tm_debug = debug.new(default_settings.debug_level) 

--
-- TM protocol
--	

cortex_tm_protocol = Proto("Cortex_TM",  "Cortex TM Protocol")

-- Various maps to decode bitfiels
local RS_Values = {
	[1] = "data (SW & R-S code removed)",
	[2] = "SW + data + R-S code"
	}

local LDPC_Values = {
	[1] = "Information data + Parity",
	[2] = "Information data only"
	}

local RS_CorrectionValues = {
	[1] = "R-S Correction capacity exceeded",
	[2] = "R-S Correction OK"
	}

local FrameCheckingValues = {
	[1] = "Frame checking ON and frame check error",
	[2] = "Frame checking OFF or no error"
	}

local retCodeValues = {
	[0] = "OK",
	[1] = "Invalid syntax",
	[2] = "M chain not mounted"
	}

local ldpcStatusValues = {
	[0] = "LDPC decoder correction OK",
	[1] = "LDPC Correction capacity exceeded",
	[2] = "Frame checking is ON and frame check error"
	}

-- a table of all of our Protocol's fields
local tm_fields =
{	
	-- Header
	preamble = ProtoField.int32("cortex.preamble", "Preamble", base.DEC),
	messageLength = ProtoField.int32("cortex.msgLength", "Message length (bytes)", base.DEC),
	flowId = ProtoField.int32("cortex.flowId", "Flow Id", base.DEC),
	-- Message
	retCode = ProtoField.int32("cortex.retCode", "Return code", base.DEC, retCodeValues),
	channel = ProtoField.int32("cortex.tmChannel", "TM Channel", base.DEC),
	number = ProtoField.int32("cortex.tmNumber", "Number of TM", base.DEC),
	dataFlow = ProtoField.int32("cortex.tmDataFlow", "Data flow", base.DEC),
	frameMask = ProtoField.uint64("cortex.tmFrameMask", "Frame mask", base.HEX),
	expectedValue = ProtoField.uint64("cortex.tmExpectedValue", "Expected value", base.HEX),
	startTimeTag = ProtoField.absolute_time("cortex.tmStartTimeTag", "TM start time tag", base.UTC),
	endTimeTag = ProtoField.absolute_time("cortex.tmEndTimeTag", "TM end time tag", base.UTC),
	firstFile = ProtoField.int16("cortex.tmFirstFile", "First file to read", base.DEC),
	numFiles = ProtoField.int16("cortex.tmNumFiles", "Number of files to read", base.DEC),
	timeTag = ProtoField.absolute_time("cortex.tmTimeTag", "Time tag", base.UTC),
	seqCount = ProtoField.uint32("cortex.tmSequenceCounter", "Sequence counter", base.DEC),
	frmCheckRes = ProtoField.uint32("cortex.tmFrameCheckResult", "Frame check result", base.DEC),
	numRS_ErrorsCorrected = ProtoField.uint32("cortex.tmNumRS_ErrorsCorrected", "Number of R-S errors corrected", base.DEC, nil, 0xff00),
	frmCheckStatus = ProtoField.bool("cortex.tmFrmCheckStatus", "Frame checking status", 32, FrameCheckingValues, 0x2),
	rsCorrectionStatus = ProtoField.bool("cortex.tmRsCorrectionStatus", "R-S correction status", 32, RS_CorrectionValues, 0x1),
	TC_DecoderStatus = ProtoField.uint32("cortex.tmTC_DecoderStatus", "Turbo-Code decoder status", base.DEC),
	numLDPC_ErrorsCorrected = ProtoField.uint16("cortex.tmNumLDPC_ErrorsCorrected", "Number of LDPC errors corrected", base.DEC),
	LDPC_DecoderStatus = ProtoField.uint8("cortex.tmLDPC_DecoderStatus", "LDPC decoder status", base.DEC, ldpcStatusValues),
	frmCheckStatus1 = ProtoField.uint8("cortex.tmFrmCheckStatus", "Frame check status", base.DEC),

	frmSyncStatus = ProtoField.uint32("cortex.tmFrameSyncStatus", "Frame synchronizer status", base.DEC),
	bitSlip = ProtoField.uint32("cortex.tmBitSlip", "Bit slip", base.DEC),
	delay = ProtoField.uint32("cortex.tmDelay", "TM delay", base.DEC),
	frmLength = ProtoField.uint32("cortex.tmFrameLength", "Frame length (bytes)", base.DEC),
	syncWordLength = ProtoField.uint32("cortex.tmSyncWordLength", "Synchronization word length (bits)", base.DEC),
	rsDecoderStatus = ProtoField.bool("cortex.tmRsStatus", "R-S decoder", 32, OnOffValues, 0x1),
	CrcVerification = ProtoField.bool("cortex.tmCRCVerification", "CRC Verification", 32, OnOffValues, 0x2),
	checksumVerification = ProtoField.bool("cortex.tmChecksumVerification", "Checksum Verification", 32, OnOffValues, 0x4),
	turboDecoder = ProtoField.bool("cortex.tmTurboDecoder", "Turbo decoder", 32, OnOffValues, 0x8),
	LDPCDecoder = ProtoField.bool("cortex.tmLDPCDecoder", "LDPC decoder", 32, OnOffValues, 0x10),
	TxData = ProtoField.none("cortex.tmTxData", "Transmitted data", base.NONE),
	dataRSContents = ProtoField.bool("cortex.tmRS_Data", "RS data", 32, RS_Values, 0x1),
	numErrorsAvail = ProtoField.bool("cortex.tmNumErrorsAvail", "Number of corrected errors available", 32, OnOffValues, 0x2),
	dataLDPCContents = ProtoField.bool("cortex.tmLDPC_Data", "LDPC data", 32, LDPC_Values, 0x1),
	unused = ProtoField.none("cortex.tmUnused", "Unused", base.NONE),
	tmFrame = ProtoField.none("cortex.TM_Frame", "CCSDS TeleMetry Frame", base.NONE),
	syncMarker = ProtoField.uint32("cortex.syncMarker", "Frame Synchronisation Marker", base.HEX),

	-- Trailer
	postamble = ProtoField.int32("cortex.postamble", "Postamble", base.DEC),
}

-- Register our fields
cortex_tm_protocol.fields = tm_fields

local tm_subdissector = nil

-- Convert a channel code into a channel name
local function get_TM_channelName(tm_channel)
  local retChannelName = "Unknown"

  if (tm_channel < 0 or tm_channel > 5) then
    return retChannelNam
  end
  
  local retChannelCode = string.char(string.byte('A')+tm_channel)
  
  retChannelName = "TM channel " .. retChannelCode
  
  return retChannelName
end

local function get_TM_DataFlowName(dataFlow)
  local retDataFlowName = "Reserved"

      if dataFlow ==  0 then retDataFlowName = "Real-Time permanent flow"
  elseif dataFlow ==  1 then retDataFlowName = "Real-Time single block/frame Tx"
  elseif dataFlow ==  2 then retDataFlowName = "Real-Time permanent flow + dummy TM"
  elseif dataFlow ==  4 then retDataFlowName = "Offline permanent flow"
  elseif dataFlow ==  5 then retDataFlowName = "Offline single block/frame Tx"
  elseif dataFlow ==  6 then retDataFlowName = "Offline permanent flow + dummy TM"
  elseif dataFlow == 80 then retDataFlowName = "Stop TM"
  end
  
  return retDataFlowName
end

local function get_TM_SyncStatusName(syncStatus)
  local retSyncStatusName = "Unknown"

      if syncStatus == 0 then retSyncStatusName = "Frame sync. Off"
  elseif syncStatus == 1 then retSyncStatusName = "Frame sync. On, Lock"
  elseif syncStatus == 2 then retSyncStatusName = "Frame sync. On, FlyWheel"
  elseif syncStatus == 4 then retSyncStatusName = "End of data (Offline TM)"
  end
  
  return retSyncStatusName
end

-- This function is made common to Turbo-code decoding or no decoder at all by ignoring the
-- unused case (1) for Frame check OK

local function get_TurboCodeStatusName(turboStatus)
  local retTurboCodeStatusName = "Unknown"

      if turboStatus == 0 or turboStatus == 1 then retTurboCodeStatusName = "Frame check OK"
  elseif turboStatus == 2 then retTurboCodeStatusName = "Frame check error"
  end
  
  return retTurboCodeStatusName
end

local function enableTM_SubDissector()
	tm_subdissector = Dissector.get(default_settings.subdissector)

	if tm_subdissector == nil then
		tm_debug:show(debugLevel.LEVEL_1, default_settings.subdissector .. " subdissector not found")
	end

end

function cortex_tm_protocol.dissector(buffer, pinfo, tree)
  length = buffer:len()
  if length == 0 then return end

  pinfo.cols.protocol = cortex_tm_protocol.name

  local subtree = tree:add(cortex_tm_protocol, buffer(), "Cortex TM Protocol Data")
  local headerSubtree = subtree:add(cortex_tm_protocol, buffer(0,12), "Header")
  local payloadSubtree = subtree:add(cortex_tm_protocol, buffer(12,length-16), "Payload")
  local trailerSubtree = subtree:add(cortex_tm_protocol, buffer(length-4,4), "Trailer") 

    -- Display preamble information
  local preambleCode = buffer(0,4):int()
  local preambleOK
  if preambleCode == 1234567890 then preambleOK = "OK" else preambleOK = "KO" end
 
  headerSubtree:add(tm_fields.preamble, buffer(0,4)):append_text(" (" .. preambleOK .. ")")
  headerSubtree:add(tm_fields.messageLength, buffer(4,4))  
  headerSubtree:add(tm_fields.flowId, buffer(8,4))

  -- Decode payload
  local msgLength = buffer(4,4):int()
  tm_debug:show(debugLevel.LEVEL_2, "Message length: " .. tostring(msgLength))

  local payloadType = "Unknown message"
  -- Simple heuristics to determine message type
  if (msgLength == 64) then
    -- TM request message for sure
    local channel = buffer(12,4):int()
    local dataFlow = buffer(20,4):int()
	
	payloadType = "TM request message"
    payloadSubtree:add(tm_fields.channel, buffer(12,4)):append_text(" (" .. get_TM_channelName(channel) .. ")")
    payloadSubtree:add(tm_fields.number, buffer(16,4))
    payloadSubtree:add(tm_fields.dataFlow, buffer(20,4)):append_text(" (" .. get_TM_DataFlowName(dataFlow) .. ")")
    payloadSubtree:add(tm_fields.frameMask, buffer(24,8))
    payloadSubtree:add(tm_fields.expectedValue, buffer(32,8))
    payloadSubtree:add(tm_fields.startTimeTag, buffer(40,8), UTC_with_msec(buffer(40,8)))
    payloadSubtree:add(tm_fields.endTimeTag, buffer(48,8), UTC_with_msec(buffer(48,8)))
    payloadSubtree:add(tm_fields.firstFile, buffer(56,2))
    payloadSubtree:add(tm_fields.numFiles, buffer(58,2))
 
  elseif (msgLength == 20) then
    -- TM Nack or STOP TM Ack
	local retCode = buffer(12,4):int()
	if retCode == 0 then
	  payloadType = "Telemetry Stop TM ACK message"
    else
	  payloadType = "Telemetry NACK message"
	end
	payloadSubtree:add(tm_fields.retCode, buffer(12,4))
  elseif (msgLength > 64) then
    -- Looks like a TM message whether real or dummy (dummy is only 68 bytes)
	local RealTM = 1
	if msgLength == 68 then
	  payloadType = "Dummy Telemetry message"
	  RealTM = 0
	else
	  payloadType = "Telemetry message"
	end
	--Time is encoded as UTC on the 1st 32 bytes word  and then msecs on the next 32 bytes word
	payloadSubtree:add(tm_fields.timeTag, buffer(12,8), UTC_with_msec(buffer(12,8)))
	payloadSubtree:add(tm_fields.seqCount, buffer(20,4))
	-- Frame check result is dependent on word #12, i.e. buffer(48,4)
	-- Logic is not so simple
	local frmStatus = buffer(48,4):uint()
	if frmStatus == 0 then
	  payloadSubtree:add(tm_fields.frmCheckRes, buffer(24,4)):append_text(" (Not significant)")
	else
	  local frmCheckSubtree = payloadSubtree:add(cortex_tm_protocol, buffer(24,4), "Frame check result")
	  if bit.band(frmStatus, 0x1) ~= 0 then
	    -- R-S decoder ON
		local txData = buffer(52,4):uint()
		if bit.band(txData, 0x2) ~= 0 then
		  frmCheckSubtree:add(tm_fields.numRS_ErrorsCorrected, buffer(24,4))
		end
		frmCheckSubtree:add(tm_fields.frmCheckStatus, buffer(24,4))
		frmCheckSubtree:add(tm_fields.rsCorrectionStatus, buffer(24,4))
	  elseif bit.band(frmStatus, 0x8) ~= 0 then
	    -- Turbocode decoder ON	  
		local turboStatus = buffer(27,1):uint()
		frmCheckSubtree:add(tm_fields.TC_DecoderStatus,buffer(24,4)):append_text(" (" .. get_TurboCodeStatusName(turboStatus) .. ")")
	  elseif bit.band(frmStatus, 0x10) ~= 0 then
	    -- LDPC decoder ON
		local errorsCorrected = buffer(25,2):uint()
		local LDPC_info = ""
		if errorsCorrected == 0xffff then
		    LDPC_info = " (LDPC correction capacity exceeded)"
		end
      	frmCheckSubtree:add(tm_fields.numLDPC_ErrorsCorrected, buffer(25,2)):append_text(LDPC_info)
		local ldpcStatus = buffer(27,1):uint()
      	frmCheckSubtree:add(tm_fields.LDPC_DecoderStatus, buffer(27,1))
	  else
	    -- No decoder
		local frmChkStatus = buffer(27,1):uint()
		frmCheckSubtree:add(tm_fields.frmCheckStatus1, buffer(27,1)):append_text(" (" .. get_TurboCodeStatusName(frmChkStatus) .. ")")
	  end
	end
	-- Decode synchronizer status
	local syncStatus = buffer(28,4):int()
	payloadSubtree:add(tm_fields.frmSyncStatus, buffer(28,4)):append_text(" (" .. get_TM_SyncStatusName(syncStatus) .. ")")
	-- Decode bit slip status
	local bitSlip =  buffer(32,4):int()
	local bitSlipStatus = "Unknown"
	if bitSlip == 0 then
	  bitSlipStatus = "No bit slip"
	elseif bitSlip == 1 then
	  bitSlipStatus = "Bit slip occured"
	end
	if RealTM == 1 then
		payloadSubtree:add(tm_fields.bitSlip, buffer(32,4)):append_text(" (" .. bitSlipStatus .. ")")
		payloadSubtree:add(tm_fields.delay, buffer(36,4))
	else
		payloadSubtree:add(tm_fields.unused, buffer(32,8))
	end
	payloadSubtree:add(tm_fields.frmLength, buffer(40,4))
	if RealTM == 1 then
	    local length = buffer:len()
		payloadSubtree:add(tm_fields.syncWordLength, buffer(44,4))
		-- Frame check & decoding status display
		local frmStatusSubtree = payloadSubtree:add(cortex_tm_protocol, buffer(48,4), "Frame check and decoder status")
		frmStatusSubtree:add(tm_fields.LDPCDecoder, frmStatus)
		frmStatusSubtree:add(tm_fields.turboDecoder, frmStatus)
		frmStatusSubtree:add(tm_fields.checksumVerification, frmStatus)
		frmStatusSubtree:add(tm_fields.CrcVerification, frmStatus)
		frmStatusSubtree:add(tm_fields.rsDecoderStatus, frmStatus)
		-- Tx Data display
		if bit.band(frmStatus,0x11) ~= 0 then
		  local txDataSubtree = payloadSubtree:add(cortex_tm_protocol, buffer(52,4), "Transmitted data")
		  if bit.band(frmStatus,0x1) ~= 0 then
			txDataSubtree:add(tm_fields.numErrorsAvail, buffer(52,4))
			txDataSubtree:add(tm_fields.dataRSContents, buffer(52,4))
		  else
			txDataSubtree:add(tm_fields.dataLDPCContents, buffer(52,4))
		  end
		else
		  payloadSubtree:add(tm_fields.TxData, buffer(52,4)):append_text(" (Unused)")
		end
		payloadSubtree:add(tm_fields.unused, buffer(56,8))
		payloadSubtree:add(tm_fields.tmFrame, buffer(64,length-68))
		-- Subdissect TM Frame if enabled
		if default_settings.subdissect then
			if tm_subdissector == nil then
			  enableTM_SubDissector()
			end
			local tmFrameTree = payloadSubtree:add(cortex_tm_protocol, buffer(64,length-68), "Transfer Frame")
			tmFrameTree:add(tm_fields.syncMarker, buffer(64,4))
			-- Skip synchronisation marker
			tm_subdissector(buffer(68, length-72):tvb(), pinfo, tmFrameTree)
		end
	else
		-- Dummy TM
		payloadSubtree:add(tm_fields.unused, buffer(44,20))
	end
  end
  payloadSubtree:append_text(" (" .. payloadType .. ")") 
	
  -- Decode postamble  
  local postambleCode = buffer(length-4,4):int()
  local postambleOK
  if postambleCode == -1234567890 then postambleOK = "OK" else postambleOK = "KO" end
  trailerSubtree:add(tm_fields.postamble, buffer(length-4,4)):append_text(" (" .. postambleOK .. ")")

end

local function enableTM_Dissector()
    -- Using DissectorTable:set() removes existing dissector(s), whereas the
    -- DissectorTable:add() one adds ours before any existing ones, but
    -- leaves the other ones alone, which is better
    DissectorTable.get("tcp.port"):add(default_settings.port, cortex_tm_protocol)
end
-- Call it now, because we're enabled by default
enableTM_Dissector()

local function disableTM_Dissector()
    DissectorTable.get("tcp.port"):remove(default_settings.port, cortex_tm_protocol)
end

local debug_pref_enum = {
    { 1,  "Disabled", debugLevel.DISABLED },
    { 2,  "Level 1",  debugLevel.LEVEL_1  },
    { 3,  "Level 2",  debugLevel.LEVEL_2  },
}

-- Register our preferences

cortex_tm_protocol.prefs.enabled     = Pref.bool("Dissector enabled", default_settings.enabled,
                                        "Whether the FPM dissector is enabled or not")

cortex_tm_protocol.prefs.port     = Pref.uint("TC port", default_settings.port,
                                        "Define the TCP port used for TeleMetries")
										
cortex_tm_protocol.prefs.subdissect  = Pref.bool("Enable sub-dissectors", default_settings.subdissect,
                                        "Whether the Cortex TM packet's content" ..
                                        " should be dissected or not using CCSDS dissector")

cortex_tm_protocol.prefs.subdissector  = Pref.string("CCSDS TM dissector", default_settings.subdissector,
                                        "CCSDS dissector to call to dissect TM")
										
cortex_tm_protocol.prefs.debug       = Pref.enum("Debug", default_settings.debug_level,
                                        "The debug printing level", debug_pref_enum)
										
function cortex_tm_protocol.prefs_changed()

    default_settings.subdissect  = cortex_tm_protocol.prefs.subdissect
	default_settings.debug_level = cortex_tm_protocol.prefs.debug
	tm_debug:setLevel(default_settings.debug_level)

    if default_settings.enabled ~= cortex_tm_protocol.prefs.enabled then
        default_settings.enabled = cortex_tm_protocol.prefs.enabled
        if default_settings.enabled then
            enableTM_Dissector()
        else
            disableTM_Dissector()
        end
        -- have to reload the capture file for this type of change
        reload()
    end

	if default_settings.port ~= cortex_tm_protocol.prefs.port then
		if default_settings.enabled then
			disableTM_Dissector()
			default_settings.port = cortex_tm_protocol.prefs.port
			enableTM_Dissector()
			-- have to reload the capture file for this type of change
			reload()
		else
			default_settings.port = cortex_tm_protocol.prefs.port
		end
    end

	if default_settings.subdissector ~= cortex_tm_protocol.prefs.subdissector then
		default_settings.subdissector = cortex_tm_protocol.prefs.subdissector
		enableTM_SubDissector()
		-- have to reload the capture file for this type of change
		reload()
    end
	
end
