------------------------------------------------------------------
--
-- CSS project constellation network layer protocol dissector
--
-- Written by Pierre Bacquet, Thales Alenia Space
--
------------------------------------------------------------------



local default_settings =
{
	tc_dissector = "tc_frame",  -- sub-dissector to be called for TC
	tm_dissector = "tm_frame",  -- sub-dissector to be called for TM
}

css_netw_protocol = Proto("CSS_Netw",  "CSS Constellation Network Protocol")

-- a table of all of our Protocol's fields
local f =
{	
	-- Header
	ttl = ProtoField.int8("CSS_Netw.ttl", "Time to live", base.DEC),
	-- Message
	syncMarker = ProtoField.uint32("CSS_Netw.syncMarker", "TM Frame Synchronisation Marker", base.HEX),
}

-- Register our fields
css_netw_protocol.fields = f

local tc_subdissector = nil
local tm_subdissector = nil

--------------------------------------------------------------------------
-- Helper functions to get the dissectors to be used for CCSDS TC and TM
-- The code takes care of the fact that CCSDS TC and TM may not already be
-- loaded at the time the CSS network dissector is loaded. Hence, this code
-- is to be called each time a CSS network packet is to be sub-dissected.
--------------------------------------------------------------------------

local function getTC_SubDissector()
    if tc_subdissector == nil then
        local sub_dissector

        tc_subdissector = Dissector.get("data")
        if default_settings.tc_dissector ~= '' then
            sub_dissector = Dissector.get(default_settings.tc_dissector)
            if sub_dissector ~= nil then
                tc_subdissector = sub_dissector
            end
        end
    end

    return tc_subdissector
end

local function getTM_SubDissector()
    if tm_subdissector == nil then
        local sub_dissector

        tm_subdissector = Dissector.get("data")
        if default_settings.tm_dissector ~= '' then
            sub_dissector = Dissector.get(default_settings.tm_dissector)
            if sub_dissector ~= nil then
                tm_subdissector = sub_dissector
            end
        end
    end

    return tm_subdissector
end

function css_netw_protocol.dissector(buffer, pinfo, tree)
	local length = buffer:len()
	if length <= 5 then return end

    local subtree = tree:add(css_netw_protocol, buffer(), "CSS Network layer")
	local offset = 0
	local subDissector
	
	pinfo.cols.protocol = css_netw_protocol.name
	subtree:add(f.ttl, buffer(offset, 1))
	offset = offset + 1
	-- Get 4 next bytes to infer type (TM or TC)
	local tm_asm
	tm_asm = buffer(offset,4):uint()
	if tm_asm == 0x1acffc1d then
		-- TM frame
		subtree:add(f.syncMarker, buffer(offset,4))
		offset = offset + 4
		subDissector = getTM_SubDissector()
	else
		-- TC frame
		subDissector = getTC_SubDissector()
	end
	local data_buffer = buffer:range(offset, length - offset):tvb()
	subDissector(data_buffer, pinfo, subtree)
	
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(5011, css_netw_protocol)
udp_port:add(5012, css_netw_protocol)

-- Register our preferences

css_netw_protocol.prefs.tc_dissector  = Pref.string("CCSDS TC dissector", default_settings.tc_dissector,
                                        "CCSDS dissector to call to dissect TC")
css_netw_protocol.prefs.tm_dissector  = Pref.string("CCSDS TM dissector", default_settings.tm_dissector,
                                        "CCSDS dissector to call to dissect TM")
										
function css_netw_protocol.prefs_changed()

    if default_settings.tc_dissector ~= css_netw_protocol.prefs.tc_dissector then
        default_settings.tc_dissector = css_netw_protocol.prefs.tc_dissector
        -- Force recomputing dissector reference
        tc_subdissector = nil
        -- Have to reload the capture file for this type of change
        reload()
    end

    if default_settings.tm_dissector ~= css_netw_protocol.prefs.tm_dissector then
        default_settings.tm_dissector = css_netw_protocol.prefs.tm_dissector
        -- Force recomputing dissector reference
        tm_subdissector = nil
        -- Have to reload the capture file for this type of change
        reload()
    end
end
