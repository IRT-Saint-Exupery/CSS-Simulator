-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENCE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENCE.md’.
------------------------------------------------------------------

require("crc16")

local default_settings =
{
    has_pec   = true,  -- whether Space Packets include PEC or not
    check_pec = false, -- whether we check Space Packets PEC or not
}

local pus_tm_frame_protocol = Proto("TM_PUS", "TM Packet utilization Service")

local f = pus_tm_frame_protocol.fields
local enums = {}

-- import services table
local services = require("pus_service_table")

-- Block Size
local TM_HEADER_LENGTH_WHITHOUT_TIME = 7
local PEC_LENGTH = 2
local TIME_FIELD_SIZE = 0

-- Header field
f.packet_version_number = ProtoField.uint8("pus_tm.packet_version_number", "PVN", base.DEC, nil, 0xf0)
f.acknowledgment_flag = ProtoField.uint8("pus_tm.acknowledgment_flag", "Acknowledgment Flag", base.DEC, nil, 0x0f)
f.service_type_id = ProtoField.uint8("pus_tm.service_type_id", "Service Type ID", base.DEC)
f.message_subtype_id = ProtoField.uint8("pus_tm.message_subtype_id", "Message Subtype ID", base.DEC)
f.message_type_counter = ProtoField.uint16("pus_tm.message_type_counter", "Message Type Counter", base.DEC)
f.destination_id = ProtoField.uint16("pus_tm.destination_id", "Destination ID", base.DEC)
f.spacecraft_time_reference_number = ProtoField.uint8("pus_tm.spacecraft_time_reference_number",
    "Spacecraft Time Reference Number", base.DEC, nil, 0x0f)
f.time = ProtoField.uint64("pus_tm.time", "Time", base.DEC)
-- PEC field
f.pec = ProtoField.uint16("pus_tm.packet_error_control", "Checksum", base.HEX)

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

function pus_tm_frame_protocol.dissector(buffer, pinfo, tree)
    local length = buffer:len()
    if length == 0 then return end

    pinfo.cols.protocol = pus_tm_frame_protocol.name

    -- add a tab PUS
    local tab = ByteArray.tvb(buffer:bytes(), "PUS")

    local offset = 0
    local user_data_buffer = nil
    local header_length = TM_HEADER_LENGTH_WHITHOUT_TIME + TIME_FIELD_SIZE
    if header_length % 2 ~= 0 then
        header_length = header_length + 1
    end
    local header_buffer = buffer:range(offset, header_length)
    local header_tree = tree:add(pus_tm_frame_protocol, header_buffer(), "Header")

    header_tree:add(f.packet_version_number, buffer(offset, 1))
    header_tree:add(f.spacecraft_time_reference_number, buffer(offset, 1))
    offset = offset + 1
    header_tree:add(f.service_type_id, buffer(offset, 1)):append_text(" (" .. get_service_name(header_buffer) .. ")")
    offset = offset + 1
    header_tree:add(f.message_subtype_id, buffer(offset, 1)):append_text(" (" ..
        get_sub_service_name(header_buffer) .. ")")
    offset = offset + 1
    header_tree:add(f.message_type_counter, buffer(offset, 2))
    offset = offset + 2
    header_tree:add(f.destination_id, buffer(offset, 2))
    offset = offset + 2
    if TIME_FIELD_SIZE ~= 0 then
        header_tree:add(f.time, buffer(offset, TIME_FIELD_SIZE))
    end
    offset = offset + TIME_FIELD_SIZE

    if default_settings.has_pec == true then
        user_data_buffer = buffer:range(offset, buffer:len() - offset - PEC_LENGTH):tvb()
    else
        user_data_buffer = buffer:range(offset, buffer:len() - offset):tvb()
    end

    local data_tree = tree:add(pus_tm_frame_protocol, user_data_buffer(), "Service data")
    Dissector.get("data"):call(user_data_buffer, pinfo, data_tree)

    offset = offset + user_data_buffer:len()

    -- Handle optional PEC
    if default_settings.has_pec == true then
        -- Optional PEC verification. Can be desactivated if needed for performance reasons.
        local pecVerification = " [unverified]"
        if default_settings.check_pec == true then
            -- PEC is computed over the whole encapsulating SPP
            local crc = crc16(buffer)
            pecVerification = " [OK]"
            if crc ~= 0 then
                pecVerification = " [KO]"
            end
        end
        tree:add(f.pec, buffer(offset, 2)):append_text(pecVerification)
    end
end

local udp_port = DissectorTable.get("udp.port")
local tcp_port = DissectorTable.get("tcp.port")
udp_port:add(8001, pus_tm_frame_protocol)
tcp_port:add(8010, pus_tm_frame_protocol)

-- Register our preferences
pus_tm_frame_protocol.prefs.has_pec = Pref.bool("Space Packet includes PEC", default_settings.has_pec,
    "Whether each Space Packet has a trailing PEC or not")

pus_tm_frame_protocol.prefs.check_pec = Pref.bool("Check Space Packet PEC", default_settings.check_pec,
    "Whether we check Space Packet PEC or not")

function pus_tm_frame_protocol.prefs_changed()
    default_settings.has_pec   = pus_tm_frame_protocol.prefs.has_pec
    default_settings.check_pec = pus_tm_frame_protocol.prefs.check_pec
end
