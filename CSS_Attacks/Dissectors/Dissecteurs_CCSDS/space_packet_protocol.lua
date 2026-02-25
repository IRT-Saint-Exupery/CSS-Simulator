-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENCE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENCE.md’.

-- Our preferences

local default_settings =
{
    tc_dissector = "tc_pus", -- sub-dissector to be called for TC (default PUS TC)
    tm_dissector = "tm_pus", -- sub-dissector to be called for TM (default PUS TM)
}

local space_packet_protocol = Proto("space_packet_protocol", "Space Packet Protocol")

local f = space_packet_protocol.fields
local enums = {}
local tc_subdissector = nil
local tm_subdissector = nil

------------------------------------------------------------------------
-- Helper functions to get the dissectors to be used for PUS TC and TM
-- The code takes care of the fact that PUS TC and TM may not already be
-- loaded at the time the SPP dissector is loaded. Hence, this code is
-- to be called each time a SPP is to be sub-dissected.
------------------------------------------------------------------------

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

-- Block size
-- SPP header length is not made local since it will be used by PUS dissectors
SPP_HEADER_LENGTH = 6

enums.packet_type = {
    [0] = "Telemetry",
    [1] = "Telecommand",
}

enums.sequence_flags = {
    [0] = "Continuation segment",
    [1] = "First segment",
    [2] = "Last segment",
    [3] = "Unsegmented data",
}

-- Header
f.packet_version_number = ProtoField.uint16("space_packet_protocol.packet_version_number", "Version", base.DEC, nil,
    0xe000)
f.packet_type = ProtoField.uint16("space_packet_protocol.packet_type", "Type", base.DEC, enums.packet_type, 0x1000)
f.secondary_header_flag = ProtoField.uint16("space_packet_protocol.secondary_header_flag", "Secondary Header", base.DEC,
    nil, 0x0800)
f.application_process_identifier = ProtoField.uint16("space_packet_protocol.application_process_identifier", "APID",
    base.DEC, nil, 0x007ff)
f.sequence_flags = ProtoField.uint16("space_packet_protocol.sequence_flags", "Sequence Flags", base.DEC,
    enums.sequence_flags, 0xc000)
f.packet_sequence_count = ProtoField.uint16("space_packet_protocol.packet_sequence_count", "Packet Sequence Count",
    base.DEC, nil, 0x3fff)
f.packet_data_length = ProtoField.uint16("space_packet_protocol.packet_data_length", "Packet Data Length", base.DEC, nil,
    0xffff)

-------------------------
-- Heuristic Dissector --
-------------------------

function space_packet_protocol_heuristic_dissector(buffer, pinfo, tree)
    local length_buffer = buffer:len()
    if length_buffer < 6 then return false end
    local pvn = buffer(0, 1):bitfield(0, 3)
    if pvn ~= 0 then return false end

    local length_spp = buffer(4, 2):uint()
    if (length_spp + SPP_HEADER_LENGTH + 1) ~= length_buffer then return false end

    space_packet_protocol.dissector(buffer, pinfo, tree)
    return true
end

---------------
-- Dissector --
---------------

function space_packet_protocol.dissector(buffer, pinfo, tree)
    local length_buffer = buffer:len()
    if length_buffer < 6 then return end
    pinfo.cols.protocol = space_packet_protocol.name
    pinfo.cols.info = "Data"

    -- add a tab SPP
    local tab = ByteArray.tvb(buffer:bytes(), "SPP")

    local spp_tree = tree:add(space_packet_protocol, buffer(), "Space Packet Protocol")

    local offset = 0

    local header_buffer = buffer:range(offset, SPP_HEADER_LENGTH):tvb()
    local header_tree = spp_tree:add(space_packet_protocol, header_buffer(), "Primary SPP Header")

    header_tree:add(f.packet_version_number, buffer(offset, 2))
    header_tree:add(f.packet_type, buffer(offset, 2))
    header_tree:add(f.secondary_header_flag, buffer(offset, 2))
    local secondary_header_flag = buffer(offset, 1):bitfield(4, 1)
    header_tree:add(f.application_process_identifier, buffer(offset, 2))
    local apid = buffer(offset, 2):bitfield(5, 11)
    offset = offset + 2
    header_tree:add(f.sequence_flags, buffer(offset, 2))
    header_tree:add(f.packet_sequence_count, buffer(offset, 2))
    offset = offset + 2
    header_tree:add(f.packet_data_length, buffer(offset, 2))
    offset = offset + 2

    local data_buffer = buffer:range(offset, buffer:len() - offset):tvb()
    -- Only Idle Data
    if apid == 2047 then
        local oid_data_tree = spp_tree:add(space_packet_protocol, data_buffer(), "IDLE Data")
        Dissector.get("data"):call(data_buffer, pinfo, oid_data_tree)
    else
        if secondary_header_flag == 1 then
            -- We suppose that a second header indicates a PUS frame
            local subDissector

            local pus_data_tree = spp_tree:add(space_packet_protocol, data_buffer(), "Packet Utilisation Service")
            type1 = buffer(0, 1):bitfield(3, 1)
            if type1 == 1 then
                subDissector = getTC_SubDissector()
            else
                subDissector = getTM_SubDissector()
            end
            -- Though checksum (PEC) is optional at PUS level, it is computed over the whole
            -- Space Packet Protocol structure. Hence, we pass to PUS dissectors the whole SPP
            subDissector(buffer, pinfo, pus_data_tree)
        else
            local user_data_tree = spp_tree:add(space_packet_protocol, data_buffer(), "User Data")
            pinfo.private.packet_type = buffer(0, 1):bitfield(3, 1)
            Dissector.get("data"):call(data_buffer, pinfo, user_data_tree)
        end
    end
end

-- Defer loading custom dissector table stuff, as plugins are loaded later than built in stuff
function space_packet_protocol.init()
    -- local table = DissectorTable.get("tc_frame.vc_id")
    -- table:add_for_decode_as(space_packet_protocol)
    -- table:add(0, space_packet_protocol)
    -- DissectorTable.get("udp.port"):add_for_decode_as(table)
end

DissectorTable.get("ccsds.pvn"):add_for_decode_as(space_packet_protocol)
DissectorTable.get("udp.port"):add_for_decode_as(space_packet_protocol)
DissectorTable.get("tcp.port"):add_for_decode_as(space_packet_protocol)
space_packet_protocol:register_heuristic("udp", space_packet_protocol_heuristic_dissector)
space_packet_protocol:register_heuristic("tcp", space_packet_protocol_heuristic_dissector)

-- Register our preferences

space_packet_protocol.prefs.tc_dissector = Pref.string("Space Packet TC dissector", default_settings.tc_dissector,
    "Dissector to call to dissect Space Packets containing TC")

space_packet_protocol.prefs.tm_dissector = Pref.string("Space Packet TM dissector", default_settings.tm_dissector,
    "Dissector to call to dissect Space Packets containing TM")

function space_packet_protocol.prefs_changed()
    if default_settings.tc_dissector ~= space_packet_protocol.prefs.tc_dissector then
        default_settings.tc_dissector = space_packet_protocol.prefs.tc_dissector
        -- Force recomputing dissector reference
        tc_subdissector = nil
        -- Have to reload the capture file for this type of change
        reload()
    end

    if default_settings.tm_dissector ~= space_packet_protocol.prefs.tm_dissector then
        default_settings.tm_dissector = space_packet_protocol.prefs.tm_dissector
        -- Force recomputing dissector reference
        tm_subdissector = nil
        -- Have to reload the capture file for this type of change
        reload()
    end
end
