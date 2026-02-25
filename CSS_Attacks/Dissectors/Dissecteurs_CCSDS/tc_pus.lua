-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENCE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENCE.md’.

-- Optional verification of PEC field

require("crc16")

local default_settings =
{
    check_pec = false,      -- whether we check Space Packets PEC or not
}

local pus_tc_frame_protocol = Proto("TC_PUS", "TC Packet utilization Service")

local f = pus_tc_frame_protocol.fields
local enums = {}

-- import services table
local services = require("pus_service_table")

-- Block Size
local TC_PUS_HEADER_LENGTH = 5
local PEC_LENGTH = 2

-- Header field
f.packet_version_number = ProtoField.uint8("pus_tc.packet_version_number", "PVN", base.DEC, nil, 0xf0)
f.acknowledgment_flag = ProtoField.uint8("pus_tc.acknowledgment_flag", "Acknowledgment Flag", base.DEC, nil, 0x0f)
f.service_type_id = ProtoField.uint8("pus_tc.service_type_id", "Service Type ID", base.DEC)
f.message_subtype_id = ProtoField.uint8("pus_tc.message_subtype_id", "Message Subtype ID", base.DEC)
f.message_type_counter = ProtoField.uint16("pus_tc.message_type_counter", "Message Type Counter", base.DEC)
f.source_id = ProtoField.uint16("pus_tc.source_id", "Source ID", base.DEC)
f.destination_id = ProtoField.uint16("pus_tc.destination_id", "Destination ID", base.DEC)
f.spacecraft_time_reference_number = ProtoField.uint8("pus_tm.spacecraft_time_reference_number",
    "Spacecraft Time Reference Number", base.DEC, nil, 0x0f)
f.time = ProtoField.uint64("pus_tc.time", "Time", base.DEC)
-- PEC field
f.pec = ProtoField.uint16("pus_tc.packet_error_control", "Packet Error Control", base.HEX)


local function get_service_name(buffer)
    local service = "st" .. buffer(1, 1):uint()
    local service_name = "Unknown"
    if services[service] ~= nil then
        service_name = services[service].name
    end
    return service_name
end

local function get_sub_service_name(buffer)
    local service = "st" .. buffer(1, 1):uint()
    local sub_service = "sub" .. buffer(2, 1):uint()
    local sub_service_name = "Unknown"
    if services[service] ~= nil and service ~= "st10" and services[service].sub_services[sub_service] ~= nil then
        sub_service_name = services[service].sub_services[sub_service]
    end
    return sub_service_name
end

---------------
-- Dissector --
---------------

function pus_tc_frame_protocol.dissector(buffer, pinfo, tree)
    -- Skip SPP header (since we have passed the whole encapsulating SPP for PEC verification)
    local offset = SPP_HEADER_LENGTH
    local length = buffer:len() - SPP_HEADER_LENGTH

    if length == 0 then return end

    pinfo.cols.protocol = pus_tc_frame_protocol.name

    -- add a tab PUS
    local tab = ByteArray.tvb(buffer:bytes(offset, length), "PUS")

    local user_data_buffer = nil
    local header_length = TC_PUS_HEADER_LENGTH
    local header_buffer = buffer:range(offset, header_length)
    local header_tree = tree:add(pus_tc_frame_protocol, header_buffer(), "Header")

    header_tree:add(f.packet_version_number, buffer(offset, 1))
    header_tree:add(f.acknowledgment_flag, buffer(offset, 1))
    offset = offset + 1
    header_tree:add(f.service_type_id, buffer(offset, 1)):append_text(" (" .. get_service_name(header_buffer) .. ")")
    offset = offset + 1
    header_tree:add(f.message_subtype_id, buffer(offset, 1)):append_text(" (" ..
    get_sub_service_name(header_buffer) .. ")")
    offset = offset + 1
    header_tree:add(f.source_id, buffer(offset, 1))
    offset = offset + 1

    user_data_buffer = buffer:range(offset, length - header_length - PEC_LENGTH):tvb()

    local data_tree = tree:add(pus_tc_frame_protocol, user_data_buffer(), "Service data")
    Dissector.get("data"):call(user_data_buffer, pinfo, data_tree)

    offset = offset + user_data_buffer:len()

    -- Optional PEC verification. Can be desactivated if needed for performance reasons.
    local pecVerification = " [unverified]"
    -- PEC is computed over the whole encapsulating SPP
    local crc = crc16(buffer)
    if default_settings.check_pec == true then
        pecVerification = " [OK]"
        if crc ~= 0 then
            pecVerification = " [KO]"
        end
    end
    tree:add(f.pec, buffer(offset, PEC_LENGTH)):append_text(pecVerification)
end

local udp_port = DissectorTable.get("udp.port")
local tcp_port = DissectorTable.get("tcp.port")
udp_port:add(50000, pus_tc_frame_protocol)
tcp_port:add(50000, pus_tc_frame_protocol)

-- Register our preferences
pus_tc_frame_protocol.prefs.check_pec = Pref.bool("Check Space Packet PEC", default_settings.check_pec,
    "Whether we check Space Packet PEC or not")

function pus_tc_frame_protocol.prefs_changed()
    default_settings.check_pec = pus_tc_frame_protocol.prefs.check_pec
end
